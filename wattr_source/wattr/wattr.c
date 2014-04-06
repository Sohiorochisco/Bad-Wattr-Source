/*
 * wattr.c
 *
 * Created: 2/24/2014 11:36:35 PM
 *  Author: mhaynes
 */ 

#define ONES 0xFFFFFFFF;
#include "sam.h"
#include "headers/wattr_mem.h"
#include "headers/pdc_periph.h"
#include "headers/wattr_pio.h"

pdc_periph wattr_uart;
pdc_periph wattr_spi;

static volatile uint32_t test_lock = 0;
void wattr_strcpy(uint8_t *buff, uint8_t *str)
{
	int i = 0;
	for(;str[i] != 0x00;++i){
		buff[i] = str[i];
	}
	buff[i++] = 0x00;
	return;
}

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	//Initialize memory management structures
	pools_init();
	//Configure PIO controllers
	pio_config();
	//Initialize UART driver structures, implement API
	make_rs232_driver(&wattr_uart);
	make_ade7753_driver(&wattr_spi);
	
	uint32_t idx = 0;
	uint32_t blink = ONES;
	wbuff *test_word = 0;
    while (1) 
    {
		blink = ~blink;
		PIOD->PIO_ODSR = blink & PIO_ODSR_P20 & ~(PIO_ODSR_P21);
		test_word = wattr_uart.read();
		if(test_word){
			wattr_uart.write(test_word);
		}
		service_ade();
		service_uart();
		//Service watchdog timer
		WDT->WDT_CR = WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT;
    }
}

