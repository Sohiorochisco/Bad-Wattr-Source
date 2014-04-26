/*Includes functions and handlers for controlling the various relays and 
 * relay board elements
 * wattr_relay_control.c
 *
 * Created: 4/26/2014 3:53:42 PM
 *  Author: mhaynes
 */ 

#include "sam.h"
#include "headers/wattr_sys.h"
#include "headers/pdc_periph.h"
#include "headers/wattr_mem.h"
#include "headers/wattr_pio.h"

#define RELAY_LED_TOGGLE_MASK (0x3u<<23)
#define RELAY_LED_TOGGLE(a) RELAY_LED_TOGGLE_MASK & a
#define RELAY_TOGGLE_MASK PIO_PER_P0
#define RELAY_TOGGLE(a) a & RELAY_TOGGLE_MASK
#define CAP_RELAY_MASK 0xffu

//Handler for the load toggle button on the DCM front panel
void relay_toggle(void)
{
	static uint32_t r_st = ~(PIO_ODSR_P23);
	r_st = ~r_st;
	uint32_t out_r = PIOD->PIO_ODSR;
	out_r &= ~RELAY_LED_TOGGLE_MASK;
	out_r |= RELAY_LED_TOGGLE(r_st);
	PIOD->PIO_ODSR = out_r;
	PIOE->PIO_CODR = ~r_st | RELAY_TOGGLE_MASK;
	PIOE->PIO_SODR = r_st & RELAY_TOGGLE_MASK;
	return;
}
uint32_t write_crelay(wbuff * wb)
{
	PIOC->PIO_CODR = CAP_RELAY_MASK;
	PIOC->PIO_SODR = CAP_RELAY_MASK & wb->buff[0];
	return 0;
}
wbuff * read_crelay(void)
{
	wbuff * wb = alloc_wbuff(1);
	if(wb){
		wb->buff[0] = PIOC->PIO_ODSR & CAP_RELAY_MASK;
	}
	return wb;
}

void make_caprelay_driver(pdc_periph *cr_driver)
{
	PIOC->PIO_CODR = CAP_RELAY_MASK;
	cr_driver->write = &write_crelay;
	cr_driver->read = &read_crelay;
}