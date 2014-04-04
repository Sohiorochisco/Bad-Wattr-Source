/*
 * wattr_assertions.c
 *
 * Created: 4/3/2014 2:57:57 PM
 *  Author: mhaynes
 */ 

#include "sam.h"
//Called when one of the alloc methods fails,
//most likely indicating a memory leak
void out_of_mem_assert(void)
{
	//Set LED high to indicate that 
	PIOD->PIO_ODSR |= PIO_ODSR_P21;
}