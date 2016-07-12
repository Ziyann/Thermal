/*
 * pwm_init.c
 *
 *  Created on: Jul 1, 2016
 *      Author: Twi
 */

#include "f2802x_headers/include/F2802x_Device.h"

#include "f2802x_common/include/clk.h"
#include "f2802x_common/include/gpio.h"
#include "f2802x_common/include/pwm.h"

static void init_gpio_pwm() {
	GPIO_Handle myGpio = GPIO_init((void *) GPIO_BASE_ADDR, sizeof(GPIO_Obj));

	GPIO_setPullUp(myGpio, GPIO_Number_4, GPIO_PullUp_Disable);
	GPIO_setPullUp(myGpio, GPIO_Number_5, GPIO_PullUp_Disable);
	GPIO_setMode(myGpio, GPIO_Number_4, GPIO_4_Mode_EPWM3A);
	GPIO_setMode(myGpio, GPIO_Number_5, GPIO_5_Mode_EPWM3B);
}

void pwm_init() {
	CLK_Handle myClk = CLK_init((void *) CLK_BASE_ADDR, sizeof(CLK_Obj));
	PWM_Handle myPwm3 = PWM_init((void *) PWM_ePWM3_BASE_ADDR, sizeof(PWM_Obj));

	init_gpio_pwm();

	CLK_enablePwmClock(myClk, PWM_Number_3);

	// Up-count mode
	PWM_setCounterMode(myPwm3, PWM_CounterMode_Up);

	// Time-Base Period
	// T_PWM = (TBPRD + 1) * T_TBLCK
	PWM_setPeriod(myPwm3, 50000 - 1);

	PWM_disableCounterLoad(myPwm3);	// Disable phase loading
	PWM_setPhase(myPwm3, 0x0000);	// Phase is 0
	PWM_setCount(myPwm3, 0x0000);	// Clear counter

	// TBCLK = SYSCLKOUT / (HSPCLKDIV × CLKDIV)
	// 2.5 MHz
	PWM_setHighSpeedClkDiv(myPwm3, PWM_HspClkDiv_by_12);
	PWM_setClkDiv(myPwm3, PWM_ClkDiv_by_2);

	// Setup shadow register load on ZERO
	PWM_setShadowMode_CmpA(myPwm3, PWM_ShadowMode_Shadow);
	PWM_setShadowMode_CmpB(myPwm3, PWM_ShadowMode_Shadow);
	PWM_setLoadMode_CmpA(myPwm3, PWM_LoadMode_Zero);
	PWM_setLoadMode_CmpB(myPwm3, PWM_LoadMode_Zero);

	// default to 1.5ms pulse width
	PWM_setCmpA(myPwm3, 3750);
	PWM_setCmpB(myPwm3, 3750);

	PWM_setActionQual_Zero_PwmA(myPwm3, PWM_ActionQual_Set);
	PWM_setActionQual_Zero_PwmB(myPwm3, PWM_ActionQual_Set);

	PWM_setActionQual_CntUp_CmpA_PwmA(myPwm3, PWM_ActionQual_Clear);
	PWM_setActionQual_CntUp_CmpB_PwmB(myPwm3, PWM_ActionQual_Clear);

	CLK_enableTbClockSync(myClk);
}
