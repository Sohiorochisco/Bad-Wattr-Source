/*File for defining the dimensions of the static memory objects 
 *
 * Created by Mitchell, 2-24-14
 */

#ifndef WATTR_MEM
#define WATTR_MEM
#include "mem/wattr_queue.h"
#include "mem/wattr_alloc.h"

#define	ADE_RXBUFF_DPTH	5
#define ADE_TXBUFF_DPTH 3
#define	ADE_RXBUFF_WL	SML_BLOCK_WL

#define	SCREEN_WRBUFF_DPTH 5
#define	SCREEN_WRBUFF_WL	MED_BLOCK_WL

#define	TELEM_WRBUFF_DPTH	3
#define	TELEM_WRBUFF_WL		MED_BLOCK_WL

#define	WF_BUFF_DPTH	3
#define	WF_BUFF_WL		BIG_BLOCK_WL

#endif


