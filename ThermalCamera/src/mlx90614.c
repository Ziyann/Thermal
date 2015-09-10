#include "mlx90614.h"
#include "i2c.h"

#include <conf_board.h>

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

int32_t readTemp(uint8_t);
uint16_t read16(uint8_t);

int32_t mlx90614_get_obj_temp() {
  return readTemp(MLX90614_TOBJ1);
}

int32_t mlx90614_get_amb_temp() {
  return readTemp(MLX90614_TA);
}

int32_t readTemp(uint8_t reg) {
	int32_t temp = ((int32_t) read16(reg)) * 100;
  
	temp /= 50;
	temp -= 27315;
  
	return temp;
}

uint16_t read16(uint8_t address) {
	uint16_t ret = 0;

	i2c_start_wait(MLX90614_I2CADDR << 1);
	
	i2c_write(address); // send register address to read from
	
    i2c_start((MLX90614_I2CADDR << 1) + I2C_READ);
    ret = i2c_read_ack();
    ret |= i2c_read_ack() << 8;
    i2c_read_nack();
	
    i2c_stop();

	return ret;
}