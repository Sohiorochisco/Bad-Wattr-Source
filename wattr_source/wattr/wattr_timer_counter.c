/*Functions to determine the length of a period
 * wattr_timer_counter.c
 *
 * Created: 4/23/2014 12:39:06 PM
 *  Author: mhaynes
 */ 

#include "sam.h"
#include "headers/pdc_periph.h"
#include "headers/wattr_mem.h"

#define TIMER_COUNTER_BUFF_DPTH 3
#define SUM_SQINT_TO_59 70210
#define MAX_SAMPLE 60

static queue periods_queue;
static wbuff *pending_period;
static float sum_of_sq = 0;
static uint32_t sample_count = 0;

static void *tc_buff[TIMER_COUNTER_BUFF_DPTH];

//samples period and performs linear regression to determine frequency
void sample_period(void)
{
	uint32_t sample = TC0->TC_CHANNEL[0].TC_CV;
	sum_of_sq += sample_count * sample;
	if(sample_count < MAX_SAMPLE){
		++sample_count;
	}else{
		TC0->TC_CHANNEL[0].TC_CV = 0;
		sample_count = 0;
		*(pending_period->buff) = (uint32_t)(sum_of_sq/SUM_SQINT_TO_59);
		uint32_t st = enqueue(&periods_queue,pending_period);
		if(st){
			free_wbuff(pending_period);
		}
		pending_period = alloc_wbuff(SML_BLOCK_WL);
	}
}

//Called from ZX handler in order to take a period measurement
wbuff * read_period(void)
{
	return dequeue(&periods_queue);
}

void config_timer_counter(void)
{
	//Enable timer counter main clock
	PMC->PMC_PCER0 = PMC_PCER0_PID21;
	//Set block mode first
	TC0->TC_BMR = 0;
	//Disable waveform mode, set mck/2 (60Mhz)as the counter input
	TC0->TC_CHANNEL[0].TC_CMR = 0;
	//Enable the clock
	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN;
}


void make_tc_driver(pdc_periph *tc)
{
	init_queue(&periods_queue,tc_buff,TIMER_COUNTER_BUFF_DPTH);
	pending_period = alloc_wbuff(SML_BLOCK_WL);
	tc->read = &read_period;
}
