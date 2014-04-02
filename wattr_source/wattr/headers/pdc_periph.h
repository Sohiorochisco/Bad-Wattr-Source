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
#define PDE_PERIPH
#include "wattr_mem.h"
typedef struct{
	wbuff *(*read)(void);
	uint32_t (*write)(wbuff *w);
}pdc_periph;

/*constructor for ade7753 spi pdc interface*/
//void make_ade7753_driver(pdc_periph *ade);

/*constructor for rs232 uart pdc interface */
void make_rs232_driver(pdc_periph *rs232);

#endif
