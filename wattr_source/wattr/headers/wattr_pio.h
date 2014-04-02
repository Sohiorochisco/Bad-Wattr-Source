/*
 * wattr_io.h
 * Contains definitions for the PIO lines used for Wattr,
 * including a (slight) abuse of the preprocessor using X-macros.
 *
 * Created: 3/22/2014 12:28:08 AM
 *  Author: mhaynes
 */ 

/*Pio Interrupt Handler Definitions */

//void ade_irq_handler(void)
//void ade_zx_handler(void)

void back_button_handler(void);
void fp_ubutton_handler(void);
void fp_dbutton_handler(void);
void fp_lbutton_handler(void);
void fp_rbutton_handler(void);
void ade_zx_handler(void);
void ade_irq_handler(void);

//Method for configuring pio pins
void pio_config(void);