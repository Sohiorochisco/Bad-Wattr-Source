/*
 * wattr_twi.c
 *
 * Created: 4/16/2014 1:48:33 AM
 *  Author: Mitchell Haynes
 * Allows communication with peripherals which use I2C or some variant thereof
 */ 

#include "headers/wattr_sys.h"
#include "headers/wattr_mem.h"
#include "sam.h"


void config_twi(void)
{
	//enable peripheral clock for TWI0
	PMC->PMC_PCER0 = PMC_PCER0_PID17;
	
}


void inline twi_pdc_send(Pdc *twi_pdc, wbuff *nwb)
{
	
}


void twi_rxbuff_handler(Pdc *twi_pdc)
{
}
void twi_twbufe_handler(Pdc *twi_pdc,Twi *tx_twi)
{
	twi_pdc->PERIPH_PTCR = PERIPH_PTCR_TXTDIS;
	while(!(tx_twi->TWI_SR & TWI_SR_TXRDY)){}
	tx_twi->TWI_CR = TWI_CR_STOP;
	tx_twi->TWI_THR = TWI_THR_TXDATA()
}