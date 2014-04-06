/*
 * wattr_sys.c
 *
 * Created: 4/6/2014 3:00:26 AM
 *  Author: mhaynes
 */ 
#include "sam.h"
#include "headers/pdc_periph.h"
#include "headers/wattr_mem.h"
#define SYS_SERVICE_PERIOD 500

//Services that should execute every time that 
#define WATTR_SERVICES(XXX)\
	XXX(service_ade)\
	XXX(service_uart)\
	0
	
#define WATTR_PERIPHS(XXX)\
	XXX(wattr_uart)\
	XXX(wattr_ade)\
	0
	
	
#define LIST(a) a, 
#define COUNT(a) 1 +

static void (*wattr_srvs[])(void) = {WATTR_SERVICES(LIST)};
static uint8_t wattr_srvscount = WATTR_SERVICES(COUNT);
static pdc_periph *wattr_prphs[] = WATTR_PERIPHS(LIST);
static uint8_t wattr_prph_count;

wbuff *wattr_read_buff(uint8_t periph_id)
{
	if(periph_id < wattr_prph_count){
		SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
		wbuff *wb = wattr_prphs[periph_id]->read();
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		return wb;
	}else{
		return 0;
	}
}

uint32_t wattr_write_buff(uint8_t periph_id,wbuff * wb)
{
	if(periph_id < wattr_prph_count){
		SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
		uint32_t st = wattr_prphs[periph_id]->write(wb);
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		return st;
	}else{
		return 3;
	}
}

void config_systime(void)
{
	//set the reload value for the system counter
	SysTick->LOAD = SYS_SERVICE_PERIOD;
	//Clear the current value of the counter
	SysTick->VAL = 0;
	//enable system counter
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler(void)
{
	if(SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk){
		int i = 0;
		//perform all necessary periodic services
		for(; i < wattr_srvscount; ++i){
			wattr_srvs[i]();
		}
		WDT->WDT_CR = WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT;
	}
}