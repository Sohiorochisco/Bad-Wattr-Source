/*
 * ade7753.h
 *
 * Created: 12 Nov 2013 10:12:55 AM
 *  Author: Andrew
 *  Heavily modified by Mitchell
 */

#ifndef ADE7753_H_
#define ADE7753_H_

#define ADE_SPI_CS						1
#define ADE_SPI_CLK_POLARITY			0
#define ADE_SPI_CLK_PHASE				0
#define ADE_SPI_DLYBS					0x40
#define ADE_SPI_DLYBCT					0x10
static uint32_t gs_ul_spi_clock = 500000;

#define VRMS_SLOPE					(0.000237748f)
#define VRMS_INTERCEPT				(-0.14427)
#define IRMS_SLOPE					113240.82786f
#define IRMS_INTERCEPT				953.97194f
#define ACTIVE_POWER_SLOPE			0.016121f
#define ACTIVE_POWER_INTERCEPT		7.0595f
#define APPARENT_POWER_SLOPE		0.019658f
#define APPARENT_POWER_INTERCEPT	11.7168f
#define ACTIVE_ENERGY_SLOPE			0.000075018f
#define ACTIVE_ENERGY_INTERCEPT		0.03266f
#define POWER_FACTOR				0.827f
#define CENTS_PER_KWH				0.13f

// lists bytes necessary to transfer in order to read or write to this register.
#define ADE_1ZBYTES ,0x00
#define ADE_2ZBYTES ,0x00,0x00,
#define ADE_3ZBYTES , 0x00,0x00,0x00

// All write operations must be OR'ed with 0b10......
#define ADE_WRITE_MASK					0x80

// Lists of things that might have gone wrong...
#define ADE_FLAGS_NONE					0x00
#define ADE_FLAGS_SAG					0x01
#define ADE_FLAGS_PKV					0x02
#define ADE_FLAGS_PKI					0x04

// Gains for the GAIN register
#define ADE_GAIN_PGA1_1					0x00
#define ADE_GAIN_PGA1_2					0x01
#define ADE_GAIN_PGA1_4					0x02
#define ADE_GAIN_PGA1_8					0x03
#define ADE_GAIN_PGA1_16				0x04

#define ADE_GAIN_C1FS_0_5V				0x00
#define ADE_GAIN_C1FS_0_25V				0x08
#define ADE_GAIN_C1FS_0_125V			0x10

#define ADE_GAIN_PGA2_1					0x00
#define ADE_GAIN_PGA2_2					0x20
#define ADE_GAIN_PGA2_4					0x40
#define ADE_GAIN_PGA2_8					0x60
#define ADE_GAIN_PGA2_16				0x80

// Listing of registers on the chip, with the size of the register in bytes. See Page 52-54 on the datasheet
// Waveform sampling register
#define ADE_REG_WF				0x01
#define ADE_REG_WF_WBITS			24

//Active Energy register
#define ADE_REG_AENERGY				0x02
#define ADE_REG_AENERGY_WBITS			24

//Reactive energy register
#define ADE_REG_RAENERGY			0x03
#define ADE_REG_RAENERGY_WBITS			24

//Bizarre "line energy" quantity...
#define ADE_REG_LAENERGY			0x04
#define ADE_REG_LAENERGY_WBITS			24

//Apparent energy register
#define ADE_REG_VAENERGY			0x05
#define ADE_REG_VAENERGY_WBITS			24

#define ADE_REG_RVAENERGY			0x06
#define ADE_REG_RVAENERGY_WBITS			24

#define ADE_REG_LVAENERGY			0x07
#define ADE_REG_LVAENERGY_WBITS			24

#define ADE_REG_LVARENERGY			0x08
#define ADE_REG_LVARENERGY_WBITS		24

#define ADE_REG_MODE				0x09
#define ADE_REG_MODE_WBITS			16

#define ADE_REG_IRQEN				0x0A
#define ADE_REG_IRQEN_WBITS			16

#define ADE_REG_STATUS				0x0B
#define ADE_REG_STATUS_WBITS			16

#define ADE_REG_RSTSTATUS			0x0C
#define ADE_REG_RSTSTATUS_WBITS			16

#define ADE_REG_CH1OS				0x0D
#define ADE_REG_CH_WBITS			8

#define ADE_REG_CH2OS				0x0E
#define ADE_REG_CH_WBITS			8

#define ADE_REG_GAIN				0x0F
#define ADE_REG_GAIN_WBITS			8

#define ADE_REG_PHCAL				0x10
#define ADE_REG_PHCAL_WBITS			6

#define ADE_REG_APOS				0x11
#define ADE_REG_APOS_WBITS			12

#define ADE_REG_WGAIN				0x12
#define ADE_REG_WGAIN_WBITS			12

#define ADE_REG_WDIV				0x13
#define ADE_REG_WDIV_WBITS			8

#define ADE_REG_CFNUM				0x14
#define ADE_REG_CFNUM_WBITS			12

#define ADE_REG_CFDEN				0x15
#define ADE_REG_CFDEN_WBITS			12

#define ADE_REG_IRMS				0x16
#define ADE_REG_IRMS_WBITS			24

#define ADE_REG_VRMS				0x17
#define ADE_REG_VRMS_WBITS			24

#define ADE_REG_IRMSOS				0x18
#define ADE_REG_IRMSOS_WBITS			12

#define ADE_REG_VRMSOS				0x19
#define ADE_REG_VRMSOS_WBITS			12

#define ADE_REG_VAGAIN				0x1A
#define ADE_REG_VAGAIN_WBITS			12

#define ADE_REG_VADIV				0x1B
#define ADE_REG_VADIV_WBITS			8

#define ADE_REG_LINECYC				0x1C
#define ADE_REG_LINECYC_WBITS			16

#define ADE_REG_ZXTOUT				0x1D
#define ADE_REG_ZXTOUT_WBITS			12

#define ADE_REG_SAGCYC				0x1E
#define ADE_REG_SAGCYC_WBITS			8

#define ADE_REG_SAGLVL				0x1F
#define ADE_REG_SAGLVL_WBITS			8

#define ADE_REG_IPKLVL				0x20
#define ADE_REG_IPKLVL_WBITS			8

#define ADE_REG_VPKLVL				0x21
#define ADE_REG_VPKLVL_WBITS			8

#define ADE_REG_IPEAK				0x22
#define ADE_REG_IPEAK_WBITS			24

#define ADE_REG_RSTIPEAK			0x23
#define ADE_REG_RSTIPEAK_WBITS			24

#define ADE_REG_VPEAK				0x24
#define ADE_REG_VPEAK_WBITS			24

#define ADE_REG_RSTVPEAK			0x25
#define ADE_REG_RSTVPEAK_WBITS			24

#define ADE_REG_TEMP				0x26
#define ADE_REG_TEMP_WBITS			8

#define ADE_REG_PERIOD				0x27
#define ADE_REG_PERIOD_WBITS			16

#define ADE_REG_CHKSUM				0x3E
#define ADE_REG_CHKSUM_WBITS			6

#define ADE_REG_DIEREV				0x3F
#define ADE_REG_DIEREV_WBITS			8




//Define buffers for commands to send to the ADE7753
#define ADE_STARTUP_BUF_ID			0
#define ADE_ZX_BUF_ID				1
#define ADE_IR1_BUF_ID				2


int ade_transfer(int buffer_id);

#endif /* ADE7753_H_ */
