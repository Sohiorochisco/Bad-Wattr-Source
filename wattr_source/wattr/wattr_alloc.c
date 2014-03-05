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

static char big_q_buf[BIG_BLOCK_NUM];
static char med_q_buf[MED_BLOCK_NUM];
static char sml_q_buf[SML_BLOCK_NUM];

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

char * b_alloc(uint32_t size)
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

uint32_t b_free(char *p, uint32_t size)
{
	uint32_t e;
	switch(size){
	case BIG_BLOCK_WL:
		e = enqueue(&big_mqueue, p);
		break;
	case MED_BLOCK_WL:
		e = enqueue(&med_mqueue, p);
		break;
	case SML_BLOCK_WL:
		e = enqueue(&sml_mqueue, p);
	default:
	}
	return e;
}
