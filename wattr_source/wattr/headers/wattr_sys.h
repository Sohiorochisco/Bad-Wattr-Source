/*
 * wattr_sys.h
 *
 * Define system interface for my "kernel", lol
 * Created: 4/6/2014 7:37:11 PM
 *  Author: mhaynes
 */ 


#ifndef WATTR_SYS
#define WATTR_SYS
#include "pdc_periph.h"

#define WATTR_CAPRELAY_PID 7
#define WATTR_PERIODMSR_PID 6
#define WATTR_FAN_CTRL_PID 5
#define WATTR_RLYDVR_PID 4 
#define WATTR_ADE_IRQ_PID 2
#define WATTR_ADE_ZX_PID 3
#define WATTR_ADE_CONFIG_PID 1
#define WATTR_UART_PID 0
//Used to initialize the background processes that service peripherals
 void wattr_sys_init(void);
 
 //Enqueues a wbuff to be written over one of the device peripherals.
 uint32_t periph_write_buff(uint8_t periph_id,wbuff * wb);
 //Retrieve a wbuff from the received-queue of one of the peripherals
 wbuff *periph_read_buff(uint8_t periph_id);
 
 //alloc_wbuff function made safe to use from the main loop (see below)
wbuff * lp_alloc_wbuff(uint32_t size);
//Use this method to free memory from functions which run in the main loop.
// This ensures that free function is not interrupted by the system clock 
//handler.
uint32_t lp_free_wbuff(wbuff *wb);

//Initialize driver for the cap relays (writes directly to GPIO, w/o buffer)
void make_caprelay_driver(pdc_periph *cr_driver);

//Send initialization values to peripheral registers
void init_the_init(void);
#endif