/*
 * wattr.c
 *
 * Created: 2/24/2014 11:36:35 PM
 *  Author: mhaynes
 */ 

#define ONES 0xFFFFFFFF;
#define CAL_COUNT  1
#include "sam.h"
#include "headers/wattr_mem.h"
#include "headers/pdc_periph.h"
#include "headers/wattr_pio.h"
#include "headers/wattr_sys.h"
#include "headers/ade7753.h"
#include "headers/fan_controller.h"

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
	//Initialize driver background processes
	wattr_sys_init();
	PIOD->PIO_SODR = PIO_SODR_P24;
	PIOD->PIO_CODR = PIO_CODR_P23;
	wbuff *test_word = 0; 
	wbuff *comm_word = 0;
    while (1) {
		do{
			test_word = periph_read_buff(WATTR_UART_PID);
		}while(!test_word);
		comm_parse(test_word);
		free_wbuff(test_word);
		test_word = 0;
	}
}

