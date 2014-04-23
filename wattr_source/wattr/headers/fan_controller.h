/*
 * fan_controller.h
 * register and position definitions for the MAX6615 temperature 
 * and fan controller
 * Created: 4/22/2014 12:39:24 PM
 *  Author: mhaynes
 */ 

/* MAX6615 register definitions */

//First temperature reading channel
#define FC_REG_TEMP_CH1 0x0u
//Second temperature reading channel
#define FC_REG_TEMP_CH2 0x1u
//Configuration Byte
#define FC_REG_CONFIG_BYTE 0x2u
//Temp channel 1 overtemp limit
#define FC_REG_TC1OT_LIM 0x3u
//Temp channel 2 overtemp limit
#define FC_REG_TC2OT_LIM 0x4u
//Over temperature status
#define FC_REG_OT_STAT 0x5u
//Over temperature mask
#define FC_REG_OT_MASK 0x6u
//PWM1 start duty cycle
#define FC_REG_PWM1_DC 0x7u
//PWM2 start duty cycle 
#define FC_REG_PWM2_SDC 0x8u
//PWM1 max duty cycle
#define FC_REG_PWM1_MDC 0x9u
//PWM2 max duty cycle
#define FC_REG_PWM2_MDC 0xAu
//PWM1 target duty cycle
#define FC_REG_PWM1_TDC 0xBu
//PWM2 target duty cycle
#define FC_REG_PWM2_TDC 0xCu
//PWM1 instantaneous duty cycle
#define FC_REG_PWM1_IDC 0xDu
//PWM2 instantaneous duty cycle
#define FC_REG_PWM2_IDC 0xEu
//Channel 1 fan start temperature
#define FC_REG_CH1_FST 0xFu
//Channel 2 fan start temperature
#define FC_REG_CH2_FST 0x10u
//Fan configuration
#define FC_REG_FANCONFIG 0x11u
//Duty cycle rate of change
#define FC_REG_DC_ROC 0x12u
//Duty cycle step size
#define FC_REG_DC_SS 0x13u
//PWM frequency select
#define FC_REG_PWM_FS 0x14u
//GPIO functions
#define FC_REG_GPIO_FUNC 0x15u
//GPIO values
#define FC_REG_GPIO_VAL 0x16u
//Thermistor offset
#define FC_REG_THERM_OFFS 0x17u
//Tachometer 1 value 
#define FC_REG_TACH1_VAL 0x18u
//Tachometer 2 value
#define FC_REG_TACH2_VAL 0x19u
//Tachometer 1 limit
#define FC_REG_TACH1_LIM 0x1Au
//Tachometer 2 limit
#define FC_REG_TACH2_LIM 0x1Bu
//Fan status byte
#define FC_REG_FSB 0x1Cu
//Channel 1 temperature least significant bits
#define FC_REG_CH1_LSBs 0x1Eu
//Channel 2 temperature least significant bits
#define FC_REG_CH2_LSBs 0x1Fu

/////////////////////////////////////////////////////////////////

#define FC_HARDWARE_ADDR 0x18
