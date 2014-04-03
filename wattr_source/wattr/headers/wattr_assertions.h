/*
 * wattr_assertions.h
 *
 * Created: 4/3/2014 3:00:59 PM
 *  Author: mhaynes
 */ 

/*Assertion thrown when a pool memory allocation method fails,
 * most likely indicating a memory leak condition
 */
void out_of_mem_assert(void);