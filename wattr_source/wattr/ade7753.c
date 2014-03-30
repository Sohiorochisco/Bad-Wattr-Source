#include "headers/wattr_mem.h"
#include "headers/ade7753.h"

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


void make_ade7753_driver(pdc_periph *ade)
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
	
}

uint32_t ade_write(wbuff *wrd)
{
	PDC_SPI->PERIPH_RCR = 0x4;
	PDC_SPI->PERIPH_TCR = wrd->length;
	PDC_SPI->PERIPH_TPR = wrd->buff;
	SPI->SPI_CR |= SPI_CR_SPIEN;
	return 0;		
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

static void config_spi(void)
{
	/*Ensure that the spi channel is in master mode, and 
	 *choose chip select channel zero */
	SPI->SPI_MR |= SPI_MR_MSTR | SPI_MR_PCS(0x7);
	/* Set clock phase and polarity for the ADE7753 chip select*/
	SPI->SPI_CSR[0] &= ~SPI_CSR_CPOL & ~SPI_CSR_NCPHA;
	SPI->SPI_CSR[0] |= SPI_CSR_CSAAT;
	return;
}

void SPI_Handler()
{
	
}
