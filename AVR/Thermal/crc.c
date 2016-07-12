#include "include/crc.h"

uint8_t crc8_ccitt(const uint8_t *data, uint8_t len) {
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