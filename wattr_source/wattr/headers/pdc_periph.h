/*Wattr firmware file
 *
 * Defines the typedef struct pcd_periph, which represents a peripheral 
 * that writes to and reads from buffers which are transfered by the 
 * peripheral memory access controller for the particular peripheral.
 * The struct contains no pointers to memory - rather, all memory 
 * structures are either delared as static globals within the file for the 
 * particular peripheral, or are allocated from the set memory pool. 
 */


#ifndef PDC_PERIPH
#define PDE_PERIPH

typedef struct{
	uint32_t (*periph_config)(void);
	uint32_t *(*read)(void);
	uint32_t (*write)(uint32_t *w);
	uint32_t (*write_std)(uint32_t x);
}pdc_periph;

void make_ade7753(pdc_periph ade);

void make_ftdi(pdc_periph ftdi);

#endif
