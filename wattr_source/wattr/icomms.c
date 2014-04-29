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
#include "headers/fan_controller.h"

#define RELAY_WRITE_HEADER 0xC7u
#define RRELAY_INIT_WRITE 0xFFC1FFC1u

void comm_parse(wbuff *wb)
{
	uint32_t st = 0;
	wbuff *nwb = 0;
	wbuff *tstwrd = 0;
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
	case 'c':
		nwb = alloc_wbuff(1);
		if(nwb){
			nwb->buff[0] = wb->buff[1];
			st = periph_write_buff(WATTR_CAPRELAY_PID,nwb);
		}
		break;
	case 'f':
		nwb = periph_read_buff(WATTR_PERIODMSR_PID);
		if(nwb){
			st = periph_write_buff(WATTR_UART_PID,nwb);
		}
		break;
	case 's':
		while(!tstwrd){
			tstwrd = periph_read_buff(WATTR_ADE_IRQ_PID);
		}
		nwb = alloc_wbuff(8);
		if(nwb){
			ade_24int_to_32int(tstwrd->buff + 5,nwb->buff);
			ade_24int_to_32int(tstwrd->buff + 9,nwb->buff + 4);
			st = periph_write_buff(WATTR_UART_PID,nwb);
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
		*(uint32_t*)(rdi->buff) = RRELAY_INIT_WRITE;
		er = periph_write_buff(WATTR_RLYDVR_PID,rdi);
		if(er){
			free_wbuff(rdi);
		}
	}
	wbuff *ade_linc = alloc_wbuff(4);
	if(ade_linc){
		ade_linc->buff[0] = ADE_REG_LINECYC | ADE_WRITE_MASK;
		ade_linc->buff[1] = 0;
		ade_linc->buff[2] = 20;
		ade_linc->buff[3] = 0;
		er = periph_write_buff(WATTR_ADE_CONFIG_PID,ade_linc);
		if(er){
			free_wbuff(ade_linc);
		}
	}
	wbuff *ade_mode = alloc_wbuff(4);
	if(ade_mode){
		ade_mode->buff[0] = ADE_REG_MODE | ADE_WRITE_MASK;
		ade_mode->buff[1] = 0;
		ade_mode->buff[2] = 0x80;
		ade_mode->buff[3] = 0;
		er = periph_write_buff(WATTR_ADE_CONFIG_PID,ade_mode);
		if(er){
			free_wbuff(ade_mode);
		}
	}
	wbuff *ade_irqen = alloc_wbuff(4);
	if(ade_irqen){
		ade_irqen->buff[0] = ADE_REG_IRQEN | ADE_WRITE_MASK;
		ade_irqen->buff[1] = 0;
		ade_irqen->buff[2] = 0x4;
		ade_irqen->buff[3] = 0;
		er = periph_write_buff(WATTR_ADE_CONFIG_PID,ade_irqen);
		if(er){
			free_wbuff(ade_irqen);
		}
	}
	/*Fan Controller initialization (over I2C) */
	
	//set pwm start duty cycle
	wbuff *fc_pwm_sdc = alloc_wbuff(2);
	if(fc_pwm_sdc){
		fc_pwm_sdc->buff[0] = FC_REG_PWM1_SDC;
		fc_pwm_sdc->buff[1] = FC_PWM1_START_DC;
		er = periph_write_buff(WATTR_FAN_CTRL_PID,fc_pwm_sdc);
		if(er){
			free_wbuff(fc_pwm_sdc);
		}
	}
	wbuff *fc_config_byte = alloc_wbuff(2);
	if(fc_config_byte){
		fc_config_byte->buff[0] = FC_REG_CONFIG_BYTE;
		fc_config_byte->buff[1] = FC_CONFIG_BYTE;
		er = periph_write_buff(WATTR_FAN_CTRL_PID,fc_config_byte);
		if(er){
			free_wbuff(fc_config_byte);
		}
	}
	wbuff *fc_start_temp = alloc_wbuff(2);
	if(fc_start_temp){
		fc_start_temp->buff[0] = FC_REG_CH1_FST;
		fc_start_temp->buff[1] = FC_PWM1_START_DC;
		er = periph_write_buff(WATTR_FAN_CTRL_PID,fc_start_temp);
		if(er){
			free_wbuff(fc_start_temp);
		}
	}
	wbuff *fc_ch1_config = alloc_wbuff(2);
	if(fc_pwm_sdc){
		fc_ch1_config->buff[0] = FC_REG_FANCONFIG;
		fc_ch1_config->buff[1] = FC_CONFIGURE;
		er = periph_write_buff(WATTR_FAN_CTRL_PID,fc_ch1_config);
		if(er){
			free_wbuff(fc_ch1_config);
		}
	}
	wbuff *fc_dcdt = alloc_wbuff(2);
	if(fc_pwm_sdc){
		fc_dcdt->buff[0] = FC_REG_DC_ROC;
		fc_dcdt->buff[1] = FC_DC_DT;
		er = periph_write_buff(WATTR_FAN_CTRL_PID,fc_dcdt);
		if(er){
			free_wbuff(fc_dcdt);
		}
	}
	wbuff *fc_dc_step = alloc_wbuff(2);
	if(fc_dc_step){
		fc_dc_step->buff[0] = FC_REG_DC_SS;
		fc_dc_step->buff[1] = FC_DC_STEP_SIZE;
		er = periph_write_buff(WATTR_FAN_CTRL_PID,fc_dc_step);
		if(er){
			free_wbuff(fc_dc_step);
		}
	}
	wbuff *fc_pwm_freq = alloc_wbuff(2);
	if(fc_pwm_freq){
		fc_pwm_freq->buff[0] = FC_REG_PWM_FS;
		fc_pwm_freq->buff[1] = FC_PWM_FREQ;
		er = periph_write_buff(WATTR_FAN_CTRL_PID,fc_pwm_freq);
		if(er){
			free_wbuff(fc_pwm_freq);
		}
	}
}