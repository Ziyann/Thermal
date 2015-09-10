#ifndef CONTROL_H_
#define CONTROL_H_

#include <stdint.h>

#define HORIZ_SERVO_FULL_RIGHT	2200
#define HORIZ_SERVO_RIGHT_45D	1700
#define HORIZ_SERVO_CENTER		1260
#define HORIZ_SERVO_LEFT_45D	850
#define HORIZ_SERVO_FULL_LEFT	480

#define VERT_SERVO_FULL_UP		2450
#define VERT_SERVO_UP_45D		2050
#define VERT_SERVO_CENTER		1600
#define VERT_SERVO_DOWN_45D		1200
#define VERT_SERVO_FULL_DOWN	700

#define PIN_HORIZ_SERVO 1
#define PIN_VERT_SERVO	2

#define CMD_SET_HORIZ_ANGLE	'h'
#define CMD_SET_VERT_ANGLE	'v'

#define CMD_GET_AMB_TEMP	'a'
#define CMD_GET_OBJ_TEMP	'o'
#define CMD_GET_IDENT		'i'

void parse_command(char*);

#endif /* CONTROL_H_ */