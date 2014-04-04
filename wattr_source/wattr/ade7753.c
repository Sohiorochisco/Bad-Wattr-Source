#include "headers/wattr_mem.h"
#include "headers/ade7753.h"
#include "sam.h"
#include "headers/wattr_pio.h"
#include "headers/pdc_periph.h"

//Used to indicate the current type of buffer being transferred
enum spiwd{
	IRQ_WRD;
	ZX_WRD;
	COM_WRD;
	NONE;
	};
/*Static buffer for a single register write*/
static wbuff *ade_com_buff;
const static wbuff ade_zx_buff;
const static wbuff ade_irq_buff;
/*Pointer to ADE7753 received data buffer */
static wbuff *ade_rx_buff;
/*received buffer queue struct*/
static queue ade_rx_queue;
static queue ade_tx_queue;
static void *rxq_buff[ADE_RXBUFF_DPTH];
static void *txq_buff[ADE_TXBUFF_DPTH];
//used to indicate current status 
struct ade_flags{
	uint8_t zx_next;//Indicates zx transfer is requested
	uint8_t irq_next;//Indicates irq_transfer is requested
	enum spiwd spiwrd;
};

/*Standard write for IRQ interrupt from ADE7753*/
const uint8_t irq_wr[] = {
	ADE_REG_AENERGY,0x00,0x00,0x00,ADE_REG_RAENERGY,0x00,0x00,0x00
};
const uint8_t zx_wr[] = {
	ADE_REG_VRMS,0x00,0x00,0x00,ADE_REG_IRMS,0x00,0x00,0x00
};


static void config_spi(void)
{
	//Enable the SPI top-level interrupt in the NVIC
	NVIC_EnableIRQ(SPI_IRQn);
	//Enable the specific interrupts used by the driver
	SPI->SPI_IER = SPI_IER_ENDRX;
	//reset the channel
	SPI->SPI_CR = SPI_CR_SWRST;
	/*Ensure that the spi channel is in master mode, and
	 *choose chip select channel zero */
	SPI->SPI_MR |= SPI_MR_MSTR | SPI_MR_PCS(0x7);
	/* Set clock phase and polarity for the ADE7753 chip select*/
	SPI->SPI_CSR[0] &= ~(SPI_CSR_CPOL) & ~(SPI_CSR_NCPHA);
	//Ensure that the chip select will stay selected between words
	SPI->SPI_CSR[0] |= SPI_CSR_CSAAT;
	return;
}

//Begins spi transmission
static inline spi_transfer_wbuff(wbuff *wbp)
{
	PDC_SPI->PERIPH_TPR = PERIPH_TPR_TXPTR((uint32_t)wbp->buff);
	PDC_SPI->PERIPH_RPR = PERIPH_RPR_RXPTR((uint32_t)(ade_rx_buff->buff));
	PDC_SPI->PERIPH_TCR = PERIPH_TCR_TXCTR(wbp->length);
	PDC_SPI->PERIPH_RCR = PERIPH_RCR_RXCTR(wbp->length);
}

wbuff *ade_read(void)
{
	//Disable the enqueueing interrupt
	SPI->SPI_IDR = SPI_IDR_ENDRX;
	wbuff *wb = dequeue(&ade_rx_queue);
	//Re-enable the rx interupt
	SPI->SPI_IER = SPI_IER_ENDRX;
	return wb;
}

uint32_t ade_write(wbuff *wrd)
{
	//Disable dequeueing interrupt
	SPI->SPI_IDR = SPI_IDR_ENDRX;
	uint32_t st = enqueue(&ade_tx_queue,wrd);
	SPI->SPI_IER = SPI_IER_ENDRX;
	return st;
}

void make_ade7753_driver(pdc_periph *ade_driver)
{
	ade_zx_buff.buff = zx_wr;
	ade_irq_buff.buff = irq_wr;
	//Set all flags to zero (because you never know...)
	ade_flags.irq_next = 0;
	ade_flags.spiwrd = NONE;
	ade_flags.zx_next = 0;
	init_queue(&ade_rx_queue,rxq_buff,ADE_RXBUFF_DPTH);
	init_queue(&ade_tx_queue,txq_buff,ADE_TXBUFF_DPTH);
	config_spi();
	ade_driver->read  = &ade_read;
	ade_driver->write = &ade_write;
	return;
}

//Handler for reloading the dynamic memory access controller, and processing reads
static void comms_rxend_handler(void)
{
	switch(ade_flags.spiwrd){
	IRQ_WRD:
		ade_rx_buff->buff[0] = ade_irq_buff->buff[0];
		ade_rx_buff->buff[4] = ade_irq_buff->buff[4];
		enqueue(&ade_rx_queue,ade_rx_buff);
		break;
	ZX_WRD:
		ade_rx_buff->buff[0] = ade_zx_buff->buff[0];
		ade_rx_buff->buff[4] = ade_zx_buff->buff[4];
		enqueue(&ade_rx_queue,ade_rx_buff);
		break;
	COM_WRD:
		if(ade_com_buff){
			if(!(ade_com_buff->buff[0] | ADE_WRITE_MASK)){
				uint32_t i = 0;
				for(; i < ade_rx_buff->length;i+=4){
					//combine the register addresses with contents
					ade_rx_buff->buff[i] |= (ade_com_buff->buff[i]);
				}
			}else{
				free_wbuff(ade_rx_buff);
			}
			free_wbuff(ade_com_buff);
			ade_com_buff = 0;
		}
		break;
	NONE: break;
	default: break;
	}
	ade_rx_buff = 0;
	ade_flags.spiwrd = NONE;
	return;		
}

void SPI_Handler(void)
{
	if(SPI->SPI_SR & SPI_SR_ENDRX &SPI_SR_ENDTX){
		comms_rxend_handler();
	}
}

void ade_irq_handler(void)
{
	//raise flag indicating irq read pending
	irq_flag = 0xff;
}
void ade_zx_handler(void)
{
	//raise flag indicating zx read pending
	zx_flag = 0xff;
}

void service_ade(void)
{
	wbuff *tx_wb = 0; //wbuff to transmit
	if(SPI->SPI_SR & SPI_SR_ENDTX){
		//Don't do anything (last transfer hasn't finished yet)
	}else if(ade_flags.zx_next){ //check zx and irq flags first
		ade_flags.zx_next = 0;
		tx_wb = &ade_zx_buff;
		ade_flags.spiwrd = ZX_WRD;
	}else if(ade_flags.irq_next){
		ade_flags.irq_next = 0;
		tx_wb = &ade_irq_buff;
		ade_flags.spiwrd = IRQ_WRD;
	}else{
		//Finally, see if there is a pending com write
		tx_wb = dequeue(&ade_tx_queue);
		ade_flags.spiwrd = tx_wb? COM_WRD: NONE;
	}
	if(tx_wb){
		wbuff rxnext = alloc_wbuff((tx_wb->length) + 4);
		if(rxnext){
			ade_rx_buff = rxnext;
			spi_transfer_wbuff(tx_wb);
		}
	}
	return;
}