/*Part of the Wattr project, written by Mitchell Haynes
 *
 * implementation of the queue functions declared in wattr_queue.h
 */


#include <stdlib.h>
#include "headers/mem/wattr_queue.h"

/*Initializes a queue, with a max buffer size of bSize
*/
void init_queue(queue q,void **buff, uint32_t l)
{
	b->length = l;
	b->buff = buff;
	b->first = 0;
	b->last = 1;
	return 0;
}	


static void wrap(uint32_t i, uint32_t l);

/*Adds a uint8_t pointer to the queue. Only call from within an interrupt handler.
 *  "first" should be the index of the empty address preceding the buffer,
 *   and "last" should be the index of the empty address following the buffer
 */ 
uint32_t enqueue(queue *q,void *p)
{
	uint32_t st;
	if (q->first == q->last){
		/*queue overflow */
		st = 1;
	}else{
		q->buff[q->first] = p;
		wrap(q->first,q->length);
	return 0;
}

void * dequeue(queue *q)
{
	uint32_t t = q->last; 
	char *p;
	wrap(t, q->length); //Test if the queue is empty
	if (t != q->first){
		q->last = t;
		p = q->buf[q->last];
	}else{
		/*Queue underflow */
		p = 0;
	}
	return (void*)p;
}

/*Shifts the specified index to the correct point in the buffer array
 */
inline void wrap(uint32_t i, uint32_t l)
{
	if (i <= 0){
		i = l - 1;
	}else{
		--i;
	}
	return;
}

/* Test main method 
 */

/*
uint32_t main()
{
	s_Queue *q = init_queue(5);
	float t[7];
	t[0] = 1.34;
	t[1] = 34.2;
	t[2] = 98.3;
	t[3] = 7.77;
	t[4] = 34.5;
	t[5] = 54;
	t[6] = 2.6;
	uint32_t i = 0;
	uint32_t j;
	for(; i < 7; ++i){
		j = enqueue(q, t + i);
		if (j == 1){
			printf("Queue Overflow\n");
		}
	}
	float *f;
	for (i = 0; i < 8 ; ++i){
		f = dequeue(q);
		if (f == 0){
			printf("Queue Underflow\n");
		}else{
			printf("%g\n",*f);
		}
	}
	return 0;
}
*/
