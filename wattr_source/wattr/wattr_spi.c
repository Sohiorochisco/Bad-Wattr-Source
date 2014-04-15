#include "headers/wattr_mem.h"
#include "headers/ade7753.h"
#include "sam.h"
#include "headers/wattr_pio.h"
#include "headers/pdc_periph.h"

//Used to indicate the current type of buffer being transferred
enum spiwd{
	IRQ_WRD,
	ZX_WRD,
	COM_WRD,
	NONE
	};

/*Pointer to ADE7753 received data buffer */
static wbuff *ade_rx_buff = 0;
static wbuff *ade_zxrx_buff = 0;
static wbuff *ade_irqrx_buff = 0;
/*received buffer queue struct*/
static queue ade_rx_queue;
static queue ade_tx_queue;
static void *rxq_buff[ADE_RXBUFF_DPTH];
static void *txq_buff[ADE_TXBUFF_DPTH];

//used to indicate current status 
struct ade_fl{
	enum spiwd spiwrd;
};
static struct ade_fl ade_flags;

/*Standard write for IRQ interrupt from ADE7753*/
static uint8_t irq_wr[] = {
	ADE_REG_AENERGY,0x00,0x00,0x00,ADE_REG_RAENERGY,0x00,0x00,0x00
};
static uint8_t zx_wr[] = {
	ADE_REG_VRMS,0x00,0x00,0x00,ADE_REG_IRMS,0x00,0x00,0x00
};

/*Static buffer for a single register write*/
static wbuff *ade_com_buff;
static wbuff ade_zx_buff;
static wbuff ade_irq_buff;

static void config_spi(void)
{
	//enable the spi clock in the power management controller
	PMC->PMC_PCER0 = PMC_PCER0_PID19;
	//reset the channel
	SPI->SPI_CR = SPI_CR_SWRST | SPI_CR_SPIDIS;
	SPI->SPI_CSR[1] |=  SPI_CSR_SCBR(6000);
	/*Ensure that the spi channel is in master mode, and
	 *choose chip select channel zero */
	SPI->SPI_MR = SPI_MR_MSTR | SPI_MR_PCS(13) | 
		SPI_MR_MODFDIS | SPI_MR_WDRBT;
	/* Set clock phase and polarity for the ADE7753 chip select*/
	SPI->SPI_CSR[1] &= ~(SPI_CSR_CPOL) & ~(SPI_CSR_NCPHA);
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
static inline void spi_transfer_wbuff(wbuff *wbp,wbuff *rwbp)
{
	PDC_SPI->PERIPH_TPR		= PERIPH_TPR_TXPTR((uint32_t)wbp->buff);
	PDC_SPI->PERIPH_RPR		= PERIPH_RPR_RXPTR((uint32_t)(rwbp->buff));
	PDC_SPI->PERIPH_RCR		= PERIPH_RCR_RXCTR(wbp->length);
	PDC_SPI->PERIPH_TCR		= PERIPH_TCR_TXCTR(wbp->length);
	SPI->SPI_CSR[1]			&= ~(SPI_CSR_CSNAAT);
	SPI->SPI_IER			= SPI_IER_ENDTX;
	PDC_SPI->PERIPH_PTCR	= PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN;
}

wbuff *ade_read(void)
{
	//Disable the enqueueing interrupt
	NVIC_DisableIRQ(SPI_IRQn);
	wbuff *wb = dequeue(&ade_rx_queue);
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

void make_ade7753_driver(pdc_periph *ade_driver)
{
	ade_zx_buff.buff = zx_wr;
	ade_zx_buff.length = TNY_BLOCK_WL;
	ade_irq_buff.buff = irq_wr;
	ade_irq_buff.length = TNY_BLOCK_WL;
	//Set all flags to zero (because you never know...)
	ade_flags.spiwrd = NONE;
	init_queue(&ade_rx_queue,rxq_buff,ADE_RXBUFF_DPTH);
	init_queue(&ade_tx_queue,txq_buff,ADE_TXBUFF_DPTH);
	config_spi();
	ade_driver->read  = &ade_read;
	ade_driver->write = &ade_write;
	ade_irqrx_buff = alloc_wbuff(TNY_BLOCK_WL);
	ade_zxrx_buff = alloc_wbuff(TNY_BLOCK_WL);
	return;
}

//Handler for received memory from the dynamic access memory controller
static void comms_rxend_handler(void)
{
	SPI->SPI_CSR[1] |= SPI_CSR_CSNAAT;
	PDC_SPI->PERIPH_PTCR |= PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS; 
	wbuff *tx_wr = 0;
	wbuff *rx_wr = 0;
	switch(ade_flags.spiwrd){
	case IRQ_WRD:
		tx_wr = &ade_irq_buff;
		rx_wr = ade_irqrx_buff;
		ade_irqrx_buff = alloc_wbuff(TNY_BLOCK_WL);
		break;
	case ZX_WRD:
		tx_wr = &ade_zx_buff;
		rx_wr = ade_zxrx_buff;
		ade_zxrx_buff = alloc_wbuff(TNY_BLOCK_WL);
		break;
	case COM_WRD:
		if(ade_com_buff){
			uint8_t wr_check = ade_com_buff->buff[0] & ADE_WRITE_MASK;
			if(wr_check){
				free_wbuff(ade_rx_buff);
				ade_rx_buff = 0;
			}else{
				tx_wr = ade_com_buff;
				rx_wr = ade_rx_buff;
				ade_rx_buff = 0;
			}
			free_wbuff(ade_com_buff);
			ade_com_buff = 0;
		}
		break;
	case NONE: break;
	default: break;
	}
	if (tx_wr && rx_wr){
		uint32_t i = 0;
		for(; i < rx_wr->length;i+=4){
			//combine the register addresses with contents
			rx_wr->buff[i] |= tx_wr->buff[i];
		}
		uint32_t st = enqueue(&ade_rx_queue,rx_wr);
		if(st){
			free_wbuff(rx_wr);
		}
		rx_wr = 0;
		free_wbuff(tx_wr);
		tx_wr = 0;
	}
	if(tx_wr){
		free_wbuff(tx_wr);
	}
	if(rx_wr){
		free_wbuff(rx_wr);
	}
	ade_flags.spiwrd = NONE;
	return;		
}

void SPI_Handler(void)
{
	if(SPI->SPI_SR &SPI_SR_ENDTX){
		comms_rxend_handler();
		SPI->SPI_IDR = SPI_IDR_ENDTX;
	}
	return;
}

void ade_irq_handler(void)
{
	if(SPI->SPI_SR & SPI_SR_TXBUFE){
		ade_flags.spiwrd = IRQ_WRD;
		spi_transfer_wbuff(&ade_irq_buff,ade_irqrx_buff);
	}
	return;
}
void ade_zx_handler(void)
{
	if(SPI->SPI_SR & SPI_SR_ENDTX){
		ade_flags.spiwrd = ZX_WRD;
		spi_transfer_wbuff(&ade_zx_buff,ade_zxrx_buff);
	}
}

void service_ade(void)
{
	wbuff *tx_wb = 0; //wbuff to transmit
	if(SPI->SPI_SR & SPI_SR_TXBUFE){
		//see if there is a pending com write
		tx_wb = dequeue(&ade_tx_queue);
		if(tx_wb){
			wbuff *rxnext = alloc_wbuff(tx_wb->length);
			if(rxnext){
				ade_flags.spiwrd = COM_WRD;
				ade_rx_buff = rxnext;
				ade_com_buff = tx_wb;
				spi_transfer_wbuff(tx_wb,ade_rx_buff);
			}else{
				ade_flags.spiwrd = NONE;
			}
		}
	}
	return;
}