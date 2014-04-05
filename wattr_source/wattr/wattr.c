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

void wattr_strcpy(uint8_t *buff, char *str)
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
	char test_vect[] = "TESTING";
    while (1) 
    {
		//blinking power indicator
		if(idx == 0x00080000){
			idx = 0;
			blink = ~blink;
			PIOD->PIO_ODSR = blink & PIO_ODSR_P20 & ~(PIO_ODSR_P21);
		}
		//Test the UART tx code
		wbuff *test_word = alloc_wbuff(SML_BLOCK_WL);
		if(test_word){
			wattr_strcpy(test_word->buff,test_vect);
			uint32_t st = wattr_uart.write(test_word);
			if(st){
				free_wbuff(test_word);
				test_word = 0;
			}
		}
		service_ade();
		service_uart();
		//Service watchdog timer
		WDT->WDT_CR = WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT;
		++idx;
    }
}

