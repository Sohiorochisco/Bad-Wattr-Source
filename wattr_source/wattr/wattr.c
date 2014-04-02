/*
 * wattr.c
 *
 * Created: 2/24/2014 11:36:35 PM
 *  Author: mhaynes
 */ 

#define ONES 0xFFFFFFFF;
#include "sam.h"
#include "headers/wattr_mem.h"
#include "headers/wattr_utils.h"
#include "headers/pdc_periph.h"
#include "headers/wattr_pio.h"

pdc_periph wattr_uart;
/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */

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
	wbuff *test_word = 0;
	uint32_t idx = 0;
	uint32_t blink = ONES;
    while (1) 
    {
		//blinking power indicator
		if(idx == 0x00080000){
			idx = 0;
			blink = ~blink;
			PIOD->PIO_ODSR = blink & PIO_ODSR_P20 & ~(PIO_ODSR_P21);
		}
		if(!test_word){
			test_word = alloc_wbuff(SML_BLOCK_WL);
			if(test_word){
				test_word->buff = "TESTING";
			}
		}
		if(test_word){
			uint32_t failw = wattr_uart.write(test_word);
			if(failw == 2){
				PIOD->PIO_ODSR |= PIO_ODSR_P21;
			}else if(!failw){
				test_word = 0;
			}
		}
		//Service watchdog timer
		WDT->WDT_CR = WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT;
		++idx;
    }
}
