/*Part of the Wattr project, written by Mitchell Haynes
 *
 * Defines the functions used in order to build and operate with a queue of 
 * uint32_t type pointers. Used primarily for buffers, but also used in order 
 * to track free memory blocks in wattr_alloc.c
 */


/*The queue struct defines a queue primarily in order to track and pass 
 * ownership of memory blocks.
 */

#ifndef WATTR_QUEUE
#define WATTR_QUEUE
#endif
#include "sam.h"
typedef struct{
	void **buf;
	uint32_t length;
	uint32_t first;
	uint32_t last;
}queue;

/*init_queue is used to initialize a static queue. The array of pointers is
 * statically allocated independently, then its handle should be passed to buff.
 */
void init_queue(queue *q, void **buff, uint32_t l);

/*Used to pass ownership of a memory block to the queue. Obviously, the memory
 * should no longer be accessed by the previous owner once this function is 
 * called. 
 */
uint32_t enqueue(queue *q,void *p);

/*dequeue is used to acquire ownership of a memory block from queue q. 
 * Since this may be called from lower-priority functions or handlers, and it
 * is not fully reentrant, interrupts are disabled for most of the execution.
 * Thus, this should not be called from within a handler.
 */
void * dequeue(queue *q);

#define WATTR_QUEUE
