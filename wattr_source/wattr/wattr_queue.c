/*Part of the Wattr project, written by Mitchell Haynes
 *
 * implementation of the queue functions declared in wattr_queue.h
 */


#include <stdlib.h>
#include "headers/mem/wattr_queue.h"
#include "sam.h"

/*Initializes a queue, with a max buffer size of bSize
*/
void init_queue(queue *q,void **buff, uint32_t l)
{
	q->length = l;
	q->buf = buff;
	q->first = 0;
	q->last = 0;
	return;
}	


static uint32_t wrap(uint32_t i, uint32_t l);

/*Adds a uint8_t pointer to the queue. Only call from within an interrupt handler.
 *  "first" should be the index of the empty address preceding the buffer,
 *   and "last" should be the index of the empty address following the buffer
 */ 
uint32_t enqueue(queue *q,void *p)
{
	uint32_t st = 2;
	uint32_t t = wrap(q->first,q->length);
	if (t == q->last){
		/*queue overflow */
		st = 1;
	}else{
		q->buf[q->first] = p;
		q->first = t;
		st = 0;
	}
	return st;
}

void * dequeue(queue *q)
{ 
	void *p;
	if (q->last != q->first){
		p = q->buf[q->last];
		q->last = wrap(q->last,q->length);
	}else{
		/*Queue underflow */
		p = 0;
	}
	return p;
}

/*Shifts the specified index to the correct point in the buffer array
 */
inline uint32_t wrap(uint32_t i, uint32_t l)
{
	if(i <= 0){
		i = l - 1;
	}else{
		--i;
	}
	return i;
}

