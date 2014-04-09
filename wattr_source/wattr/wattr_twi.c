/*
 * wattr_twi.c
 *
 * Created: 4/7/2014 1:14:14 PM
 *  Author: mhaynes
 */ 

#include "headers/wattr_sys.h"
#include "headers/pdc_periph.h"

pdc_periph patsy;
pdc_periph gerald;

#define TWI_PERIPHS(XXX)\
	XXX(patsy,1,0x87)\
	XXX(gerald,2,0x79)\
	0
	
#define LIST_PERIPHS(a,b,c) a,
#define LIST_ADD(a,b,c) c ,
#define READ_DEC(a,b,c) static wbuff *wb a##_read_##b(void)
#define READ_DEC_TERM(a,b,c) READ_DEC(a,b,c);
#define WRITE_DEC(a,b,c) static uint32_t a##_write_##b(wbuff *wb)
#define WRITE_DEC_TERM(a,b,c) WRITE_DEC(a,b,c);

#define READ_DEF(a,b,c)\
	READ_DEC(a,b,c){\
		return twi_read(c);\
	}
	
#define WRITE_DEF(a,b,c)\
	WRITE_DEC(a,b,c){\
		return twi_write(c,wb);\
	}


void make_twi_driver(pdc_periph *pp)
{
	
}

static wbuff *twi_read(uint32_t p_addr)
{
	return 0;
}

static uint32_t twi_write(uint32_t p_addr,wbuff *wb)
{
	return 0;
}

TWI_PERIPHS(READ_DEC_TERM)

TWI_PERIPHS(WRITE_DEC_TERM)

TWI_PERIPHS(WRITE_DEF)

TWI_PERIPHS(READ_DEF)