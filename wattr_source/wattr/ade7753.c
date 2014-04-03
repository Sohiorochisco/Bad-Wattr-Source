#include "headers/wattr_mem.h"
#include "headers/ade7753.h"
#include "sam.h"
#include "headers/wattr_pio.h"
#include "headers/pdc_periph.h"

#define SPI_TRANSFER_WBUFF(WBUFF_POINTER)\
	do{\
			PDC_SPI->PERIPH_TPR = PERIPH_TPR_TXPTR((uint32_t)WBUFF_POINTER->buff);\
			PDC_SPI->PERIPH_RPR = PERIPH_RPR_RXPTR((uint32_t)(rxnext->buff));\
			PDC_SPI->PERIPH_TCR = PERIPH_TCR_TXCTR(WBUFF_POINTER->length);\
			PDC_SPI->PERIPH_RCR = PERIPH_RCR_RXCTR(WBUFF_POINTER->length);\
	} while (0)
/*Static buffer for a single register write*/
static wbuff *ade_tx_buff;
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
	uint8_t zx_pending; //Indicates zx transfer in process
	uint8_t irq_pending; //Indicates irq transfer in process
	uint8_t com_pending; //Indicates coms transfer in process
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
	//Set all flags to zero (because you never know...)
	ade_flags.com_pending = 0;
	ade_flags.irq_next = 0;
	ade_flags.irq_pending = 0;
	ade_flags.zx_next = 0;
	ade_flags.zx_pending 0;
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
	wbuff *orx = ade_rx_buff;
	wbuff *otx = ade_tx_buff;
	wbuff *rxnext;
	if(zx_flag){
		zx_flag = 0;
		rxnext = alloc_wbuff(SML_BLOCK_WL);
		//Begin transmission
		if(rxnext){
			PDC_SPI->PERIPH_TPR = PERIPH_TPR_TXPTR((uint32_t)zx_wr);
			PDC_SPI->PERIPH_RPR = PERIPH_RPR_RXPTR((uint32_t)(rxnext->buff));
			PDC_SPI->PERIPH_TCR = PERIPH_TCR_TXCTR(SML_BLOCK_WL);
			PDC_SPI->PERIPH_RCR = PERIPH_RCR_RXCTR(SML_BLOCK_WL);
			ade_rx_buff = rxnext;
		}else{
			//throw out of memory assertion
			out_of_mem_assert();
			ade_rx_buff = 0;
		}
	}else if(irq_flag){
		irq_flag = 0;
		rxnext = alloc_wbuff(SML_BLOCK_WL);
		//Begin transmission
		if(rxnext){
			PDC_SPI->PERIPH_TPR = PERIPH_TPR_TXPTR((uint32_t)irq_wr);
			PDC_SPI->PERIPH_RPR = PERIPH_RPR_RXPTR((uint32_t)(rxnext->buff));
			PDC_SPI->PERIPH_TCR = PERIPH_TCR_TXCTR(SML_BLOCK_WL);
			PDC_SPI->PERIPH_RCR = PERIPH_RCR_RXCTR(SML_BLOCK_WL);
			ade_rx_buff = rxnext;
		}else{
			//throw out of memory assertion
			out_of_mem_assert();
			ade_rx_buff = 0;
		}	
	}else{
		rxnext = alloc_wbuff((txnext->length) + 4);
		if(rxnext){
			wbuff *txnext = dequeue(&ade_tx_queue);
			if(txnext){
				PDC_SPI->PERIPH_TPR = PERIPH_TPR_TXPTR((uint32_t)irq_wr);
				PDC_SPI->PERIPH_RPR = PERIPH_RPR_RXPTR((uint32_t)(rxnext->buff));
				PDC_SPI->PERIPH_TCR = PERIPH_TCR_TXCTR(SML_BLOCK_WL);
				PDC_SPI->PERIPH_RCR = PERIPH_RCR_RXCTR(SML_BLOCK_WL);
				ade_rx_buff = rxnext;
				ade_tx_buff = txnext;			
			}else{
				//if there is no queued word to transmit, set buffer to null
				ade_tx_buff = 0;
			}
		}else{
			//throw out of memory assertion
			out_of_mem_assert();
			ade_rx_buff = 0;
		}
	}
	if(!(orx->buff[0] | ADE_WRITE_MASK)){
		uint32_t i = 0;
		for(; i < ade_rx_buff->length;i+=4){
			//combine the register addresses with contents 
			orx->buff[i] |= (otx->buff[i]);
		}
		enqueue(&ade_rx_queue,ade_rx_buff);
	}else{
		free_wbuff(orx);
	}
	free_wbuff(otx);
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

int service_ade(void)
{
	if(SPI.SPI_SR & SPI_SR_ENDTX){
		//Don't do anything (last transfer hasn't finished yet)
	}else
	//check zx and irq flags first
	if(ade_flags.zx_next){
		ade_flags.zx_next = 0;
		rxnext = alloc_wbuff(SML_BLOCK_WL);
		//Begin transmission
		if(rxnext){
			ade_flags.zx_pending = 0xff;
			PDC_SPI->PERIPH_TPR = PERIPH_TPR_TXPTR((uint32_t)zx_wr);
			PDC_SPI->PERIPH_RPR = PERIPH_RPR_RXPTR((uint32_t)(rxnext->buff));
			PDC_SPI->PERIPH_TCR = PERIPH_TCR_TXCTR(SML_BLOCK_WL);
			PDC_SPI->PERIPH_RCR = PERIPH_RCR_RXCTR(SML_BLOCK_WL);
		}else{
			//throw out of memory assertion
			out_of_mem_assert();
			ade_rx_buff = 0;
		}
	}else if(ade_flags.irq_next){
		ade_flags.irq_next = 0;
		rxnext = alloc_wbuff(SML_BLOCK_WL);
		//Begin transmission
		if(rxnext){
			ade_flags.irq_pending;
			PDC_SPI->PERIPH_TPR = PERIPH_TPR_TXPTR((uint32_t)irq_wr);
			PDC_SPI->PERIPH_RPR = PERIPH_RPR_RXPTR((uint32_t)(rxnext->buff));
			PDC_SPI->PERIPH_TCR = PERIPH_TCR_TXCTR(SML_BLOCK_WL);
			PDC_SPI->PERIPH_RCR = PERIPH_RCR_RXCTR(SML_BLOCK_WL);
			ade_rx_buff = rxnext;
		}else{
				//throw out of memory assertion
				out_of_mem_assert();
				ade_rx_buff = 0;
		}
	}else{
		rxnext = alloc_wbuff((txnext->length) + 4);
		if(rxnext){
			wbuff *txnext = dequeue(&ade_tx_queue);
			if(txnext){
				PDC_SPI->PERIPH_TPR = PERIPH_TPR_TXPTR((uint32_t)irq_wr);
				PDC_SPI->PERIPH_RPR = PERIPH_RPR_RXPTR((uint32_t)(rxnext->buff));
				PDC_SPI->PERIPH_TCR = PERIPH_TCR_TXCTR(SML_BLOCK_WL);
				PDC_SPI->PERIPH_RCR = PERIPH_RCR_RXCTR(SML_BLOCK_WL);
				ade_rx_buff = rxnext;
				ade_tx_buff = txnext;
				}else{
				//if there is no queued word to transmit, set buffer to null
				ade_tx_buff = 0;
			}
			}else{
			//throw out of memory assertion
			out_of_mem_assert();
			ade_rx_buff = 0;
		}
	}
}