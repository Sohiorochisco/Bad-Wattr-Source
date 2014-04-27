#include "headers/wattr_mem.h"
#include "headers/ade7753.h"
#include "sam.h"
#include "headers/wattr_pio.h"
#include "headers/pdc_periph.h"

#define ZX_INDICATOR_MASK PIO_PER_P22
#define RELAY_CONTR_CS 2
#define RELAY_CONTR_TXBUFF_DPTH 4
#define ZX_WRD_LNGTH MED_BLOCK_WL
#define IRQ_WRD_LNGTH MED_BLOCK_WL
//Used to account for the mismatch between the tx and rx buffers
#define SPI_RX_OFFSET 0
//Used to indicate the current type of buffer being transferred
enum spiwd{
	IRQ_WRD,
	ZX_WRD,
	COM_WRD,
	SCRN_WRD,
	NONE
	};

/*Pointer to ADE7753 received data buffer */
static wbuff *ade_rx_buff;
static wbuff *ade_zxrx_buff;
static wbuff *ade_irqrx_buff;
/*received buffer queue struct*/
static queue ade_zxrx_queue;
static queue ade_irqrx_queue;
static queue ade_tx_queue;
static queue screen_tx_queue;
static void *screen_txq_buff[RELAY_CONTR_TXBUFF_DPTH];
static void *ade_irqrxq_buff[ADE_RXBUFF_DPTH];
static void *ade_zxrxq_buff[ADE_RXBUFF_DPTH];
static void *ade_txq_buff[ADE_TXBUFF_DPTH];

//used to indicate current status 
struct ade_fl{
	enum spiwd spiwrd;
};
static struct ade_fl ade_flags;

/*Standard write for IRQ interrupt from ADE7753*/
static uint8_t irq_wr[] = {
	0x00,ADE_REG_AENERGY,0x00,0x00,0x00,ADE_REG_RAENERGY,0x00,0x00,0x00,
	0x00,0x00,0x00
};
static uint8_t zx_wr[] = {
	0x00,ADE_REG_VRMS,0x00,0x00,0x00,ADE_REG_IRMS,0x00,0x00,0x00,
	0x00,0x00,0x00
};

/*Static buffers for one-off ade transfers or screen writes*/
static wbuff *spi_tx_buff;
static wbuff ade_zx_buff;
static wbuff ade_irq_buff;

static void config_spi(void)
{
	int csi = 0;
	//enable the spi clock in the power management controller
	PMC->PMC_PCER0 = PMC_PCER0_PID19;
	//reset the channel
	SPI->SPI_CR = SPI_CR_SWRST | SPI_CR_SPIDIS;
	for(;csi < 4; ++csi){
		/* Set clock phase and polarity for the ADE7753 chip select*/
		SPI->SPI_CSR[csi] &= ~(SPI_CSR_CPOL) & ~(SPI_CSR_NCPHA) & 
		~(SPI_CSR_SCBR_Msk) & ~(SPI_CSR_DLYBCT_Msk);
		SPI->SPI_CSR[csi] |=  SPI_CSR_SCBR(32) | SPI_CSR_DLYBCT(16);
	}
	/*Ensure that the spi channel is in master mode, and
	 *choose chip select channel zero */
	SPI->SPI_MR = SPI_MR_MSTR | SPI_MR_PCS(13) | 
		SPI_MR_MODFDIS | SPI_MR_WDRBT;
	//Enable tranceiver
	SPI->SPI_CR = SPI_CR_SPIEN;
	//Disable PDC
	PDC_SPI->PERIPH_PTCR |= PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS;
	SPI->SPI_IDR = 0xffffffff;
	//Enable the SPI top-level interrupt in the NVIC
	NVIC_SetPriority(SPI_IRQn,8);
	NVIC_EnableIRQ(SPI_IRQn);
	return;
}

//Begins spi transmission
static inline void spi_transfer_wbuff(wbuff *wbp,wbuff *rwbp,uint32_t chp_sel)
{
	PDC_SPI->PERIPH_PTCR	= PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS;
	SPI->SPI_CR				= SPI_CR_SPIDIS;
	SPI->SPI_MR				&= ~(SPI_MR_PCS_Msk);
	//Switch the the correct chip select
	SPI->SPI_MR				|= SPI_MR_PCS(~(0x1u << chp_sel));
	PDC_SPI->PERIPH_TPR		= PERIPH_TPR_TXPTR((uint32_t)((wbp->buff)));
	PDC_SPI->PERIPH_RPR		= PERIPH_RPR_RXPTR((uint32_t)((rwbp->buff)));
	PDC_SPI->PERIPH_RCR		= PERIPH_RCR_RXCTR(wbp->length);
	PDC_SPI->PERIPH_TCR		= PERIPH_TCR_TXCTR(wbp->length);
	SPI->SPI_IER			= SPI_IER_TXEMPTY;
	SPI->SPI_CR				= SPI_CR_SPIEN;
	PDC_SPI->PERIPH_PTCR	= PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN;
}

wbuff *ade_irq_read(void)
{
	//Disable the enqueueing interrupt
	NVIC_DisableIRQ(SPI_IRQn);
	wbuff *wb = dequeue(&ade_irqrx_queue);
	//Re-enable the rx interupt
	NVIC_EnableIRQ(SPI_IRQn);
	return wb;
}

wbuff *ade_zx_read(void)
{
	//Disable the enqueueing interrupt
	NVIC_DisableIRQ(SPI_IRQn);
	wbuff *wb = dequeue(&ade_zxrx_queue);
	//Re-enable the rx interupt
	NVIC_EnableIRQ(SPI_IRQn);
	return wb;
}

uint32_t ade_write(wbuff *wrd)
{
	//Disable dequeueing interrupt
	NVIC_DisableIRQ(SPI_IRQn);
	uint32_t st = enqueue(&ade_tx_queue,wrd);
	NVIC_EnableIRQ(SPI_IRQn);
	return st;
}

uint32_t screen_write(wbuff *wrd)
{
	//Disable dequeueing interrupt
	NVIC_DisableIRQ(SPI_IRQn);
	uint32_t st = enqueue(&screen_tx_queue,wrd);
	NVIC_EnableIRQ(SPI_IRQn);
	return st;
}

void make_spi_driver(pdc_periph *ade_configure, pdc_periph *ade_zxread,
						pdc_periph * ade_irqread,pdc_periph *screen)
{
	ade_zx_buff.buff = zx_wr;
	ade_zx_buff.length = ZX_WRD_LNGTH;
	ade_irq_buff.buff = irq_wr;
	ade_irq_buff.length = TNY_BLOCK_WL;
	//Set all flags to zero (because you never know...)
	ade_flags.spiwrd = NONE;
	init_queue(&ade_zxrx_queue,ade_zxrxq_buff,ADE_RXBUFF_DPTH);
	init_queue(&ade_irqrx_queue,ade_irqrxq_buff,ADE_RXBUFF_DPTH);
	init_queue(&ade_tx_queue,ade_txq_buff,ADE_TXBUFF_DPTH);
	init_queue(&screen_tx_queue,screen_txq_buff,RELAY_CONTR_TXBUFF_DPTH);
	config_spi();
	ade_configure->read  = 0;
	ade_configure->write = &ade_write;
	ade_irqread->read = &ade_irq_read;
	ade_irqread->write = 0;
	ade_zxread->read = &ade_zx_read;
	ade_zxread->write = 0;
	screen->write = &screen_write;
	ade_irqrx_buff = alloc_wbuff(IRQ_WRD_LNGTH);
	ade_zxrx_buff = alloc_wbuff(ZX_WRD_LNGTH);
	return;
}

//Handler for received memory from the dynamic access memory controller
static void comms_rxend_handler(void)
{
	wbuff *rx_wr = 0;
	uint32_t st = 0;
	switch(ade_flags.spiwrd){
	case IRQ_WRD:
		rx_wr = ade_irqrx_buff;
		ade_irqrx_buff = alloc_wbuff(IRQ_WRD_LNGTH);
		st = enqueue(&ade_irqrx_queue,rx_wr);
		break;
	case ZX_WRD:
		rx_wr = ade_zxrx_buff;
		ade_zxrx_buff = alloc_wbuff(ZX_WRD_LNGTH);
		st = enqueue(&ade_zxrx_queue,rx_wr);
		break;
	case SCRN_WRD:
	case COM_WRD:
		if(ade_rx_buff){
			free_wbuff(ade_rx_buff);
		}
		if(spi_tx_buff){
			free_wbuff(spi_tx_buff);
			spi_tx_buff = 0;
		}
		break;
	case NONE: break;
	default: break;
	}
	if(st){
		free_wbuff(rx_wr);
	}
	NVIC_ClearPendingIRQ(PIOC_IRQn);
}

void SPI_Handler(void)
{
	if(SPI->SPI_SR & SPI_SR_TXEMPTY){
		comms_rxend_handler();
		SPI->SPI_IDR = SPI_SR_TXEMPTY;
	}
	NVIC_ClearPendingIRQ(PIOC_IRQn);
	return;
}

void ade_irq_handler(void)
{
	if(SPI->SPI_SR & SPI_SR_TXEMPTY){
		ade_flags.spiwrd = IRQ_WRD;
		spi_transfer_wbuff(&ade_irq_buff,ade_irqrx_buff,1);
	}
	return;
}

void ade_zx_handler(void)
{
	if(SPI->SPI_SR &SPI_SR_TXEMPTY){
		ade_flags.spiwrd = ZX_WRD;
		spi_transfer_wbuff(&ade_zx_buff,ade_zxrx_buff,1);
		NVIC_ClearPendingIRQ(PIOC_IRQn);
	}
	sample_period();
	//toggle the zx indicator LED
	static uint32_t zx_state = ZX_INDICATOR_MASK;
	zx_state = ~zx_state;
	PIOD->PIO_SODR = zx_state & ZX_INDICATOR_MASK;
	PIOD->PIO_CODR = ~zx_state & ZX_INDICATOR_MASK;  
}

void service_ade(void)
{
	wbuff *tx_wb = 0; //wbuff to transmit
	if(SPI->SPI_SR & SPI_SR_TXBUFE){
		//see if there is a pending com write
		tx_wb = dequeue(&ade_tx_queue);
		if(tx_wb){
			ade_rx_buff = alloc_wbuff(tx_wb->length);
			if(ade_rx_buff){
				ade_flags.spiwrd = COM_WRD;
				spi_tx_buff = tx_wb;
				spi_transfer_wbuff(tx_wb,ade_rx_buff,1);
			}else{
				ade_flags.spiwrd = NONE;
				free_wbuff(tx_wb);
			}
		}else{
			tx_wb = dequeue(&screen_tx_queue);
		}
		if(tx_wb){
			ade_rx_buff = alloc_wbuff(tx_wb->length);
			if(ade_rx_buff){
				ade_flags.spiwrd = SCRN_WRD;
				spi_tx_buff = tx_wb;
				spi_transfer_wbuff(tx_wb,ade_rx_buff,RELAY_CONTR_CS);
			}else{
				ade_flags.spiwrd = NONE;
				free_wbuff(tx_wb);
			}
		}
	}
	return;
}

//used to format a register write to the ade7753
void write_ade_reg(uint8_t  *b,uint32_t wrd,uint8_t reg, uint32_t wl)
{
	uint32_t shft_wrd = wrd <<(24 % wl);
	b[0] = reg | ADE_WRITE_MASK;
	b[1] = (uint8_t)shft_wrd >> 16;
	b[2] = (uint8_t)((shft_wrd << 8) >> 16);
	b[3] = (uint8_t)((shft_wrd << 16) >> 16);
	return;
}