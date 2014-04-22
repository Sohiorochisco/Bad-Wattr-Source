/*
 * wattr_twi.c
 *
 * Created: 4/16/2014 1:48:33 AM
 *  Author: Mitchell Haynes
 * Allows communication with peripherals which use I2C or some variant thereof
 */ 


#include "headers/wattr_sys.h"
#include "headers/wattr_mem.h"
#include "headers/pdc_periph.h"
#include "sam.h"

//Clock piecewise durations in microseconds
#define TWI_CLK_HIGH_T	8
#define TWI_CLK_LOW_T	80

#define FAN_CTRL_PRIORITY 0
#define FAN_CONTR_SLAVE_ADDR
#define TWI_RXQUEUE_DPTH 2
#define TWI_TXQUEUE_DPTH 2
#define COUNT_SLAVES(a) a +
#define LIST_SLAVES(a) a , 


//Use form XXX(address,tx queue,rx queue)
#define TWI_SLAVES(XXX,TERM)\
	XXX(FAN_CONTR_SLAVE_ADDR)\
	TERM
	
//static initialization of the memory structures needed for the twi driver	
static uint32_t twi_slave_count = TWI_SLAVES(COUNT_SLAVES,0);
static 
static wbuff *tx_word;
static wbuff *rx_word;
static queue rx_queues[twi_slave_count];
static queue tx_queues[twi_slave_count];
static uint8_t *tx_qbuf[TWI_RXQUEUE_DPTH * twi_slave_count];
static uint8_t *rx_qbuf[TWI_TXQUEUE_DPTH * twi_slave_count];

//Peripheral read method for the fan controller 
wbuff * fan_ctr_read(void)
{
	return dequeue(*(rx_queues[FAN_CTRL_PRIORITY]));
}

//Peripheral write method for the fan controller
uint32_t fan_ctr_write(wbuff * wb)
{
	return enqueue(*(tx_queues[FAN_CTRL_PRIORITY]));
}

void config_twi(void)
{	//enable peripheral clock for TWI0
	PMC->PMC_PCER0 = PMC_PCER0_PID17;
	//Set clock divider (slow for now, probably never necessary to increase)
	uint32_t t_high = (uint32_t)(60*TWI_CLK_HIGH_T - 2);
	uint32_t t_low	= (uint32_t)(60*TWI_CLK_LOW_T - 2);
	TWI0->TWI_CWGR = TWI_CWGR_CHDIV(t_high) | TWI_CWGR_CKDIV(1) | TWI_CWGR_CLDIV(t_low);
	//enable master mode; disable slave mode
	TWI0->TWI_CR = TWI_CR_MSEN | TWI_CR_SVDIS;
}


void inline twi_pdc_send(Pdc *twi_pdc, wbuff *nwb)
{
}

void make_twi_drivers(pdc_periph *fanctrl_driver)
{
	int i = 0;
	for(;i < twi_slave_count;++i){
		init_queue(rx_queues[i],&(rx_qbuf[i * TWI_RXQUEUE_DPTH]));
		init_queue(tx_queues[i],&(tx_qbuf[i * TWI_RXQUEUE_DPTH]));
	}
	config_twi();
	fanctrl_driver->read = &fan_ctr_read;
	fanctrl_driver->write = &fan_ctr_write;
}

void twi_rxbuff_handler(Pdc *twi_pdc)
{
	
}


//Complete the transfer
static void twi_twbufe_handler(Pdc *twi_pdc,Twi *tx_twi)
{
	twi_pdc->PERIPH_PTCR = PERIPH_PTCR_TXTDIS;
	while(!(tx_twi->TWI_SR & TWI_SR_TXRDY)){}
	tx_twi->TWI_CR = TWI_CR_STOP;
	tx_twi->TWI_THR = TWI_THR_TXDATA(tx_word->buff[tx_word->length - 1]);
}

void service_twi(void)
{
	if(!(TWI0->TWI_SR & TWI_SR_TXRDY & TWI_SR_TXBUFE)){
		return;
	}
	wbuff *tx_wr = 0;
	int i = 0;
	//attempt dequeuing peripheral tx buffers in priority order
	for(; (tx_wr = 0) || i < twi_slave_count;++i){
		tx_wr = dequeue(&(tx_queues[i]))
	}
	if(tx_wr){
		TWI0->TWI_MR
	}
	
}