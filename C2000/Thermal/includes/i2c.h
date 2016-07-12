/*
 * i2c.h
 *
 *  Created on: Jun 29, 2016
 *      Author: Daniel Jarai
 */

#ifndef INCLUDES_I2C_H_
#define INCLUDES_I2C_H_

#include "f2802x_headers/include/F2802x_Device.h"

uint16_t i2c_read_block(uint16_t slave_address, uint16_t read_address,
		uint16_t count, uint16_t *out);
uint16_t i2c_read_byte(uint16_t slave_address, uint16_t read_address);

void i2c_init();

#endif /* INCLUDES_I2C_H_ */
