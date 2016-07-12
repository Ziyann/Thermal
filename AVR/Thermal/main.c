#include "config/conf_board.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>

#include "include/control.h"
#include "include/i2c.h"
#include "include/mlx90614.h"
#include "include/pwm.h"
#include "include/servo.h"
#include "include/uart.h"

int main(void)
{
	pwm_setup();
	uart_setup();
	
	uart_enable_rx_pullup();
	
	i2c_init();
	
	servo_stabilize(PIN_HORIZ_SERVO);
	servo_stabilize(PIN_VERT_SERVO);
	
	while (1) {
		;
	}
}

ISR(USART_RX_vect)
{
	static uint8_t cnt;
	static char cmd[16];
	
    cmd[cnt] = uart_getchar();
	
    if (cmd[cnt] == '\n') {
		parse_command(cmd);
		cnt = 0;
		memset(cmd, 0x0, 16);
	} else {
	    ++cnt;
    }
}