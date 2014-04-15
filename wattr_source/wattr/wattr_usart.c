/*Part of the Wattr source code
 * wattr_usart.c
 *
 * Created: 4/13/2014 9:48:55 PM
 *  Author: mhaynes
 *
 *This file contains the driver implementation for the two USART
 * modules on the SAM4E. It also contains the read and write 
 * function implementations for each device which communicates
 * with the MCU using the MCU, through synchronous mode.
 */ 
#include "headers/pdc_periph.h"
#include "headers/wattr_mem.h"

//Not yet sure what order these should be set in.
//Adjusting may need to be a trial and error process,
//due to ambiguities in the data-sheet
void config_usart(void){
	//Disable the interrupt for USART1 while configuring
	NVIC_DisableIRQ(USART1_IRQn);
	//Set USART mode to SPI master, with 
	USART1->US_MR = US_MR_USART_MODE_SPI_MASTER |
	
}


void make_screen_driver(pdc_periph){
	
}

void write_screen(void){
	
}