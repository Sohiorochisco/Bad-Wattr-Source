/*Wattr firmware, pio configuration functions
 *macros are defined int the c file as they should not be 
 *shared with other files.
 *
 * Created: 3/21/2014 4:21:53 PM
 *  Author: mhaynes
 */ 
#include "headers/ade7753.h"



#define ONES 0xFFFFFFFF
//Define empty macros for unused entries
#define X0(a,b,c,d,e)
#define X0(a,b,c,d)
#define X0(a,b)
#define X0(a)
//List interrupt lines and trigger conditions
//In order of priority for each peripheral.
//Use format ENTRY*PIA letter*(line num,both edge,rising edge,falling edge,
//handler pointer)
#define WATTR_INTERRUPT_LINES(ENTRYA,ENTRYB,ENTRYC,ENTRYD)\
	ENTRYA(PIO_PER_P15,0,0,PIO_FELLSR_P15,&ade_irq_handler)\
	ENTRYC(PIO_PER_P7,PIO_AIMER_P7,0,0,&ade_zx_handler)\
	ENTRYD(PIO_PER_P27,0,PIO_REHLSR_P27,0,&fp_ubutton_handler)\
	ENTRYD(PIO_PER_P25,0,PIO_REHLSR_P25,0,&fp_dbutton_handler)\
	ENTRYD(PIO_PER_P26,0,PIO_REHLSR_P26,0,&fp_lbutton_handler)\
	ENTRYD(PIO_PER_P24,0,PIO_REHLSR_P24,0,&fp_rbutton_handler)\
	ENTRYD(PIO_PER_P25,0,PIO_REHLSR_P25,0,&back_button_handler)\
	0/*prevent trailing operators */


//Define as follows:
// X(pio id, 
#define WATTR_OUTPUT_LINES(ENTRYA,ENTRYB,ENTRYC,ENTRYD)\
	ENTRYD(PIO_PER_P20)\
	ENTRYD(PIO_PER_P21)\
	ENTRYD(PIO_PER_P22)\
	ENTRYD(PIO_PER_P23)\
	ENTRYD(PIO_PER_P24)\
	0
	


//Defines the lines under control of peripherals.
// Use format ENTRY*PIO block*(pin mask,periph bit 1, bit 2)
#define WATTR_PERIPH_LINES(ENTRYA,ENTRYB,ENTRYC,ENTRYD)\
	ENTRYA(PIO_PDR_P12,0,0)\//SPI MISO
	ENTRYA(PIO_PDR_P13,0,0)\//SPI MOSI
	ENTRYA(PIO_PDR_P11,0,0)\/*SPI NPCS0	*/
	ENTRYA(PIO_PDR_P14,0,0)\/*SPI SPCK		*/
0/*Avoid trailing operator */


#define WATTR_PMC_CLK_LINES


//Interrupt vector definitions
#define X1(a,b,c,d,e) a |

config_pio_irq()
{
	//Enable PIO control of the lines
	PIOA->PIO_PER |= WATTR_INTERRUPT_LINES(X1,X0,X0,X0);
	PIOB->PIO_PER |= WATTR_INTERRUPT_LINES(X0,X1,X0,X0);
	PIOC->PIO_PER |= WATTR_INTERRUPT_LINES(X0,X0,X1,X0);
	PIOD->PIO_PER |= WATTR_INTERRUPT_LINES(X0,X0,X0,X1);
	
	//Set interrupt lines as input only (disable output writes)
	PIOA->PIO_ODR |= WATTR_INTERRUPT_LINES(X1,X0,X0,X0);
	PIOB->PIO_ODR |= WATTR_INTERRUPT_LINES(X0,X1,X0,X0);
	PIOC->PIO_ODR |= WATTR_INTERRUPT_LINES(X0,X0,X1,X0);
	PIOD->PIO_ODR |= WATTR_INTERRUPT_LINES(X0,X0,X0,X1);
	
	//Enable the input interrupt for each line
	PIOA->PIO_IER |= WATTR_INTERRUPT_LINES(X1,X0,X0,X0);
	PIOB->PIO_IER |= WATTR_INTERRUPT_LINES(X0,X1,X0,X0);
	PIOC->PIO_IER |= WATTR_INTERRUPT_LINES(X0,X0,X1,X0);
	PIOD->PIO_IER |= WATTR_INTERRUPT_LINES(X0,X0,X0,X1);
	
	//Only Edge-detection interrupts (no level based)
	PIOA->PIO_ESR |= WATTR_INTERRUPT_LINES(X1,X0,X0,X0);
	PIOB->PIO_ESR |= WATTR_INTERRUPT_LINES(X0,X1,X0,X0);
	PIOC->PIO_ESR |= WATTR_INTERRUPT_LINES(X0,X0,X1,X0);
	PIOD->PIO_ESR |= WATTR_INTERRUPT_LINES(X0,X0,X0,X1);
	
#define X2(a,b,c,d,e) b |
	
	//Default to all irqs being single edge detection
	PIOA->PIO_AIMER = ONES;
	PIOB->PIO_AIMER = ONES;
	PIOC->PIO_AIMER = ONES;
	PIOD->PIO_AIMER = ONES;
	
	//Set the double - edge interrupts
	PIOA->PIO_AIMDR |= WATTR_INTERRUPT_LINES(X2,X0,X0,X0);
	PIOB->PIO_AIMDR |= WATTR_INTERRUPT_LINES(X0,X2,X0,X0);
	PIOC->PIO_AIMDR |= WATTR_INTERRUPT_LINES(X0,X0,X2,X0);
	PIOD->PIO_AIMDR |= WATTR_INTERRUPT_LINES(X0,X0,X0,X2);
	
#define X3(a,b,c,d,e) c |
	
	//Set the rising edge interrupts
	PIOA->PIO_REHLSR |= WATTR_INTERRUPT_LINES(X3,X0,X0,X0);
	PIOB->PIO_REHLSR |= WATTR_INTERRUPT_LINES(X0,X3,X0,X0);
	PIOC->PIO_REHLSR |= WATTR_INTERRUPT_LINES(X0,X0,X3,X0);
	PIOD->PIO_REHLSR |= WATTR_INTERRUPT_LINES(X0,X0,X0,X3);
	

#define X4(a,b,c,d,e) d |
	
	//Set falling edge interrupts
	PIOA->PIO_FELLSR |= WATTR_INTERRUPT_LINES(X4,X0,X0,X0);
	PIOB->PIO_FELLSR |= WATTR_INTERRUPT_LINES(X0,X4,X0,X0);
	PIOC->PIO_FELLSR |= WATTR_INTERRUPT_LINES(X0,X0,X4,X0);
	PIOD->PIO_FELLSR |= WATTR_INTERRUPT_LINES(X0,X0,X0,X4);
	

	return;
}




#define X5(a,b,c,d,e)  e, //use to assign function pointers

//The following is to define each irq handler vector with a different
//number of function pointers based on the contents of the
//interrupt lines macro. If you can think of a better solution,
//PLEASE replace this code with your solution.
static void (*pioa_handlers[]) = {WATTR_INTERRUPT_LINES(X5,X0,X0,X0)};
static void (*piob_handlers[]) = {WATTR_INTERRUPT_LINES(X0,X5,X0,X0)};
static void (*pioc_handlers[]) = {WATTR_INTERRUPT_LINES(X0,X0,X5,X0)};
static void (*pioc_handlers[]) = {WATTR_INTERRUPT_LINES(X0,X0,X0,X5)};
	
#define X6(a,b,c,d,e) a,

static uint32_t pioa_masks[] = {WATTR_INTERRUPT_LINES(X6,X0,X0,X0)};
static uint32_t pioa_masks[] = {WATTR_INTERRUPT_LINES(X0,X6,X0,X0)};
static uint32_t pioa_masks[] = {WATTR_INTERRUPT_LINES(X0,X0,X6,X0)};
static uint32_t pioa_masks[] = {WATTR_INTERRUPT_LINES(X0,X0,X0,X6)};

#define X7(a,b,c,d,e) 1 +
//determine the number of valid entries for each irq handler vector
static int pioa_icount = WATTR_INTERRUPT_LINES(X7,X0,X0,X0);
static int piob_icount = WATTR_INTERRUPT_LINES(X0,X7,X0,X0);
static int pioc_icount = WATTR_INTERRUPT_LINES(X0,X0,X7,X0);
static int piod_icount = WATTR_INTERRUPT_LINES(X0,X0,X0,X7);


void PIOA_Handler()
{
	uint32_t mask = PIOA->PIO_IMR & PIOA->PIO_ISR;
	int i = 0;
	for(;i < pioa_icount;++i){
		if(mask & pioa_masks[i]){
			(*pioa_handlers)[i]();
			mask &= ~pioa_masks[i];//Indicate irq handled
		}
		if(!mask){
			break;
		}
	}
	return;	
}

void PIOB_Handler()
{
		uint32_t mask = PIOB->PIO_IMR & PIOB->PIO_ISR;
		int i = 0;
		for(;i < piob_icount;++i){
			if(mask & piob_masks[i]){
				(*piob_handlers)[i]();
				mask &= ~piob_masks[i];//Indicate irq handled
			}
			if(!mask){
				break;
			}
		}
		return;
}

void PIOC_Handler()
{
		uint32_t mask = PIOC->PIO_IMR & PIOC->PIO_ISR;
		int i = 0;
		for(;i < pioc_icount;++i){
			if(mask & pioc_masks[i]){
				(*pioc_handlers)[i]();
				mask &= ~pioc_masks[i];//Indicate irq handled
			}
			if(!mask){
				break;
			}
		}
		return;	
}

void PIOD_Handler()
{
		uint32_t mask = PIOD->PIO_IMR & PIOD->PIO_ISR;
		int i = 0;
		for(;i < piob_icount;++i){
			if(mask & piod_masks[i]){
				(*piod_handlers)[i]();
				mask &= ~piod_masks[i];//Indicate irq handled
			}
			if(!mask){
				break;
			}
		}
		return;	
}

//Definitions for the Peripheral controlled lines
#define X1(a,b,c) a |
#define X2(a,b,c) b |
#define X3(a,b,c) c |

wattr_periph_config(void)
{
	//Disable PIO control of the peripheral lines
	PIOA->PIO_PDR |= WATTR_PERIPH_LINES(X1,X0,X0,X0);
	PIOB->PIO_PDR |= WATTR_PERIPH_LINES(X0,X1,X0,X0);
	PIOC->PIO_PDR |= WATTR_PERIPH_LINES(X0,X0,X1,X0);
	PIOD->PIO_PDR |= WATTR_PERIPH_LINES(X0,X0,X0,X1);
	
	//Enable Peripheral control of the lines
	PIOA->PIO_ABCDSR[0] = WATTR_PERIPH_LINES(X2,X0,X0,X0);
	PIOA->PIO_ABCDSR[1] = WATTR_PERIPH_LINES(X3,X0,X0,X0);
	PIOB->PIO_ABCDSR[0] = WATTR_PERIPH_LINES(X0,X2,X0,X0);
	PIOB->PIO_ABCDSR[1] = WATTR_PERIPH_LINES(X0,X3,X0,X0);
	PIOC->PIO_ABCDSR[0] = WATTR_PERIPH_LINES(X0,X0,X2,X0);
	PIOC->PIO_ABCDSR[1] = WATTR_PERIPH_LINES(X0,X0,X3,X0);
	PIOD->PIO_ABCDSR[0] = WATTR_PERIPH_LINES(X0,X0,X0,X2);
	PIOD->PIO_ABCDSR[1] = WATTR_PERIPH_LINES(X0,X0,X0,X3);
}
//Configure lines that are used as outputs.
#define X1(a) a |
wattr_output_config(void)
{
	//Enable PIO control of lines
	PIOA->PIO_PER = WATTR_OUTPUT_LINES(X1,X0,X0,X0);
	PIOB->PIO_PER = WATTR_OUTPUT_LINES(X1,X0,X0,X0);
	PIOC->PIO_PER = WATTR_OUTPUT_LINES(X1,X0,X0,X0);
	PIOD->PIO_PER = WATTR_OUTPUT_LINES(X1,X0,X0,X0);
	
	//Enable outputs on the lines
	PIOA->PIO_OER = WATTR_OUTPUT_LINES(X1,X0,X0,X0);
	PIOB->PIO_OER = WATTR_OUTPUT_LINES(X1,X0,X0,X0);
	PIOC->PIO_OER = WATTR_OUTPUT_LINES(X1,X0,X0,X0);
	PIOD->PIO_OER = WATTR_OUTPUT_LINES(X1,X0,X0,X0);
	
}
