/*Part of the Wattr project, written by Mitchell Haynes
 *
 * Implements wattr_alloc.h
 */

#include "headers/wattr_assertions.h"
#include "headers/wattr_mem.h"
#include "sam.h"

#define PERIPH_IRQS(FUNC)\
	do{\
	FUNC(SPI_IRQn);\
	FUNC(UART0_IRQn);\
	FUNC(TWI0_IRQn);\
	}while(0)
	
//static allocation for each of the memory pools
static char big_pool[BIG_BLOCK_WL * BIG_BLOCK_NUM];
static char med_pool[MED_BLOCK_WL * MED_BLOCK_NUM];
static char sml_pool[SML_BLOCK_WL * SML_BLOCK_NUM];
static char tny_pool[TNY_BLOCK_WL * TNY_BLOCK_NUM];
static char sb_pool[ SINGLE_BYTE_NUM];
static char db_pool[2 * DOUBLE_BYTE_NUM];

static queue big_mqueue;
static queue med_mqueue;
static queue sml_mqueue;
static queue tny_mqueue;
static queue sb_mqueue;
static queue db_mqueue;

static void *big_q_buf[BIG_BLOCK_NUM + 1];
static void *med_q_buf[MED_BLOCK_NUM + 1];
static void *sml_q_buf[SML_BLOCK_NUM + 1];
static void *tny_q_buf[TNY_BLOCK_NUM + 1];
static void *sb_q_buf[SINGLE_BYTE_NUM + 1];
static void *db_q_buf[DOUBLE_BYTE_NUM + 1];


volatile static uint32_t alloc_count_sml = 0;
volatile static uint32_t alloc_count_tny = 0;
volatile static uint32_t alloc_count_wbuff = 0;
volatile static uint32_t free_count_sml = 0;
volatile static uint32_t free_count_tny = 0;
volatile static uint32_t free_count_wbuff = 0;

void pools_init()
{
	//initialize each memory queue, attach "buff" array
	init_queue(&big_mqueue, (void*)(big_q_buf), BIG_BLOCK_NUM + 1);
	init_queue(&med_mqueue, (void*)(med_q_buf), MED_BLOCK_NUM + 1);
	init_queue(&sml_mqueue, (void*)(sml_q_buf), SML_BLOCK_NUM + 1);
	init_queue(&tny_mqueue, (void*)(tny_q_buf), TNY_BLOCK_NUM + 1);
	init_queue(&sb_mqueue,  (void*)(sb_q_buf),  SINGLE_BYTE_NUM + 1);
	init_queue(&db_mqueue,	(void*)(db_q_buf),	DOUBLE_BYTE_NUM + 1);
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
	k = 0;
	for(i=tny_pool;k < TNY_BLOCK_NUM;i+=TNY_BLOCK_WL){
		enqueue(&tny_mqueue,i);
		++k;
	}
	k = 0;
	for(i=sb_pool;k < SINGLE_BYTE_NUM; i+=1){
		enqueue(&sb_mqueue,i);
		++k;
	}
	k = 0;
	for(i=db_pool;k < DOUBLE_BYTE_NUM; i+=2){
		enqueue(&db_mqueue,i);
		++k;
	}
	return;
}

void * b_alloc(uint32_t size)
{
	PERIPH_IRQS(NVIC_DisableIRQ);
	char *b = 0;
	switch(size){
	case BIG_BLOCK_WL:
		b = dequeue(&big_mqueue);
		break;
	case MED_BLOCK_WL:
		b = dequeue(&med_mqueue);
		break;
	case SML_BLOCK_WL:
		++alloc_count_sml;
		b = dequeue(&sml_mqueue);
		break;
	case TNY_BLOCK_WL:
		++alloc_count_tny;
		b = dequeue(&tny_mqueue);
		break;
	case 2:
		b = dequeue(&db_mqueue);
		break;
	case 1:
		b = dequeue(&sb_mqueue);
		break;
	default:
		break;
	}
	PERIPH_IRQS(NVIC_EnableIRQ);
	if(b == 0){
		//For debug use only, remove during normal use
		out_of_mem_assert();
		//throw out-of-memory assertion
	}
	return b;
}

uint32_t b_free(void *p, uint32_t size)
{
	PERIPH_IRQS(NVIC_DisableIRQ);
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
	case TNY_BLOCK_WL:
		e = enqueue(&tny_mqueue, p);
		break;
	case 2:
		e = enqueue(&db_mqueue,p);
		break;
	case 1:
		e = enqueue(&sb_mqueue, p);
		break;
	default:
		e = 1;
	}
	PERIPH_IRQS(NVIC_EnableIRQ);
	return e;
}

wbuff *alloc_wbuff(uint32_t l)
{
	++alloc_count_wbuff;
	wbuff *b;
	PERIPH_IRQS(NVIC_DisableIRQ);
	b = (wbuff*)b_alloc(sizeof(wbuff));
	if(b){
		b->length = l;
		b->buff = b_alloc(l);
	}
	PERIPH_IRQS(NVIC_EnableIRQ);
	return b;
}

uint32_t free_wbuff(wbuff *oldbuff)
{
	PERIPH_IRQS(NVIC_DisableIRQ);
	uint32_t st = 4;
	if(oldbuff){
		uint32_t l = oldbuff->length;
		st = b_free(oldbuff->buff,l);
		st += b_free(oldbuff,sizeof(wbuff));
	}
	PERIPH_IRQS(NVIC_EnableIRQ);
	return st;
}


uint32_t get_leak_count(void)
{
	return alloc_count_tny + alloc_count_sml +alloc_count_wbuff;
}
