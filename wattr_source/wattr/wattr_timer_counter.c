/*Functions to determine the length of a period
 * wattr_timer_counter.c
 *
 * Created: 4/23/2014 12:39:06 PM
 *  Author: mhaynes
 */ 

#include "sam.h"
#include "headers/pdc_periph.h"
#include "headers/mem/"

#define TIMER_COUNTER_BUFF_DPTH 3

static queue periods_queue;
static wbuff *pending_period;

static void *tc_buff[TIMER_COUNTER_BUFF_DPTH];

wbuff * read_period(void)
{
	return dequeue(&periods_queue);
}

//Called from ZX handler in order to take a period measurement
void reset_period(void)
{
	uint32_t period = TC0->TC_CHANNEL(0)->
}
void config_timer_counter(void)
{
	//Enable timer counter main clock
	PMC->PMC_PCER0 = PMC_PCER0_PID21;
	//Set block mode first
	TC0->TC_BMR = 0
	//Disable waveform mode, set mck/2 (60Mhz)as the counter input
	TC0->TC_CHANNEL(0)->TC_CMR = 0;
	//Enable the clock
	TC0->TC_CHANNEL(0)->TC_CCR = TC_CCR_CLKEN_1;
}


void make_tc_driver(pdc_periph tc)
{
	init_queue(&periods_queue,tc_buff,TIMER_COUNTER_BUFF_DPTH);
	pending_period = alloc_wbuff(SML_BLOCK_WL);
	tc->read = &read_period;
}
