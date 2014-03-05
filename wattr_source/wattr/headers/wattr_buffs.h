/*Include file for the limited-scope memory allocation objects used for Wattr,
 * all of which simply redistribute uniform length arrays from within 
 * larger static arrays
 */



#ifndef WATTR_BUFFS_
#define WATTR_BUFFS_

typedef struct{
}aqueue;

 
uint32_t * aq_next_free(aqueue aq);

void aq_free(aqueue *aq);

uint32_t aq_enqueue(aqueue *aq, uint32_t *i);

uint32_t *aq_dequeue(aqueue *aq);

void make_aqueue(aqueue *aq, uint32_t *sa, uint32_t d, uint32_t wl);

typedef struct{
}array_pool;

uint32_t * array_alloc(array_pool *ap);

void array_free(array_pool *ap);

void make_array_pool(array_pool *ap, uint32_t *sa, uint32_t d, uint32_t al);


#endif





