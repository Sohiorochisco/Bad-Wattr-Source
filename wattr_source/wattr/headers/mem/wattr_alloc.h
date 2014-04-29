/*Part of the Wattr project, written by Mitchell Haynes
 *
 * Includes macros definitions and forward declarations relating to the memory
 * pools used for deterministic dynamic memory allocation.
 */


#ifndef WATTR_MEM_ALLOC
#define WATTR_MEM_ALLOC
#include "sam.h"
//Defines the dimensions of the pool of large memory blocks
#define BIG_BLOCK_WL	256
#define BIG_BLOCK_NUM	2
//Define the dimensions of the pool of medium memory blocks
#define MED_BLOCK_WL	12
#define MED_BLOCK_NUM	64
//Define the dimensions of the pool of small memory blocks
#define SML_BLOCK_WL	4
#define SML_BLOCK_NUM	64
#define TNY_BLOCK_WL	8
#define TNY_BLOCK_NUM	128

#define SINGLE_BYTE_NUM 32
#define DOUBLE_BYTE_NUM 32

void pools_init();

/* Struct used as generic "buffer" object, for received data. */
typedef struct{
	uint32_t length;
	uint8_t *buff;
}wbuff;

/*Function for allocating wbuffs from the normal memory pools. l should include
 *the space for the length tag at the beginning of the struct.
 */	
wbuff *alloc_wbuff(uint32_t l);
//Function for properly freeing wbuffs
uint32_t free_wbuff(wbuff *buff);

/*Used to allocate a single block of memory from one of the memory pools. The
 * only valid values for "size" are the values assigned to the macros above.
 * any other sizes will result in the function returning the null pointer.
 */
void * b_alloc(uint32_t size);

/*Used to free the memory block pointed to by p, assuming the array length
 *  passed as wl. Returns 1 in the case that the reference queue is full for 
 *  the pool,which indicates something horrible and bizarre, or 2 in the case
 *  that the memory does not belong to the pool to which it is being returned.
 */
uint32_t b_free(void *p, uint32_t size);

//Used only in debugging to determine the difference between free anc alloc 
//call counts
uint32_t get_leak_count(void);

#endif
