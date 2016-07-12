/*
 * crc.h
 *
 *  Created on: Jul 2, 2016
 *      Author: Twi
 */

#ifndef INCLUDES_CRC_H_
#define INCLUDES_CRC_H_

#include "f2802x_headers/include/F2802x_Device.h"

uint16_t crc8_ccitt(const uint16_t *data, uint16_t len);

#endif /* INCLUDES_CRC_H_ */
