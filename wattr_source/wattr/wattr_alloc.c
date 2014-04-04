/*Part of the Wattr project, written by Mitchell Haynes
 *
 * Implements wattr_alloc.h
 */


#include "headers/wattr_mem.h"
#include "sam.h"

//static allocation for each of the memory pools
static char big_pool[BIG_BLOCK_WL * BIG_BLOCK_NUM];
static char med_pool[MED_BLOCK_WL * MED_BLOCK_NUM];
static char sml_pool[SML_BLOCK_WL * SML_BLOCK_NUM];

static queue big_mqueue;
static queue med_mqueue;
static queue sml_mqueue;

static void *big_q_buf[BIG_BLOCK_NUM +1];
static void *med_q_buf[MED_BLOCK_NUM +1];
static void *sml_q_buf[SML_BLOCK_NUM +1];

void pools_init()
{
	//initialize each memory queue, attach "buff" array
	init_queue(&big_mqueue, (void*)(&big_q_buf), BIG_BLOCK_NUM + 1);
	init_queue(&med_mqueue, (void*)(&med_q_buf), MED_BLOCK_NUM + 1);
	init_queue(&sml_mqueue, (void*)(&sml_q_buf), SML_BLOCK_NUM + 1);
	//break each up buffer into blocks, add to free memory pointer queues
	void *i;
	uint32_t k = 0;
	for(i=big_pool;k < BIG_BLOCK_NUM;i+=BIG_BLOCK_WL){
		enqueue(&big_mqueue,i);
		++k;
	}
	k = 0;
	for(i=med_pool;k < MED_BLOCK_NUM;i+=MED_BLOCK_WL){
		enqueue(&med_mqueue,i);
		++k;
	}
	k = 0;
	for(i=sml_pool;k < SML_BLOCK_NUM;i+=SML_BLOCK_WL){
		enqueue(&sml_mqueue,i);
		++k;
	}
	return;
}

void * b_alloc(uint32_t size)
{
	NVIC_DisableIRQ(UART0_IRQn);
	NVIC_DisableIRQ(SPI_IRQn);
	char *b = 0;
	switch(size){
	case BIG_BLOCK_WL:
		b = dequeue(&big_mqueue);
		break;
	case MED_BLOCK_WL:
		b = dequeue(&med_mqueue);
		break;
	case SML_BLOCK_WL:
		b = dequeue(&sml_mqueue);
		break;
	default:
		break;
	}
	//Will replace with macro soon
	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_EnableIRQ(SPI_IRQn);
	if(!b){
		//For debug use only, remove during normal use
		out_of_mem_assert();
		//throw out-of-memory assertion
	}
	return b;
}

uint32_t b_free(void *p, uint32_t size)
{
	NVIC_DisableIRQ(UART0_IRQn);
	NVIC_DisableIRQ(SPI_IRQn);
	uint32_t e = 1;
	switch(size){
	case BIG_BLOCK_WL:
		e = enqueue(&big_mqueue, p);
		break;
	case MED_BLOCK_WL:
		e = enqueue(&med_mqueue, p);
		break;
	case SML_BLOCK_WL:
		e = enqueue(&sml_mqueue, p);
		break;
	default:
		e = 1;
	}
	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_EnableIRQ(SPI_IRQn);
	return e;
}

wbuff *alloc_wbuff(uint32_t l)
{
	wbuff *b;
	/*Some checks necessary to ensure that the block size is not too small
	 *for the minimal wbuff
	 */
	if(l > 4){
		b = (wbuff*)b_alloc(l);
	}else{
		b = 0;
	}
	if(b){
		b->length = l - 4;
	}
	return b;
}

uint32_t free_wbuff(wbuff *buff)
{
	uint32_t l = 4 + buff->length;
	return b_free(buff,l);
}