/*
 * mlx90614.c
 *
 *  Created on: Jun 30, 2016
 *      Author: Twi
 */

#include "includes/crc.h"
#include "includes/i2c.h"
#include "includes/sci.h"

#define MLX90614_I2CADDR 0x5A

// RAM
#define MLX90614_RAWIR1 0x04
#define MLX90614_RAWIR2 0x05
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08

// EEPROM
#define MLX90614_TOMAX 0x20
#define MLX90614_TOMIN 0x21
#define MLX90614_PWMCTRL 0x22
#define MLX90614_TARANGE 0x23
#define MLX90614_EMISS 0x24
#define MLX90614_CONFIG 0x25
#define MLX90614_ADDR 0x0E
#define MLX90614_ID1 0x3C
#define MLX90614_ID2 0x3D
#define MLX90614_ID3 0x3E
#define MLX90614_ID4 0x3F

// defines for PEC
#define I2C_READ	1
#define I2C_WRITE	0

uint16_t read16(uint16_t address) {
	uint16_t ret, pec, out[3];

	ret = i2c_read_block(MLX90614_I2CADDR, address, 3, out);
	if (ret)
		return ret;

	ret = out[0];
	ret |= out[1] << 8;
	pec = out[2];

	uint16_t commArray[5] = { (MLX90614_I2CADDR << 1) + I2C_WRITE, // SA_W
			address & 0xFF,	// Command
			(MLX90614_I2CADDR << 1) + I2C_READ,	// SA_R
			out[0] & 0xFF, 	// LSB
			out[1] & 0xFF 	// MSB
	};

	uint16_t crc = crc8_ccitt(commArray, 5);

	if (pec != crc) {
		SCI_putStringBlocking("PEC error!\n");
		return 0; // PEC error
	}

	return ret;
}

int32_t readTemp(uint16_t reg) {
	int32_t temp = ((int32_t) read16(reg)) * 100;

	temp /= 50;
	temp -= 27315;

	return temp;
}

int32_t mlx90614_get_obj_temp() {
	return readTemp(MLX90614_TOBJ1);
}

int32_t mlx90614_get_amb_temp() {
	return readTemp(MLX90614_TA);
}
