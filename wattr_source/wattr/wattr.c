/*
 * wattr.c
 *
 * Created: 2/24/2014 11:36:35 PM
 *  Author: mhaynes
 */ 

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
	pio_config();
	make_rs232_driver(&wattr_uart);
	wbuff *test_word;
    while (1) 
    {
		test_word = wattr_uart.read();
		if(!test_word){
			test_word = alloc_wbuff(SML_BLOCK_WL);
			test_word->buff = "TESTING";
		}
		uint32_t failw = wattr_uart.write(test_word);
		if(failw){
			free_wbuff(test_word);
		}
		//Service watchdog timer
		WDT->WDT_CR = WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT;
    }
}
