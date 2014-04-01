/*
 * wattr_uart.c
 *
 * Created: 3/31/2014 7:34:20 PM
 *  Author: mhaynes
 */ 
#include "headers/pdc_periph.h"
#define WATTR_UART_BAUD 0x

//Perform initial setup of the UART module
void config_uart(void)
{
	//Enable the clock using the power management controller
	PMC.PMC_PCER0 |= PMC_PCER0_PID7;
	//Unmask the UART0 interrupt in the NVIC
	NVIC_EnableIRQ(UART0_IRQn)
}
void make_rs232_driver(pdc_periph *rs232)
{
	config_uart();
	rs232->read = &wattr_usart_read;
	rs232->write = &wattr_usart_write;
	return;	
}


wbuff * wattr_usart_read(void)
{
	
}

void wattr_usart_write(wbuff *w)
{
	
}