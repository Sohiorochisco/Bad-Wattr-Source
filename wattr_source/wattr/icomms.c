/* Top level protocol based user interface and processing main file
 * icomms.c
 *
 * Created: 4/26/2014 7:13:45 PM
 *  Author: mhaynes
 */ 

#include "sam.h"
#include "headers/wattr_sys.h"
#include "headers/wattr_mem.h"
#include "headers/ade7753.h"

#define RELAY_WRITE_HEADER 0xC7u
void comm_parse(wbuff *wb)
{
	uint32_t st = 0;
	wbuff *nwb = 0;
	switch(wb->buff[0]){
	case 'v':
		
		break;
	case 'i':
		break;
	case 'r':
		nwb = alloc_wbuff(4);
		if(nwb){
			nwb->buff[0] = RELAY_WRITE_HEADER;
			nwb->buff[1] = wb->buff[1];
			nwb->buff[2] = RELAY_WRITE_HEADER;
			nwb->buff[3] = wb->buff[2];
			st = periph_write_buff(WATTR_RLYDVR_PID,nwb);
		}
		break;
	default:
		break;	
	}
	if(st){
		free_wbuff(nwb);
	}
}

//Function for initializing the initializer
void init_the_init(void)
{
	uint32_t er = 0;
	wbuff *rdi = alloc_wbuff(4);
	if(rdi){
		*(uint32_t*)(rdi->buff) = 0xC1FFC1FFu;
		er = periph_write_buff(WATTR_RLYDVR_PID,rdi);
	}
	if(er){
		free_wbuff(rdi);
	}
	wbuff *ade_linc = alloc_wbuff(4);
	if(ade_linc){
		ade_linc->buff[0] = ADE_REG_LINECYC | ADE_WRITE_MASK;
		ade_linc->buff[1] = 0;
		ade_linc->buff[2] = 200;
		ade_linc->buff[3] = 0;
		er = periph_write_buff(WATTR_ADE_CONFIG_PID,ade_linc);
	}
	if(er){
		free_wbuff(ade_linc);
	}
	wbuff *ade_mode = alloc_wbuff(4);
	if(ade_mode){
		*(uint32_t*)(ade_mode->buff) = ADE_REG_MODE | ADE_WRITE_MASK |
			ADE_MODE_CYCMODE;
		er = periph_write_buff(WATTR_ADE_CONFIG_PID,ade_mode);
	}
	if(er){
		free_wbuff(ade_mode);
	}
	wbuff *ade_irqen = alloc_wbuff(4);
	if(ade_irqen){
		*(uint32_t*)(ade_irqen->buff) = ADE_REG_IRQEN | ADE_WRITE_MASK |
			ADE_IRQ_CYCEND | ADE_IRQ_WSMP;
		er = periph_write_buff(WATTR_ADE_CONFIG_PID,ade_irqen);
	}
	if(er){
		free_wbuff(ade_irqen);
	}
}