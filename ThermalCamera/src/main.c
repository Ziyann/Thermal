#include <asf.h>

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>

#include "control.h"
#include "i2c.h"
#include "mlx90614.h"
#include "pwm.h"
#include "servo.h"
#include "uart.h"

int main(void)
{
	pwm_setup();
	uart_setup();
	
	uart_enable_rx_pullup();
	
	i2c_init();
	
	pwm_set_pulse_duration(PIN_HORIZ_SERVO, HORIZ_SERVO_CENTER);
	pwm_set_pulse_duration(PIN_VERT_SERVO, VERT_SERVO_CENTER);
	
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