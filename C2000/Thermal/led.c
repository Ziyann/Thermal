/*
 * led.c
 *
 *  Created on: Jul 8, 2016
 *      Author: Twi
 */

#include "f2802x_headers/include/F2802x_Device.h"
#include "f2802x_common/include/gpio.h"

void led_set(int num, int enable) {
	GPIO_Handle myGpio = GPIO_init((void *) GPIO_BASE_ADDR, sizeof(GPIO_Obj));

	GPIO_setMode(myGpio, (GPIO_Number_e) num, (GPIO_Mode_e) 0); // general purpose
	GPIO_setDirection(myGpio, (GPIO_Number_e) num, GPIO_Direction_Output);

	if (enable)
		GPIO_setLow(myGpio, (GPIO_Number_e) num);
	else
		GPIO_setHigh(myGpio, (GPIO_Number_e) num);
}

void leds_set_all(int enable) {
	led_set(0, enable);
	led_set(1, enable);
	led_set(2, enable);
	led_set(3, enable);
}
