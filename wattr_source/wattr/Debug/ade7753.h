/*
 * ade7753.h
 *
 * Created: 12 Nov 2013 10:12:55 AM
 *  Author: Andrew
 */ 

#ifndef ADE7753_H_
#define ADE7753_H_

#define ADE_SPI_CS			  1
#define ADE_SPI_CLK_POLARITY		  0
#define ADE_SPI_CLK_PHASE			  0
#define ADE_SPI_DLYBS				  0x40
#define ADE_SPI_DLYBCT				  0x10
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


// The number of bits the register is in bytes
#define BITS6  1
#define BITS8  1
#define BITS12 2
#define BITS16 2
#define BITS24 3

// All write operations must be OR'ed with 0b10......
#define ADE_WRITE_COMMAND				0x80

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
#define ADE_GAIN_C1FS_0_125V				0x10

#define ADE_GAIN_PGA2_1					0x00
#define ADE_GAIN_PGA2_2					0x20
#define ADE_GAIN_PGA2_4					0x40
#define ADE_GAIN_PGA2_8					0x60
#define ADE_GAIN_PGA2_16				0x80

// Listing of registers on the chip. See Page 52-54 on the datasheet
#define ADE_REG_WF			0x01
#define ADE_REG_WF_BYTES		BITS24

#define ADE_REG_AENERGY			0x02
#define ADE_REG_AENERGY_BYTES		BITS24

#define ADE_REG_RAENERGY			0x03
#define ADE_REG_RAENERGY_BYTES		BITS24

#define ADE_REG_LAENERGY			0x04
#define ADE_REG_LAENERGY_BYTES		BITS24

#define ADE_REG_VAENERGY			0x05
#define ADE_REG_VAENERGY_BYTES		BITS24

#define ADE_REG_RVAENERGY			0x06
#define ADE_REG_RVAENERGY_BYTES	BITS24

#define ADE_REG_LVAENERGY			0x07
#define ADE_REG_LVAENERGY_BYTES	BITS24

#define ADE_REG_LVARENERGY			0x08
#define ADE_REG_LVARENERGY_BYTES	BITS24

#define ADE_REG_MODE				0x09
#define ADE_REG_MODE_BYTES			BITS16

#define ADE_REG_IRQEN				0x0A
#define ADE_REG_IRQEN_BYTES		BITS16

#define ADE_REG_STATUS				0x0B
#define ADE_REG_STATUS_BYTES		BITS16

#define ADE_REG_RSTSTATUS			0x0C
#define ADE_REG_RSTSTATUS_BYTES	BITS16

#define ADE_REG_CH1OS				0x0D
#define ADE_REG_CH1OS_BYTES		BITS8

#define ADE_REG_CH2OS				0x0E
#define ADE_REG_CH2OS_BYTES		BITS8

#define ADE_REG_GAIN				0x0F
#define ADE_REG_GAIN_BYTES			BITS8

#define ADE_REG_PHCAL				0x10
#define ADE_REG_PHCAL_BYTES		BITS6

#define ADE_REG_APOS				0x11
#define ADE_REG_APOS_BYTES			BITS16

#define ADE_REG_WGAIN				0x12
#define ADE_REG_WGAIN_BYTES		BITS12

#define ADE_REG_WDIV				0x13
#define ADE_REG_WDIV_BYTES			BITS8

#define ADE_REG_CFNUM				0x14
#define ADE_REG_CFNUM_BYTES		BITS12

#define ADE_REG_CFDEN				0x15
#define ADE_REG_CFDEN_BYTES		BITS12

#define ADE_REG_IRMS				0x16
#define ADE_REG_IRMS_BYTES			BITS24

#define ADE_REG_VRMS				0x17
#define ADE_REG_VRMS_BYTES			BITS24

#define ADE_REG_IRMSOS				0x18
#define ADE_REG_IRMSOS_BYTES		BITS12

#define ADE_REG_VRMSOS				0x19
#define ADE_REG_VRMSOS_BYTES		BITS12

#define ADE_REG_VAGAIN				0x1A
#define ADE_REG_VAGAIN_BYTES		BITS12

#define ADE_REG_VADIV				0x1B
#define ADE_REG_VADIV_BYTES		BITS8

#define ADE_REG_LINECYC			0x1C
#define ADE_REG_LINECYC_BYTES		BITS16

#define ADE_REG_ZXTOUT				0x1D
#define ADE_REG_ZXTOUT_BYTES		BITS12

#define ADE_REG_SAGCYC				0x1E
#define ADE_REG_SAGCYC_BYTES		BITS8

#define ADE_REG_SAGLVL				0x1F
#define ADE_REG_SAGLVL_BYTES		BITS8

#define ADE_REG_IPKLVL				0x20
#define ADE_REG_IPKLVL_BYTES		BITS8

#define ADE_REG_VPKLVL				0x21
#define ADE_REG_VPKLVL_BYTES		BITS8

#define ADE_REG_IPEAK				0x22
#define ADE_REG_IPEAK_BYTES		BITS24

#define ADE_REG_RSTIPEAK			0x23
#define ADE_REG_RSTIPEAK_BYTES		BITS24

#define ADE_REG_VPEAK				0x24
#define ADE_REG_VPEAK_BYTES		BITS24

#define ADE_REG_RSTVPEAK			0x25
#define ADE_REG_RSTVPEAK_BYTES		BITS24

#define ADE_REG_TEMP				0x26
#define ADE_REG_TEMP_BYTES			BITS8

#define ADE_REG_PERIOD				0x27
#define ADE_REG_PERIOD_BYTES		BITS16

#define ADE_REG_CHKSUM				0x3E
#define ADE_REG_CHKSUM_BYTES		BITS6

#define ADE_REG_DIEREV				0x3F
#define ADE_REG_DIEREV_BYTES		BITS8

void spi_master_initialize(void);
void spi_master_transfer(void *p_buf, uint32_t size);

void ade7753_read(uint8_t ic_register, uint32_t *data, uint8_t length, uint8_t *checksum);
void ade7753_write(uint8_t ic_register, uint32_t *data, uint8_t length);
uint8_t verify_result(uint32_t *result, uint8_t *checksum);

void ade7753_calibrate_watt(void);
void ade7753_calibrate_watt_offset(void);
void ade7753_calibrate_phase(void);


#endif /* ADE7753_H_ */
