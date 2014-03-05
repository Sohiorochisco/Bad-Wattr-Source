/*Part of the Wattr project, written by Mitchell Haynes
 *
 * Includes macros definitions and forward declarations relating to the memory
 * pools used for deterministic dynamic memory allocation.
 */


#ifndef WATTR_MEM_ALLOC
#define WATTR_MEM_ALLOC
//Defines the dimensions of the pool of large memory blocks
#define BIG_BLOCK_WL	256
#define BIG_BLOCK_NUM	2
//Define the dimensions of the pool of medium memory blocks
#define MED_BLOCK_WL	32
#define MED_BLOCK_NUM	32
//Define the dimensions of the pool of small memory blocks
#define SML_BLOCK_WL	4
#define SML_BLOCK_NUM	32


void pools_init();

/*Used to allocate a single block of memory from one of the memory pools. The
 * only valid values for "size" are the values assigned to the macros above.
 * any other sizes will result in the function returning the null pointer.
 */

uint8_t * b_alloc(uint32_t size);

/*Used to free the memory block pointed to by p, assuming the array length
 *  passed as wl. Returns 1 in the case that the reference queue is full for 
 *  the pool,which indicates something horrible and bizzare, or 2 in the case
 *  that the memory does not belong to the pool to which it is being returned.
 */

uint32_t b_free(uint8_t *p, uint32_t size);

#endif
