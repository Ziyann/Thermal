#include "config/conf_board.h"

#include "include/mlx90614.h"
#include "include/i2c.h"
#include "include/crc.h"

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

uint16_t read16(uint8_t address) {
	uint8_t err, pec;
	uint16_t ret = 0;

	i2c_start_wait(MLX90614_I2CADDR << 1);

	err = i2c_write(address); // send register address to read from
	if (err > 0) {
		return 0;
	}

	err = i2c_start((MLX90614_I2CADDR << 1) + I2C_READ);
	if (err > 0) {
		return 0;
	}
	
	ret = i2c_read_ack();
	ret |= i2c_read_ack() << 8;
	pec = i2c_read_nack();
	
	i2c_stop();

	uint8_t commArray[5] = {MLX90614_I2CADDR << 1,
		address,
		(MLX90614_I2CADDR << 1) + I2C_READ,
		ret & 0xFF,
		(ret >> 8) & 0xFF
	};

	uint8_t crc = crc8_ccitt(commArray, 5);

	if (pec != crc) { // PEC error
		return 0;
	}

	return ret;
}

int32_t readTemp(uint8_t reg) {
	uint16_t reading = read16(reg);
	
	if (reading == 0) {
		return -1;
	}
	
    int32_t temp = ((int32_t) reading) * 100;

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