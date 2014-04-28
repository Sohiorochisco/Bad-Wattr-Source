/*
 * wattr_sys.c
 *
 * Created: 4/6/2014 3:00:26 AM
 *  Author: mhaynes
 */ 
#include "sam.h"
#include "headers/pdc_periph.h"
#include "headers/wattr_mem.h"
#include "headers/wattr_pio.h"
#include "headers/wattr_sys.h"

#define SYS_SERVICE_PERIOD 500

static pdc_periph wattr_uart;
static pdc_periph wattr_ade_zx;
static pdc_periph wattr_ade_irq;
static pdc_periph wattr_ade_config;
static pdc_periph wattr_rlydvr;
static pdc_periph wattr_fan_ctrl;
static pdc_periph wattr_periodmsr;
static pdc_periph wattr_caprelays;

//Services that should execute every time that 
#define WATTR_SERVICES(XXX)\
	XXX(service_ade)\
	XXX(service_uart)\
	XXX(service_twi)\
	0
	
#define WATTR_PERIPHS(XXX)\
	XXX(wattr_uart)\
	XXX(wattr_ade_config)\
	XXX(wattr_ade_irq)\
	XXX(wattr_ade_zx)\
	XXX(wattr_rlydvr)\
	XXX(wattr_fan_ctrl)\
	XXX(wattr_periodmsr)\
	XXX(wattr_caprelays)\
	0
	
#define POINTER_LIST(a) &a ,
#define LIST(a) a, 
#define COUNT(a) 1 +

static void (*wattr_srvs[])(void) = {WATTR_SERVICES(LIST)};
static uint8_t wattr_srvscount = WATTR_SERVICES(COUNT);
static pdc_periph *wattr_prphs[] = {WATTR_PERIPHS(POINTER_LIST)};
static uint8_t wattr_prph_count = WATTR_PERIPHS(COUNT);

wbuff *periph_read_buff(uint8_t periph_id)
{
	if(periph_id < wattr_prph_count){
		//SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
		wbuff *wb = wattr_prphs[periph_id]->read();
		//SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		return wb;
	}else{
		return 0;
	}
}

wbuff * lp_alloc_wbuff(uint32_t size){
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	wbuff *wb = alloc_wbuff(size);
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	return wb;
}

uint32_t lp_free_wbuff(wbuff *wb){
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	uint32_t st = free_wbuff(wb);
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	return st;
}

/*Method for writing to peripheral which will prevent a systick
 * from occurring during the write. The only problem with this 
 * method is that if it is polled, it may prevent the system
 * clock from advancing, thereby preventing writes and potentially
 * neglecting the watchdog timer, leading to system reset.
 * A different implementation may be called for...
 */
uint32_t periph_write_buff(uint8_t periph_id,wbuff * wb)
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
	NVIC_SetPriority(SysTick_IRQn,13);
	NVIC_EnableIRQ(SysTick_IRQn);
}

void wattr_sys_init(void)
{
		pools_init();
		//Configure PIO controllers
		pio_config();
		//Reset all of the peripherals
		uint32_t  indexer = 1000;
		PIOA->PIO_CODR = PIO_ODSR_P25;
		for(;indexer > 0; --indexer){}
		PIOA->PIO_SODR = PIO_ODSR_P25;
		//Initialize UART driver structures, implement API
		make_caprelay_driver(&wattr_caprelays);
		make_rs232_driver(&wattr_uart);
		make_spi_driver(&wattr_ade_config,&wattr_ade_zx,&wattr_ade_irq,&wattr_rlydvr);
		make_twi_drivers(&wattr_fan_ctrl);
		make_tc_driver(&wattr_periodmsr);
		config_systime();
		init_the_init();
		//turn on power light (show that the DCM isn't too dead)
		PIOD->PIO_ODSR |= PIO_ODSR_P20;
		return;	
}


void SysTick_Handler(void)
{
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		int i = 0;
		//perform all necessary periodic services
		for(; i < wattr_srvscount; ++i){
			wattr_srvs[i]();
		}
		WDT->WDT_CR = WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT;
	}
}