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
	uint32_t  indexer = 1000;
	PIOA->PIO_ODSR &= ~(PIO_ODSR_P25);
	for(;indexer > 0; --indexer){}
	PIOA->PIO_ODSR |= PIO_ODSR_P25;
	uint32_t idx = 0;
	uint32_t blink = ONES;
	wbuff *test_word = 0; 
	wbuff *comm_word = 0;
	uint32_t st = 0;
	//wbuff *mode_conf1 = lp_alloc_wbuff(TNY_BLOCK_WL);
	//wbuff *mode_conf2 = lp_alloc_wbuff(TNY_BLOCK_WL);
	//wbuff *mode_conf3 = lp_alloc_wbuff(TNY_BLOCK_WL);
	//write_ade_reg(mode_conf1->buff,0x700C,ADE_REG_GAIN,ADE_REG_GAIN_WBITS);
	//write_ade_reg(mode_conf2->buff,200,ADE_REG_LINECYC,ADE_REG_LINECYC_WBITS);
	//write_ade_reg(mode_conf3->buff,(0x1u << 3),ADE_REG_IRQEN,ADE_REG_IRQEN_WBITS);
	//periph_write_buff(WATTR_ADE_PID,mode_conf1);
	//periph_write_buff(WATTR_ADE_PID,mode_conf2);
	//periph_write_buff(WATTR_ADE_PID,mode_conf3);
    while (1) {
//		if(!test_word){
//			test_word = lp_alloc_wbuff(TNY_BLOCK_WL);
//		}
//		if(test_word){
//			test_word->buff[0] = ADE_REG_DIEREV;
//			for(i=1;i < 4; ++i){
//				test_word->buff[i] = 0;
//			}
//			test_word->buff[4] = ADE_REG_DIEREV;
//			for(i=5; i< 8; ++i){
//				test_word->buff[i] = 0;
//			}
//			do{
//				st = periph_write_buff(WATTR_ADE_PID,test_word);
//			}while(st);
//		}
//		test_word = 0;
		while(!comm_word){
			comm_word = periph_read_buff(WATTR_ADE_PID);
		}
		do{
			st = periph_write_buff(WATTR_UART_PID,comm_word);
		}while(st);
		comm_word = 0;
	}
}

