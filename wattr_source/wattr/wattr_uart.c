/*
 * wattr_uart.c
 *
 * Created: 3/31/2014 7:34:20 PM
 *  Author: mhaynes
 */ 
#include "sam.h"
#include "headers/pdc_periph.h"
#include "headers/wattr_mem.h"
/////////UART SPECIFIC DEFINES///////////////////
#define WATTR_UART_BAUD 0x2860 //9600 baud
#define WATTR_UART_RX_DPTH 0x4




////////////////////////////////////////////////////


static queue wattr_uart_rx_queue;
static void *uart_rx_fifo[WATTR_UART_RX_DPTH];
static wbuff *tx_buff;
static wbuff *rx_buff1;
static wbuff *rx_buff2;



#define UART_SET_BAUD(baud) \
UART_BRGR_CD((1/baud))

void uart_endrx_handler()
{
	//Push the current block address onto the FIFO
	uint32_t e = enqueue(&wattr_uart_rx_queue,rx_buff1);
	if(!e){
		rx_buff1 = rx_buff2;
		rx_buff2 = alloc_wbuff(SML_BLOCK_WL);
	} 
	while(UART0->UART_RNCR){}//Just to ensure that buffers are transfered properly.
	UART0->UART_RNPR = UART_RNPR_RXNPTR((uint32_t)(rx_buff2->buff));
	UART0->UART_RNCR = UART_RNCR_RXNCTR(rx_buff2->length);
	return;
}
//Perform initial setup of the UART module
static void config_uart(void)
{
	//Enable the clock using the power management controller
	PMC->PMC_PCER0 = PMC_PCER0_PID7;
	//Unmask the UART0 interrupt in the NVIC
	NVIC_EnableIRQ(UART0_IRQn);
	//Find correct divider value to generate baud-rate.
	uint32_t baud_div = (uint32_t)((SystemCoreClock/WATTR_UART_BAUD)>>4);
	UART0->UART_BRGR |= UART_SET_BAUD(baud_div);
	//Reset transmitter and receiver
	UART0->UART_CR |= UART_CR_RSTTX | UART_CR_RSTTX;
	//Set the UART mode to Normal, with no parity bit 
	UART0->UART_MR |= UART_MR_CHMODE_NORMAL | UART_MR_PAR_NO;
	//Enable interrupt for end of receive transfer
	UART0->UART_IER = UART_IER_ENDRX;
	init_queue(&wattr_uart_rx_queue,uart_rx_fifo,
		WATTR_UART_RX_DPTH);
	rx_buff1 = alloc_wbuff(SML_BLOCK_WL);
	rx_buff2 = alloc_wbuff(SML_BLOCK_WL);
	return;
}

static wbuff * wattr_usart_read(void)
{
	//Temporarily disable the interrupt which acts on the 
	//FIFO buffer from the other end.
	UART0->UART_IDR = UART_IDR_ENDRX;
	wbuff * buff = dequeue(&wattr_uart_rx_queue);
	UART0->UART_IER = UART_IER_ENDRX;
	return buff;
}

static uint32_t wattr_usart_write(wbuff *w)
{
	uint32_t status = 1;//Generic error code if transmission is unsuccessful
	UART0->UART_CR |= UART_CR_TXEN;
	if(UART0->UART_SR & UART_SR_TXRDY){	
		if(tx_buff){
			status = free_wbuff(tx_buff);
		}
		if(!status){
			tx_buff = w;
			UART0->UART_TPR = UART_TPR_TXPTR((uint32_t)(tx_buff->buff));
			//Begins transmission
			UART0->UART_TCR = UART_TCR_TXCTR(tx_buff->length);
		}
	}else{
		status = 2;// Code for "too soon"...
	}
	return status;
}

void make_rs232_driver(pdc_periph *rs232)
{
	config_uart();
	rs232->read = &wattr_usart_read;
	rs232->write = &wattr_usart_write;
	return;
}

void UART0_Handler()
{
	if(UART0->UART_SR & UART_SR_ENDRX){
		uart_endrx_handler();	
	}
}