/*
 * servo.c
 *
 *  Created on: Jul 9, 2016
 *      Author: Twi
 */

#include "f2802x_headers/include/F2802x_Device.h"

#include "f2802x_common/include/pwm.h"

#include "includes/sci.h"

#define HORIZ_SERVO 1
#define VERT_SERVO	2

#define SERVO_CENTER	3750
#define SERVO_MAX_CH	1250

void servo_set_angle(int servo_num, int angle) {
	PWM_Handle myPwm3 = PWM_init((void *) PWM_ePWM3_BASE_ADDR, sizeof(PWM_Obj));
	float tenth_deg_ch = (float) 1250 / 450;
	int cmp_val = SERVO_CENTER;

	// safety
	if (angle > 450 || angle < -450)
		return;

	cmp_val += (float) angle * tenth_deg_ch;

	if (servo_num == HORIZ_SERVO)
		PWM_setCmpA(myPwm3, cmp_val);
	else if (servo_num == VERT_SERVO)
		PWM_setCmpB(myPwm3, cmp_val);
}

void servos_reset() {
	servo_set_angle(HORIZ_SERVO, 0);
	servo_set_angle(HORIZ_SERVO, 0);
}
