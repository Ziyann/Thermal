#include "include/control.h"
#include "include/pwm.h"
#include "include/uart.h"
#include "include/mlx90614.h"
#include "include/servo.h"

#include <stdlib.h>

int16_t get_param_numeric(uint8_t index, char *command) {
    uint8_t beginning = 0;

    while (index--) {
        do {
            ++beginning;
        } while (*(command + beginning) != ' ');
        ++beginning;
    }

    return atoi(command + beginning);
}

void parse_command(char *command) {
    switch (command[0]) {
        case CMD_SET_HORIZ_ANGLE:
            set_servo_pos_relative_deg(PIN_HORIZ_SERVO, get_param_numeric(1, command));
            break;
        case CMD_SET_VERT_ANGLE:
            set_servo_pos_relative_deg(PIN_VERT_SERVO, get_param_numeric(1, command));
            break;
        case CMD_RESET_SERVOS:
            servo_stabilize(PIN_HORIZ_SERVO);
            servo_stabilize(PIN_VERT_SERVO);
			break;
        case CMD_GET_AMB_TEMP:
            uart_sendint(mlx90614_get_amb_temp());
            break;
        case CMD_GET_OBJ_TEMP:
            uart_sendint(mlx90614_get_obj_temp());
            break;
        case CMD_GET_IDENT:
            uart_sendstring("MLX90614-based Thermal Camera\n");
            break;
        default:
            uart_sendstring("Unknown command\n");
    }
}