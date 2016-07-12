#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>

uint8_t crc8_ccitt(const uint8_t *data, uint8_t len);

#endif /* CRC_H_ */