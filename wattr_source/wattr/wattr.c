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
	uint32_t er = 0;
	uint32_t count = 0;
	uint32_t offset = 0;
	uint32_t blink = ONES;
	wbuff *test_word = 0; 
	wbuff *comm_word = 0;
	wbuff *dat_word = 0;
	wbuff *term_word = 0;
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
		test_word = periph_read_buff(WATTR_UART_PID);
		if(test_word){
			switch(test_word->buff[0]){
				case 'v':
					count = CAL_COUNT;
					offset = 2;
					break;
				case 'i':
					count = CAL_COUNT;
					offset = 6;
					break;
				default:
					break;			
			}
			lp_free_wbuff(test_word);
			test_word = 0;	
		}
		while(!comm_word){
			comm_word = periph_read_buff(WATTR_ADE_PID);
			//for(indexer = 0; indexer <= 200; ++indexer);
		}
//		if(count == 10){
//			dat_word = lp_alloc_wbuff(SML_BLOCK_WL);
//			average_point = (uint32_t *)(dat_word->buff);
//			*average_point = 0;		
//		}
		if(count){
			dat_word = lp_alloc_wbuff(SML_BLOCK_WL);
			dat_word->buff[0] = 0;
			dat_word->buff[1] = comm_word->buff[offset];
			dat_word->buff[2] = comm_word->buff[offset + 1];
			dat_word->buff[3] = comm_word->buff[offset + 2];
			--count;
				do{
					er = periph_write_buff(WATTR_UART_PID,dat_word);
				}while(er);
				term_word = lp_alloc_wbuff(1);
				term_word->buff[0] = 0xA;
				do {
					er = periph_write_buff(WATTR_UART_PID,term_word);
				} while (er);
				term_word = 0;
		}
		
		lp_free_wbuff(comm_word);
		comm_word = 0;
	}
}

