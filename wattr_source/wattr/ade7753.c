#include "headers/wattr_mem.h"
#include "headers/ade7753.h"
#include "sam.h"
#include "headers/wattr_pio.h"

/*Static buffer for a single register write*/
static volatile wbuff *ade_tx_buff;
/*Pointer to ADE7753 received data buffer */
static volatile wbuff *ade_rx_buff;
/*received buffer queue struct*/
static queue ade_rx_queue;
static void *rxq_buff[ADE_RXBUFF_DPTH];

/*Standard write for IRQ interrupt from ADE7753*/
uint8_t irq_wr[] = {
	ADE_REG_AENERGY,0x00,0x00,0x00,ADE_REG_RAENERGY,0x00,0x00,0x00
}
uint8_t zx_wr[] = {
	ADE_REG_VRMS,0x00,0x00,0x00,ADE_REG_IRMS,0x00,0x00,0x00
} 

inline void ade_write_reg(uint32_t w,uint32_t wl, uint8_t addr)
{
	uint32_t nw = w <<(24 % wl);
	ade_tx_buff->buff[0] = w | ADE_WRITE_MASK;
	ade_tx_buff->buff[1] = nw;
	ade_tx_buff->buff[2] = nw >> 8;
	ade_tx_buff->buff[3] = nw >> 16;
	ade_write(ade_tx_buff);
	return 0;
}

static void config_spi(void)
{
	/*Ensure that the spi channel is in master mode, and 
	 *choose chip select channel zero */
	SPI->SPI_MR |= SPI_MR_MSTR | SPI_MR_PCS(0x7);
	/* Set clock phase and polarity for the ADE7753 chip select*/
	SPI->SPI_CSR[0] &= ~(SPI_CSR_CPOL) & ~(SPI_CSR_NCPHA);
	SPI->SPI_CSR[0] |= SPI_CSR_CSAAT;
	SPI->SPI_CR |= SPI_CR_
	return;
}


void make_ade7753_driver(pdc_periph *ade_driver)
{
	config_spi();
	ade->periph_config = ade_config;
	ade->read = ade_read
	ade->write = ade_write;
	ade->write_std = ade_write_std;
	return;
}

uint32_t ade_read()
{
	//Disable the enqueueing interrupt
	SPI.SPI_IDR = SPI_IDR_ENDRX;
	//Re-enable the rx interupt, since 
	SPI.SPI_IER = SPI_IER_ENDRX;	
}

uint32_t ade_write(wbuff *wrd)
{
	uint32_t st = 1;
	if(SPI->SPI_SR & SPI_SR_ENDTX){
		if(ade_rx_buff){
			st = free_wbuff(ade_rx_buff);
		}
		ade_rx_buff = alloc_wbuff((wrd->length) + 4);
		if(ade_tx_buff){
			st = free_wbuff(ade_tx_buff);
		}
		ade_tx_buff = wrd;
		if(ade_tx_buff || ade_rx_buff){
			PDC_SPI->PERIPH_RNCR = PERIPH_RNCR_RXNCTR(ade_rx_buff->length);
			SPI_SPI->PERIPH_TNCR = PERIPH_TNCR_TXNCTR(ade_tx_buff->length);
			PDC_SPI->PERIPH_TNPR = PERIPH_TNPR_TXNPTR(ade_tx_buff->buff);
			PDC_SPI->PERIPH_RNPR = PERIPH_RNPR_RXNPTR(ade_rx_buff->buff);
			SPI->SPI_CR |= SPI_CR_SPIEN;
		}
	}
	return st;		
}

uint32_t ade_config(void)
{
	init_queue(&ade_rx_queue,rxq_buff,ADE_RXBUFF_DPTH);
	config_spi();
}

uint32_t ade_write_std(uint32_t rw)
{
	switch(rw){
	case 0:
		break;
	case 1:
		break;
	case 2:	
		break;
	default:
		break;
	}
	return 0;
}

static void comms_rxend_handler(void)
{
	if(!(ade_tx_buff->buff[0] | ADE_WRITE_MASK)){
		uint32_t i = 0;
		for(; i < ade_rx_buff->length;++i){
			//combine the register addresses with contents 
			ade_rx_buff->buff[i] |= ade_tx_buff->buff[i];
		}
		enqueue(&ade_rx_queue,ade_rx_buff);
	}
	return;
}

void SPI_Handler()
{
	if(SPI->SPI_SR & SPI->SPI_IMR & SPI_SR_ENDRX){
		comms_rxend_handler();
	}	
}
