/*
 * control.c
 *
 *  Created on: Jul 2, 2016
 *      Author: Twi
 */

#include "f2802x_headers/include/F2802x_Device.h"

#include <stdlib.h>
#include "includes/mlx90614.h"
#include "includes/sci.h"
#include "includes/servo.h"

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

#define PIN_HORIZ_SERVO 	1
#define PIN_VERT_SERVO		2

#define CMD_SET_HORIZ_ANGLE	'h'
#define CMD_SET_VERT_ANGLE	'v'

#define CMD_RESET_SERVOS	'r'

#define CMD_GET_AMB_TEMP	'a'
#define CMD_GET_OBJ_TEMP	'o'
#define CMD_GET_IDENT		'i'

int get_param_numeric(int index, char *command) {
	int beginning = 0;

	while (index--) {
		do {
			++beginning;
		} while (*(command + beginning) != ' ');
		++beginning;
	}

	return atoi(command + beginning);
}

void parse_command(char *command) {
	char buffer[16];

	switch (command[0]) {
	case CMD_SET_HORIZ_ANGLE:
		servo_set_angle(PIN_HORIZ_SERVO, get_param_numeric(1, command));
		break;
	case CMD_SET_VERT_ANGLE:
		servo_set_angle(PIN_VERT_SERVO, get_param_numeric(1, command));
		break;
	case CMD_RESET_SERVOS:
		servos_reset();
		break;
	case CMD_GET_AMB_TEMP:
		ltoa(mlx90614_get_amb_temp(), buffer);
		SCI_putStringBlocking(buffer);
		SCI_putByteBlocking('\n');
		break;
	case CMD_GET_OBJ_TEMP:
		ltoa(mlx90614_get_obj_temp(), buffer);
		SCI_putStringBlocking(buffer);
		SCI_putByteBlocking('\n');
		break;
	case CMD_GET_IDENT:
		SCI_putStringBlocking("MLX90614-based Thermal Camera\n");
		break;
	default:
		SCI_putStringBlocking("Unknown command\n");
	}
}

