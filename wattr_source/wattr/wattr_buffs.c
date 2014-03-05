#include <stdlib.h>


typedef struct{
	char **buf;
	uint32_t length;
	uint32_t first;
	uint32_t last;
} s_Queue;

s_Queue itr_queue;

/*Initializes a queue, with a max buffer size of bSize
*/
s_Queue * init_queue(uint32_t bSize)
{
	s_Queue *b = malloc(sizeof(s_Queue));
	uint32_t a = bSize * sizeof(char*);
	b->length = bSize;
	b->buf = malloc(a);
	b->first = 0;
	b->last = 1;
	return b;
}	

/*Use this method to ensure that s_Queue memory is properly freed
 */ 
void  release_queue(s_Queue *q)
{
	free(q->buf);
	free(q);
	q = NULL;
	return;
}


void wrap(uint32_t i, uint32_t l);

/*Adds a char pointer to the interrupt queue. Only call from within an interrupt handler.
 *  "first" should be the index of the empty address preceding the buffer,
 *   and "last" should be the index of the empty address following the buffer
 */ 
char * enqueue()
{
	char *p;
	if (itr_q->first == itr_q->last){
		/*queue overflow */
		p = 0;
	}else{
		p = itr_q->first;
		wrap(q->first,q->length);
	return 0;
}

/*Should disable interrupts during this method */
char * dequeue()
{
	uint32_t t = itr_q->last; 
	s_Queue *p;
	cpu_irq_disable();
	wrap(t, itr_q->length); //Test if the queue is empty
	if (t != itr_q->first){
		itr_q->last = t;
		p = itr_q->buf[itr_q->last];
	}else{
		/*Queue underflow */
		p = 0;
	}
	cpu_irq_enable();
	return p;
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
