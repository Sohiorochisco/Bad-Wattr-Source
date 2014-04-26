/*Wattr firmware file
 *
 * Defines the typedef struct pcd_periph, which represents a peripheral 
 * that writes to and reads from buffers which are transfered by the 
 * peripheral memory access controller for the particular peripheral.
 * The struct contains no pointers to memory - rather, all memory 
 * structures are either declared as static globals within the file for the 
 * particular peripheral, or are allocated from the set memory pool. 
 */


#ifndef PDC_PERIPH
#define PDC_PERIPH
#include "wattr_mem.h"
typedef struct{
	wbuff *(*read)(void);
	uint32_t (*write)(wbuff *w);
}pdc_periph;

/*constructor for spi interface, for the screen and the ade7753*/
void make_spi_driver(pdc_periph *ade_configure, pdc_periph *ade_zxread,
pdc_periph * ade_irqread,pdc_periph *screen);

/*constructor for rs232 uart pdc interface */
void make_rs232_driver(pdc_periph *rs232);

/*constructor for the TWI (I2C) interface */
void make_twi_drivers(pdc_periph *fanctrl_driver);

/* constructor for timer counter read-only interface */
void make_tc_driver(pdc_periph *tc);

/*These functions are called from the system timer interrupt
 *handler, in order to read from the peripheral write queues */
void service_ade(void);

void service_uart(void);

void service_twi(void);

//Reads period and performs linear regression to determine frequency
void sample_period(void);

#endif
