/*
 * Firmware File for Wattr
 * By Mitchell Haynes
 */ 
#ifndef WATTR_PERIPH
#define WATTR_PERIPH
#include "wattr_mem.h"

uint32_t init_ADE7753(void);

uint32_t init_serial(void);

void service_ade(void);

void service_uart(void);


#endif