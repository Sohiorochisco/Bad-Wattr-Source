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
#include "headers/wattr_sys.h"
#include "headers/ade7753.h"

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
	uint32_t idx = 0;
	uint32_t blink = ONES;
	wbuff *test_word = 0;
	wbuff *comm_word = 0;
	uint32_t st = 0;
	wbuff *read_spi_stuff = 0;
		wbuff *mode_conf = alloc_wbuff(TNY_BLOCK_WL);
		wbuff *lcyc_conf = alloc_wbuff(TNY_BLOCK_WL);
		wbuff *irq_en	= alloc_wbuff(TNY_BLOCK_WL);
		write_ade_reg(mode_conf,0x700C,ADE_REG_GAIN,ADE_REG_GAIN_WBITS);
		write_ade_reg(lcyc_conf,200,ADE_REG_LINECYC,ADE_REG_LINECYC_WBITS);
		write_ade_reg(irq_en,(0x1u << 3),ADE_REG_IRQEN,ADE_REG_IRQEN_WBITS);
		wattr_write_buff(WATTR_ADE_PID,mode_conf);
		wattr_write_buff(WATTR_ADE_PID,lcyc_conf);
		wattr_write_buff(WATTR_ADE_PID,irq_en);
		int i = 0;
    while (1) {
		if(!test_word){
			test_word = alloc_wbuff(TNY_BLOCK_WL);
		}
		if(test_word){
			test_word->buff[0] = ADE_REG_DIEREV;
			for(i=1;i < 4; ++i){
				test_word->buff[i] = 0;
			}
			st = wattr_write_buff(WATTR_ADE_PID,test_word);
			if(st){
				free_wbuff(test_word);
			}
			test_word = 0;
		}
		if(!comm_word){
			comm_word = wattr_read_buff(WATTR_ADE_PID);
		}
		if(comm_word){
			st = wattr_write_buff(WATTR_UART_PID,comm_word);
			if(st){
				free_wbuff(comm_word);
			}
			comm_word = 0;
		}
	}
}

