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
#define WATTR_UART_BAUD 19200 //9600 baud
#define WATTR_UART_RX_DPTH 0x4
#define WATTR_UART_TX_DPTH 0x2
//Acknowledgement character
#define ASCII_BELL 0x07


////////////////////////////////////////////////////


static queue wattr_uart_rx_queue;
static queue wattr_uart_tx_queue;
static wbuff *uart_tx_buff = 0;
static wbuff *uart_rx_buff = 0;
static void *uart_rx_fifo[WATTR_UART_RX_DPTH];
static void *uart_tx_fifo[WATTR_UART_TX_DPTH];
static uint32_t ack_flag = 0;


void uart_endrx_handler(void)
{
	//Push the current block address onto the FIFO
	uint32_t e = enqueue(&wattr_uart_rx_queue,uart_rx_buff);
	//Get ready to acknowledge receipt
	ack_flag = 0xff;
	if(!e){
		uart_rx_buff = alloc_wbuff(4);
	} 
	if(uart_rx_buff){
		PDC_UART0->PERIPH_RPR = PERIPH_RPR_RXPTR((uint32_t)(uart_rx_buff->buff));
		PDC_UART0->PERIPH_RCR = PERIPH_RCR_RXCTR(uart_rx_buff->length);
	}
	return;
}
//Perform initial setup of the UART module
static void config_uart(void)
{
	//Initialize static memory structures
	init_queue(&wattr_uart_rx_queue,uart_rx_fifo,
		WATTR_UART_RX_DPTH);
	init_queue(&wattr_uart_tx_queue,uart_tx_fifo,
		WATTR_UART_TX_DPTH);
	//Enable the clock using the power management controller
	PMC->PMC_PCER0 = PMC_PCER0_PID7;
	//Reset and disable UART0
	UART0->UART_CR = UART_CR_RSTTX | UART_CR_RSTTX |
		UART_CR_TXDIS | UART_CR_RXDIS | UART_CR_RSTSTA; 
	//Find correct divider value to generate baud-rate.
	UART0->UART_BRGR = (uint32_t)(SystemCoreClock/(WATTR_UART_BAUD *16));
	//Set the UART mode to Normal, with no parity bit
	UART0->UART_MR = UART_MR_CHMODE_NORMAL | UART_MR_PAR_EVEN;
	//Enable interrupt for end of receive transfer
	UART0->UART_IER = UART_IER_ENDRX;
	//Enable transmitter and receiver
	UART0->UART_CR = UART_CR_TXEN | UART_CR_RXEN;
	//Enable PDC
	PDC_UART0->PERIPH_PTCR = PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN;
	//Unmask the UART0 interrupt in the NVIC
	NVIC_SetPriority(UART0_IRQn,8);
	NVIC_EnableIRQ(UART0_IRQn);
	return;
}

static wbuff * wattr_uart_read(void)
{
	//Temporarily disable the interrupt which acts on the 
	//FIFO buffer from the other end.
	UART0->UART_IDR = UART_IDR_ENDRX;
	wbuff * buff = dequeue(&wattr_uart_rx_queue);
	UART0->UART_IER = UART_IER_ENDRX;
	return buff;
}


static uint32_t wattr_uart_write(wbuff *w)
{
	//Since no interrupts use this queue, re-entrance unnecessary
	uint32_t st = enqueue(&wattr_uart_tx_queue,w);
	return st;
}

void make_rs232_driver(pdc_periph *rs232)
{
	uart_rx_buff = alloc_wbuff(4);
	config_uart();
	if(uart_rx_buff){
		PDC_UART0->PERIPH_RPR = PERIPH_RPR_RXPTR((uint32_t)(uart_rx_buff->buff));
		PDC_UART0->PERIPH_RCR = PERIPH_RCR_RXCTR(uart_rx_buff->length);
	}
	rs232->read = &wattr_uart_read;
	rs232->write = &wattr_uart_write;
	return;
}

void service_uart(void)
{
	//Only service if the TX PDC channel is inactive
	if(UART0->UART_SR & UART_SR_ENDTX){
		if(uart_tx_buff){
			free_wbuff(uart_tx_buff);
			uart_tx_buff = 0;
		}
		if(ack_flag){
			if(UART0->UART_SR & UART_SR_TXRDY){
				UART0->UART_THR = UART_THR_TXCHR(ASCII_BELL);
			}
			ack_flag = 0;
			return;
		}
		wbuff *tx_wb = dequeue(&wattr_uart_tx_queue); //wbuff to transmit
		if(tx_wb){
			uart_tx_buff = tx_wb;
			PDC_UART0->PERIPH_TPR = PERIPH_TPR_TXPTR((uint32_t)(tx_wb->buff));
			//Begins transmission
			PDC_UART0->PERIPH_TCR = PERIPH_TCR_TXCTR(tx_wb->length);
		}
	}
	return;
}
void UART0_Handler(void)
{

	if(UART0->UART_SR & UART_SR_RXBUFF){
		uart_endrx_handler();	
	}
}