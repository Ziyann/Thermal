#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>

#define PIN_HORIZ_SERVO 1
#define PIN_VERT_SERVO	2

// measured
#define HORIZ_SERVO_FULL_RIGHT	2200
#define HORIZ_SERVO_RIGHT_45D	1700
#define HORIZ_SERVO_CENTER		1260
#define HORIZ_SERVO_LEFT_45D	850
#define HORIZ_SERVO_FULL_LEFT	480

// measured
#define VERT_SERVO_FULL_UP		2450
#define VERT_SERVO_UP_45D		2050
#define VERT_SERVO_CENTER		1600
#define VERT_SERVO_DOWN_45D		1200
#define VERT_SERVO_FULL_DOWN	700

void set_servo_pos_relative_deg(uint8_t pin, int16_t deg);

void servo_stabilize(uint8_t pin);

#endif /* SERVO_H_ */