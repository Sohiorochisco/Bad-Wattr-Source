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
#include "headers/fan_controller.h"
#include "sam.h"

//Clock piecewise durations in microseconds
#define TWI_CLK_HIGH_T	8
#define TWI_CLK_LOW_T	80
#define FAN_CTRL_PRIORITY 0
#define TWI_RXQUEUE_DPTH 2
#define TWI_TXQUEUE_DPTH 20
#define COUNT_SLAVES(a) a +
#define LIST_SLAVES(a) a , 
#define TWI_SLAVE_COUNT 1


//Use form XXX(address,tx queue,rx queue)
#define TWI_SLAVES(XXX,TERM)\
	XXX(FC_HARDWARE_ADDR)\
	TERM
	
//static initialization of the memory structures needed for the twi driver	
static uint8_t twi_addresses[] = {TWI_SLAVES(LIST_SLAVES,0)};
static wbuff *tx_word;
static wbuff *rx_word;
queue rx_queues[TWI_SLAVE_COUNT];
queue tx_queues[TWI_SLAVE_COUNT];
void *tx_qbuf[TWI_RXQUEUE_DPTH * TWI_SLAVE_COUNT];
void *rx_qbuf[TWI_TXQUEUE_DPTH * TWI_SLAVE_COUNT];

//Peripheral read method for the fan controller 
wbuff * fan_ctr_read(void)
{
	return dequeue(&(rx_queues[FAN_CTRL_PRIORITY]));
}

//Peripheral write method for the fan controller
uint32_t fan_ctr_write(wbuff * wb)
{
	return enqueue(&(tx_queues[FAN_CTRL_PRIORITY]),wb);
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
	return;
}


void twi_send(Twi *this_twi, wbuff *nwb)
{
	this_twi->TWI_IADR = TWI_IADR_IADR(nwb->buff[0]);
	this_twi->TWI_CR = TWI_CR_STOP;
	this_twi->TWI_THR = nwb->buff[1];
}

void make_twi_drivers(pdc_periph *fanctrl_driver)
{
	int i = 0;
	for(;i < TWI_SLAVE_COUNT;++i){
		init_queue(&(rx_queues[i]),rx_qbuf[i * TWI_RXQUEUE_DPTH],TWI_RXQUEUE_DPTH);
		init_queue(&(tx_queues[i]),tx_qbuf[i * TWI_RXQUEUE_DPTH],TWI_TXQUEUE_DPTH);
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
	if(!(TWI0->TWI_SR & TWI_SR_TXRDY)){
		return;
	}
	wbuff *tx_wr = 0;
	int i = 0;
	//attempt dequeuing peripheral tx buffers in priority order
	for(; (tx_wr = 0) || i < TWI_SLAVE_COUNT;++i){
		tx_wr = dequeue(&(tx_queues[i]));
	}
	if(tx_wr){
		TWI0->TWI_MMR &= ~(TWI_MMR_DADR_Msk);
		TWI0->TWI_MMR = TWI_MMR_DADR(twi_addresses[i]);
		twi_send(TWI0,tx_wr);
		//Since the txbuff is only used by value,
		//It may be freed after the transfer is started
		free_wbuff(tx_wr);
	}
	/////For debug only////////////////////////////
	//currently nothing here...
	///////////////////////////////////////////////
}