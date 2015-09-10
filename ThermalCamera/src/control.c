#include "control.h"
#include "pwm.h"
#include "uart.h"
#include "mlx90614.h"

#include <stdlib.h>

uint16_t get_param_numeric(uint8_t, char*);

uint16_t get_param_numeric(uint8_t index, char* command) {
	uint8_t beginning = 0;
	
	while (index--) {
		do {
			++beginning;
		} while (*(command + beginning) != ' ');
		++beginning;
	}
	
	return atoi(command + beginning);
}

void parse_command(char* command) {
	switch (command[0]) {
		case CMD_SET_HORIZ_ANGLE:
		pwm_set_pulse_duration(PIN_HORIZ_SERVO, get_param_numeric(1, command));
		break;
		case CMD_SET_VERT_ANGLE:
		pwm_set_pulse_duration(PIN_VERT_SERVO, get_param_numeric(1, command));
		break;
		case CMD_GET_AMB_TEMP:
		uart_sendint(mlx90614_get_amb_temp());
		break;
		case CMD_GET_OBJ_TEMP:
		uart_sendint(mlx90614_get_obj_temp());
		break;
		case CMD_GET_IDENT:
		uart_sendstring("MLX90614 Thermal Camera\n");
		break;
	}
}