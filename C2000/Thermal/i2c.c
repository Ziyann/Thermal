/*
 * i2c.c
 *
 *  Created on: Jun 29, 2016
 *      Author: Daniel Jarai
 */

#include "includes/i2c.h"

#include "f2802x_headers/include/F2802x_Device.h"
#include "f2802x_common/include/f2802x_examples.h"
#include "f2802x_common/include/clk.h"
#include "f2802x_common/include/gpio.h"


static void i2c_start(uint16_t slave_address) {
	// load slave address
	I2caRegs.I2CSAR = slave_address;

	// wait for STOP condition
	while (I2caRegs.I2CMDR.bit.STP != 0)
		;

	// master mode
	I2caRegs.I2CMDR.bit.MST = 1;

	// generate START condition
	I2caRegs.I2CMDR.bit.STT = 1;
}

static uint16_t i2c_write_byte(uint16_t byte) {
	// repeat mode
	I2caRegs.I2CMDR.bit.RM = 1;

	// transmit mode
	I2caRegs.I2CMDR.bit.TRX = 1;

	// wait for XRDY flag to transmit data
	while (I2caRegs.I2CSTR.bit.XRDY != 1)
		;

	// load data into the transmit register
	I2caRegs.I2CDXR = byte;

	// wait for XRDY flag to transmit data or ARDY if we get NACKed
	while (I2caRegs.I2CSTR.bit.XRDY != 1 || I2caRegs.I2CSTR.bit.ARDY != 1)
		;

	// if a NACK is received, clear the NACK bit and stop the transfer
	if (I2caRegs.I2CSTR.bit.NACK == 1) {
		I2caRegs.I2CMDR.bit.STP = 1;
		I2caRegs.I2CSTR.all = I2C_CLR_NACK_BIT;
		return I2C_NACK_ERROR;
	}

	return I2C_SUCCESS;
}

uint16_t i2c_read_block(uint16_t slave_address, uint16_t read_address,
		uint16_t count, uint16_t *out) {
	uint16_t i, ret;

	i2c_start(slave_address);

	ret = i2c_write_byte(read_address);
	if (ret)
		return ret;

	// issue repeated start
	I2caRegs.I2CMDR.bit.STT = 1;

	// wait for repeated start to end
	while (I2caRegs.I2CMDR.bit.STT != 0)
		;

	// non-repeat mode
	I2caRegs.I2CMDR.bit.RM = 0;

	// set data count
	I2caRegs.I2CCNT = count;

	// generate STOP condition when the data counter counts down to 0
	I2caRegs.I2CMDR.bit.STP = 1;

	// receiver mode
	I2caRegs.I2CMDR.bit.TRX = 0;

	for (i = 0; i < count; ++i) {
		// wait until the data receive register is ready to be read
		while (I2caRegs.I2CSTR.bit.RRDY != 1)
			;

		out[i] = I2caRegs.I2CDRR;
	}

	return I2C_SUCCESS;
}

uint16_t i2c_read_byte(uint16_t slave_address, uint16_t read_address) {
	uint16_t ret, val;

	ret = i2c_read_block(slave_address, read_address, 1, &val);
	if (ret)
		return ret;

	return val;
}

static void i2c_init_gpio() {
	GPIO_Handle myGpio = GPIO_init((void *) GPIO_BASE_ADDR, sizeof(GPIO_Obj));

	GPIO_setPullUp(myGpio, GPIO_Number_32, GPIO_PullUp_Enable);
	GPIO_setPullUp(myGpio, GPIO_Number_33, GPIO_PullUp_Enable);

	GPIO_setQualification(myGpio, GPIO_Number_32, GPIO_Qual_ASync);
	GPIO_setQualification(myGpio, GPIO_Number_33, GPIO_Qual_ASync);

	GPIO_setMode(myGpio, GPIO_Number_32, GPIO_32_Mode_SDAA);
	GPIO_setMode(myGpio, GPIO_Number_33, GPIO_33_Mode_SCLA);
}

void i2c_init() {
	CLK_Handle myClk = CLK_init((void *) CLK_BASE_ADDR, sizeof(CLK_Obj));

	i2c_init_gpio();

	CLK_enableI2cClock(myClk);

	// place I2C module in reset
	I2caRegs.I2CMDR.bit.IRS = 0;

	// module clock - must be between 7-12 MHz
	// module clock = cpu clock / (IPSC + 1)
	// 60 MHz / 6 => 10 MHz
	I2caRegs.I2CPSC.all = 5;

	// master clock
	// high-time = Tmod * (ICCH + d) = 12.5 us
	// low-time = Tmod * (ICCL + d) = 12.5 us
	// 25 us -> 40 kHz
	I2caRegs.I2CCLKL = 120;
	I2caRegs.I2CCLKH = 120;

	// enable I2C module
	I2caRegs.I2CMDR.bit.IRS = 1;
}
