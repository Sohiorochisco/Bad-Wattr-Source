/*Wattr firmware, pio configuration functions
 *macros are defined int the c file as they should not be 
 *shared with other files.
 *
 * Created: 3/21/2014 4:21:53 PM
 *  Author: mhaynes
 */ 
#include "sam.h"
#include "headers/wattr_pio.h"





#define ONES 0xFFFFFFFF
//Define empty macros for unused entries
#define X05(a,b,c,d,e)
#define X04(a,b,c,d)
#define X03(a,b,c)
#define X02(a,b)
#define X01(a)
//List interrupt lines and trigger conditions
//In order of priority for each peripheral.
//Use format ENTRY*PIA letter*(line num,both edge,rising edge,falling edge,
//handler pointer)
	//ENTRYA(PIO_PER_P15,0,0,PIO_FELLSR_P15,&ade_irq_handler)
	//ENTRYC(PIO_PER_P7,PIO_AIMER_P7,0,0,&ade_zx_handler)
	
#define WATTR_INTERRUPT_LINES(ENTRYA,ENTRYB,ENTRYC,ENTRYD)\
	ENTRYC(PIO_PER_P9,0,0,PIO_FELLSR_P15,&ade_irq_handler)\
	ENTRYC(PIO_PER_P7,PIO_AIMER_P7,0,0,&ade_zx_handler)\
	ENTRYD(PIO_PER_P27,0,PIO_REHLSR_P27,0,&fp_ubutton_handler)\
	ENTRYD(PIO_PER_P25,0,PIO_REHLSR_P25,0,&fp_dbutton_handler)\
	ENTRYD(PIO_PER_P26,0,PIO_REHLSR_P26,0,&fp_lbutton_handler)\
	ENTRYD(PIO_PER_P24,0,PIO_REHLSR_P24,0,&fp_rbutton_handler)\
	ENTRYD(PIO_PER_P25,0,PIO_REHLSR_P25,0,&back_button_handler)\
	0/*prevent trailing operators */


//Define as follows:
// X(pio id) 
#define WATTR_OUTPUT_LINES(ENTRYA,ENTRYB,ENTRYC,ENTRYD)\
	ENTRYD(PIO_PER_P20)\
	ENTRYD(PIO_PER_P21)\
	ENTRYD(PIO_PER_P22)\
	ENTRYD(PIO_PER_P23)\
	ENTRYD(PIO_PER_P24)\
	0
	


//Defines the lines under control of peripherals.
// Use format ENTRY*PIO block*(pin mask,periph bit 1, bit 2)
/*SPI MISO */
/*SPI MOSI */
/*SPI NPCS0	*/
/*SPI SPCK	*/
/*UART0 TX */
/*UART0 RX */
/*TWI 0		*/
/*TWI 1		*/
#define WATTR_PERIPH_LINES(ENTRYA,ENTRYB,ENTRYC,ENTRYD)\
	ENTRYA(PIO_PDR_P12,0,0)\
	ENTRYA(PIO_PDR_P13,0,0)\
	ENTRYA(PIO_PDR_P11,0,0)\
	ENTRYA(PIO_PDR_P14,0,0)\
	ENTRYA(PIO_PDR_P10,0,0)\
	ENTRYA(PIO_PDR_P9,0,0)\
	ENTRYA(PIO_PDR_P4,0,0)\
	ENTRYA(PIO_PDR_P3,0,0)\
0/*Avoid trailing operator */


#define WATTR_PMC_CLK_LINES


//Interrupt vector definitions
#define X1(a,b,c,d,e) a |

static void config_pio_irq(void)
{
	//Enable PIO control of the lines
	PIOA->PIO_PER |= WATTR_INTERRUPT_LINES(X1,X05,X05,X05);
	PIOB->PIO_PER |= WATTR_INTERRUPT_LINES(X05,X1,X05,X05);
	PIOC->PIO_PER |= WATTR_INTERRUPT_LINES(X05,X05,X1,X05);
	PIOD->PIO_PER |= WATTR_INTERRUPT_LINES(X05,X05,X05,X1);
	
	//Set interrupt lines as input only (disable output writes)
	PIOA->PIO_ODR |= WATTR_INTERRUPT_LINES(X1,X05,X05,X05);
	PIOB->PIO_ODR |= WATTR_INTERRUPT_LINES(X05,X1,X05,X05);
	PIOC->PIO_ODR |= WATTR_INTERRUPT_LINES(X05,X05,X1,X05);
	PIOD->PIO_ODR |= WATTR_INTERRUPT_LINES(X05,X05,X05,X1);
	
	//Enable the input interrupt for each line
	PIOA->PIO_IER |= WATTR_INTERRUPT_LINES(X1,X05,X05,X05);
	PIOB->PIO_IER |= WATTR_INTERRUPT_LINES(X05,X1,X05,X05);
	PIOC->PIO_IER |= WATTR_INTERRUPT_LINES(X05,X05,X1,X05);
	PIOD->PIO_IER |= WATTR_INTERRUPT_LINES(X05,X05,X05,X1);
	
	//Only Edge-detection interrupts (no level based)
	PIOA->PIO_ESR |= WATTR_INTERRUPT_LINES(X1,X05,X05,X05);
	PIOB->PIO_ESR |= WATTR_INTERRUPT_LINES(X05,X1,X05,X05);
	PIOC->PIO_ESR |= WATTR_INTERRUPT_LINES(X05,X05,X1,X05);
	PIOD->PIO_ESR |= WATTR_INTERRUPT_LINES(X05,X05,X05,X1);
	
#define X2(a,b,c,d,e) b |
	
	//Default to all irqs being single edge detection
	PIOA->PIO_AIMER = ONES;
	PIOB->PIO_AIMER = ONES;
	PIOC->PIO_AIMER = ONES;
	PIOD->PIO_AIMER = ONES;
	
	//Set the double - edge interrupts
	PIOA->PIO_AIMDR |= WATTR_INTERRUPT_LINES(X2,X05,X05,X05);
	PIOB->PIO_AIMDR |= WATTR_INTERRUPT_LINES(X05,X2,X05,X05);
	PIOC->PIO_AIMDR |= WATTR_INTERRUPT_LINES(X05,X05,X2,X05);
	PIOD->PIO_AIMDR |= WATTR_INTERRUPT_LINES(X05,X05,X05,X2);
	
#define X3(a,b,c,d,e) c |
	
	//Set the rising edge interrupts
	PIOA->PIO_REHLSR |= WATTR_INTERRUPT_LINES(X3,X05,X05,X05);
	PIOB->PIO_REHLSR |= WATTR_INTERRUPT_LINES(X05,X3,X05,X05);
	PIOC->PIO_REHLSR |= WATTR_INTERRUPT_LINES(X05,X05,X3,X05);
	PIOD->PIO_REHLSR |= WATTR_INTERRUPT_LINES(X05,X05,X05,X3);
	

#define X4(a,b,c,d,e) d |
	
	//Set falling edge interrupts
	PIOA->PIO_FELLSR |= WATTR_INTERRUPT_LINES(X4,X05,X05,X05);
	PIOB->PIO_FELLSR |= WATTR_INTERRUPT_LINES(X05,X4,X05,X05);
	PIOC->PIO_FELLSR |= WATTR_INTERRUPT_LINES(X05,X05,X4,X05);
	PIOD->PIO_FELLSR |= WATTR_INTERRUPT_LINES(X05,X05,X05,X4);
	

	return;
}




#define X5(a,b,c,d,e)  e, //use to assign function pointers

//The following is to define each irq handler vector with a different
//number of function pointers based on the contents of the
//interrupt lines macro. If you can think of a better solution,
//PLEASE replace this code with your solution.
static void (*pioa_handlers[])(void) = {WATTR_INTERRUPT_LINES(X5,X05,X05,X05)};
static void (*piob_handlers[])(void) = {WATTR_INTERRUPT_LINES(X05,X5,X05,X05)};
static void (*pioc_handlers[])(void) = {WATTR_INTERRUPT_LINES(X05,X05,X5,X05)};
static void (*piod_handlers[])(void) = {WATTR_INTERRUPT_LINES(X05,X05,X05,X5)};
	
#define X6(a,b,c,d,e) a,

static uint32_t pioa_masks[] = {WATTR_INTERRUPT_LINES(X6,X05,X05,X05)};
static uint32_t piob_masks[] = {WATTR_INTERRUPT_LINES(X05,X6,X05,X05)};
static uint32_t pioc_masks[] = {WATTR_INTERRUPT_LINES(X05,X05,X6,X05)};
static uint32_t piod_masks[] = {WATTR_INTERRUPT_LINES(X05,X05,X05,X6)};

#define X7(a,b,c,d,e) 1 +
//determine the number of valid entries for each irq handler vector
static int pioa_icount = WATTR_INTERRUPT_LINES(X7,X05,X05,X05);
static int piob_icount = WATTR_INTERRUPT_LINES(X05,X7,X05,X05);
static int pioc_icount = WATTR_INTERRUPT_LINES(X05,X05,X7,X05);
static int piod_icount = WATTR_INTERRUPT_LINES(X05,X05,X05,X7);


void PIOA_Handler()
{
	uint32_t mask = PIOA->PIO_IMR & PIOA->PIO_ISR;
	int i = 0;
	for(;i < pioa_icount;++i){
		if(mask & pioa_masks[i]){
			pioa_handlers[i]();
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
				piob_handlers[i]();
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
				pioc_handlers[i]();
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
			piod_handlers[i]();
			mask &= ~piod_masks[i];//Indicate irq handled
		}
		if(!mask){
			break;
		}
	}
	return;	
}

//Definitions for the Peripheral controlled lines
#define AOR(a,b,c) a |
#define BOR(a,b,c) b |
#define COR(a,b,c) c |

static void wattr_periph_config(void)
{
	//Disable PIO control of the peripheral lines
	PIOA->PIO_PDR |= WATTR_PERIPH_LINES(AOR,X03,X03,X03);
	PIOB->PIO_PDR |= WATTR_PERIPH_LINES(X03,AOR,X03,X03);
	PIOC->PIO_PDR |= WATTR_PERIPH_LINES(X03,X03,AOR,X03);
	PIOD->PIO_PDR |= WATTR_PERIPH_LINES(X03,X03,X03,AOR);
	
	//Enable Peripheral control of the lines
	PIOA->PIO_ABCDSR[0] = WATTR_PERIPH_LINES(BOR,X03,X03,X03);
	PIOA->PIO_ABCDSR[1] = WATTR_PERIPH_LINES(COR,X03,X03,X03);
	PIOB->PIO_ABCDSR[0] = WATTR_PERIPH_LINES(X03,BOR,X03,X03);
	PIOB->PIO_ABCDSR[1] = WATTR_PERIPH_LINES(X03,COR,X03,X03);
	PIOC->PIO_ABCDSR[0] = WATTR_PERIPH_LINES(X03,X03,BOR,X03);
	PIOC->PIO_ABCDSR[1] = WATTR_PERIPH_LINES(X03,X03,COR,X03);
	PIOD->PIO_ABCDSR[0] = WATTR_PERIPH_LINES(X03,X03,X03,BOR);
	PIOD->PIO_ABCDSR[1] = WATTR_PERIPH_LINES(X03,X03,X03,COR);
}
//Configure lines that are used as outputs.
#define X11(a) a |
static void wattr_output_config(void)
{
	//Enable PIO control of lines
	PIOA->PIO_PER = WATTR_OUTPUT_LINES(X11,X01,X01,X01);
	PIOB->PIO_PER = WATTR_OUTPUT_LINES(X01,X11,X01,X01);
	PIOC->PIO_PER = WATTR_OUTPUT_LINES(X01,X01,X11,X01);
	PIOD->PIO_PER = WATTR_OUTPUT_LINES(X01,X01,X01,X11);
	
	//Enable outputs on the lines
	PIOA->PIO_OER = WATTR_OUTPUT_LINES(X11,X01,X01,X01);
	PIOB->PIO_OER = WATTR_OUTPUT_LINES(X01,X11,X01,X01);
	PIOC->PIO_OER = WATTR_OUTPUT_LINES(X01,X01,X11,X01);
	PIOD->PIO_OER = WATTR_OUTPUT_LINES(X01,X01,X01,X11);
	
	PIOA->PIO_OWER = WATTR_OUTPUT_LINES(X11,X01,X01,X01);
	PIOB->PIO_OWER = WATTR_OUTPUT_LINES(X01,X11,X01,X01);
	PIOC->PIO_OWER = WATTR_OUTPUT_LINES(X01,X01,X11,X01);
	PIOD->PIO_OWER = WATTR_OUTPUT_LINES(X01,X01,X01,X11);
}

void pio_config(void)
{
	//Enable the clocks for each PIO controller
	PMC->PMC_PCER0 = PMC_PCER0_PID9 | PMC_PCER0_PID10 |
		PMC_PCER0_PID11 | PMC_PCER0_PID12;
	wattr_periph_config();
	wattr_output_config();
	config_pio_irq();
	//Enable all pio interrupts that may be used
	if(pioa_icount){
		NVIC_SetPriority(PIOA_IRQn,5);
		NVIC_EnableIRQ(PIOA_IRQn);
	}
	if(piob_icount){
		NVIC_SetPriority(PIOB_IRQn,5);
		NVIC_EnableIRQ(PIOB_IRQn);
	}
	if(pioc_icount){
		NVIC_SetPriority(PIOC_IRQn,5);
		NVIC_EnableIRQ(PIOC_IRQn);
	}
	if(piod_icount){
		NVIC_SetPriority(PIOD_IRQn,5);
		NVIC_EnableIRQ(PIOD_IRQn);
	}
}

//Button press handlers - likely outsourced to ATTINY
void fp_ubutton_handler()
{
	
}

void fp_lbutton_handler()
{
	
}

void fp_rbutton_handler()
{
	
}

void fp_dbutton_handler()
{
	
}

void back_button_handler()
{
	
}
