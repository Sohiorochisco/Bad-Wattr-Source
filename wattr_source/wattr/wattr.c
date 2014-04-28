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
	uint32_t er = 0;
	uint32_t count = 0;
	uint32_t offset = 0;
	uint32_t blink = ONES;
	wbuff *test_word = 0; 
	wbuff *comm_word = 0;
	wbuff *dat_word = 0;
	wbuff *term_word = 0;
	wbuff *period_word = 0;
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
	wbuff *test_i2c = lp_alloc_wbuff(TNY_BLOCK_WL);
	test_i2c->buff[0] = FC_REG_CONFIG_BYTE;
	test_i2c->buff[1] = 0x1A;
	periph_write_buff(WATTR_FAN_CTRL_PID,test_i2c);
    while (1) {
		do{
			test_word = periph_read_buff(WATTR_UART_PID);
		}while(!test_word);
		comm_parse(test_word);
		free_wbuff(test_word);
		test_word = 0;
	}
}

