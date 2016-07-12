/*
 * crc.c
 *
 *  Created on: Jul 2, 2016
 *      Author: Twi
 */

#include "includes/crc.h"

uint16_t crc8_ccitt(const uint16_t *data, uint16_t len) {
	int i, j;
	int c;
	int CRC = 0;
	int genPoly = 0x07;

	for (j = 0; j < len; ++j) {
		c = data[j];
		CRC ^= c;
		for (i = 0; i < 8; ++i) {
			if (CRC & 0x80) {
				CRC = (CRC << 1) ^ genPoly;
			} else {
				CRC <<= 1;
			}
		}
		CRC &= 0xff;
	}

	return CRC;
}
