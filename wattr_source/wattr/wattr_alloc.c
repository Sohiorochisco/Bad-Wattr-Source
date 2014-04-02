/*Part of the Wattr project, written by Mitchell Haynes
 *
 * Implements wattr_alloc.h
 */


#include "headers/mem/wattr_alloc.h"

//static allocation for each of the memory pools
static char big_pool[BIG_BLOCK_WL * BIG_BLOCK_NUM];
static char med_pool[MED_BLOCK_WL * MED_BLOCK_NUM];
static char sml_pool[SML_BLOCK_WL * SML_BLOCK_NUM];

static queue big_mqueue;
static queue med_mqueue;
static queue sml_mqueue;

static void *big_q_buf[BIG_BLOCK_NUM];
static void *med_q_buf[MED_BLOCK_NUM];
static void *sml_q_buf[SML_BLOCK_NUM];

void pools_init()
{
	//initialize each memory queue, attach "buff" array
	init_queue( big_mqueue, &big_q_buf, BIG_BLOCK_NUM);
	init_queue( med_mqueue, &med_q_buf, MED_BLOCK_NUM);
	init_queue( sml_mqueue, &sml_q_buf, SML_BLOCK_NUM);
	//break each up buffer into blocks, add to free memory pointer queues
	char *i;
	for(i=big_pool;i < BIG_BLOCK_NUM;i+=BIG_BLOCK_WL){
		enqueue(&big_mqueue,i);
	}
	for(i=med_pool;i < MED_BLOCK_NUM;i+=MED_BLOCK_WL){
		enqueue(&med_mqueue,i);
	}
	for(i=sml_pool;i < BIG_BLOCK_NUM;i+=SML_BlOCK_WL){
		enqueue(&sml_mqueue,i);
	}
	return 0;
}

void * b_alloc(uint32_t size)
{
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
	}
	return b;
}

uint32_t b_free(void *p, uint32_t size)
{
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
	return e;
}

inline wbuff *alloc_wbuff(uint32_t l)
{
	wbuff *b;
	/*Some checks necessary to ensure that the block size is not too small
	 *for the minimal wbuff
	 */
	if(l > 4){
		b = (wbuff*)b_alloc(l);
	}else{
		b = NULL;
	}
	if(b != NULL){
		b->length = l - 4;
	}
	return b;
}

inline uint32_t free_wbuff(wbuff *buff)
{
	l = 4 + buff->length
	return b_free(buff,l);
}