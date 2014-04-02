/*
 * wattr_uart.c
 *
 * Created: 3/31/2014 7:34:20 PM
 *  Author: mhaynes
 */ 
#include "sam.h"
#include "headers/pdc_periph.h"
/////////UART SPECIFIC DEFINES///////////////////
#define WATTR_UART_BAUD 0x2860 //9600 baud
#define WATTR_UART_RX_DPTH 0x4; 




////////////////////////////////////////////////////


static queue wattr_uart_rx_queue;
static void *uart_rx_fifo[WATTR_UART_RX_DPTH];
static wbuff *tx_buff;
static wbuff *rx_buff;



#define UART_SET_BAUD(baud) \
UART_BRGR_CD((1/baud))

void uart_endrx_handler()
{
	
		
}
//Perform initial setup of the UART module
static void config_uart(void)
{
	//Enable the clock using the power management controller
	PMC->PMC_PCER0 = PMC_PCER0_PID7;
	//Unmask the UART0 interrupt in the NVIC
	NVIC_EnableIRQ(UART0_IRQn);
	UART0->UART_BRGR |= UART_SET_BAUD(WATTR_UART_BAUD);
	//Reset transmitter and receiver
	UART0->UART_CR |= UART_CR_RSTTX | UART_CR_RSTTX;
	//Set the UART mode to Normal, with no parity bit 
	UART0->UART_MR |= UART_MR_CHMODE_NORMAL | UART_MR_PAR_NO;
	//Enable interrupt for end of receive transfer
	UART0->UART_IER = UART_IER_ENDRX;
	init_queue(&wattr_uart_rx_queue,uart_rx_fifo,
		WATTR_UART_RX_DPTH);
}
void make_rs232_driver(pdc_periph *rs232)
{
	config_uart();
	rs232->read = &wattr_usart_read;
	rs232->write = &wattr_usart_write;
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
		if(tx_buff != NULL){
			status = free_wbuff(tx_buff);
		}
		if(!status){
			tx_buff = w;
			UART0->UART_TPR = tx_buff->buff;
			//Begins transmission
			UART0->UART_TCR = tx_buff->length;
		}
	}else{
		status = 2;// Code for "too soon"...
	}
	return status;
}

UART0_Handler()
{
	if(UART0->UART_SR & UART_SR_ENDRX & UART_SR_ENDTX){
		uart_endrx_handler();	
	}
}