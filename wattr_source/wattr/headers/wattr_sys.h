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

#define WATTR_ADE_PID 1
#define WATTR_UART_PID 0
//Used to initialize the background processes that service peripherals
 void wattr_sys_init(void);
 
 uint32_t wattr_write_buff(uint8_t periph_id,wbuff * wb);
 
 wbuff *wattr_read_buff(uint8_t periph_id);
#endif