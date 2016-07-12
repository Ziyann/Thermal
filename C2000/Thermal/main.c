#include "f2802x_headers/include/F2802x_Device.h"

#include "f2802x_common/include/f2802x_examples.h"

#include "f2802x_common/include/adc.h"
#include "f2802x_common/include/clk.h"
#include "f2802x_common/include/cpu.h"
#include "f2802x_common/include/flash.h"
#include "f2802x_common/include/i2c.h"
#include "f2802x_common/include/pie.h"
#include "f2802x_common/include/pll.h"
#include "f2802x_common/include/wdog.h"

#include <string.h>
#include <stdlib.h>

#include "includes/control.h"
#include "includes/i2c.h"
#include "includes/led.h"
#include "includes/mlx90614.h"
#include "includes/osc_comp.h"
#include "includes/pwm.h"
#include "includes/sci.h"
#include "includes/pwm.h"
#include "includes/sci.h"

ADC_Handle myAdc;
CLK_Handle myClk;

void setup_handles() {
	myAdc = ADC_init((void *) ADC_BASE_ADDR, sizeof(ADC_Obj));
	myClk = CLK_init((void *) CLK_BASE_ADDR, sizeof(CLK_Obj));
}

void init_system() {
	CPU_Handle myCpu = CPU_init((void *) NULL, sizeof(CPU_Obj));
	FLASH_Handle myFlash = FLASH_init((void *) FLASH_BASE_ADDR,
			sizeof(FLASH_Obj));
	PIE_Handle myPie = PIE_init((void *) PIE_BASE_ADDR, sizeof(PIE_Obj));
	PLL_Handle myPll = PLL_init((void *) PLL_BASE_ADDR, sizeof(PLL_Obj));
	WDOG_Handle myWDog = WDOG_init((void *) WDOG_BASE_ADDR, sizeof(WDOG_Obj));

	// running from flash - copy RAM based functions to RAM
	memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t) &RamfuncsLoadSize);

	// disable watchdog
	WDOG_disable(myWDog);

	// load factory calibration
	CLK_enableAdcClock(myClk);
	(*Device_cal )();
	CLK_disableAdcClock(myClk);

	// select the internal oscillator 1 (10 MHz) as the clock source
	CLK_setOscSrc(myClk, CLK_OscSrc_Internal);

	// setup the PLL for 10 MHz * 12 / 2 = 60 MHz
	PLL_setup(myPll, PLL_Multiplier_12, PLL_DivideSelect_ClkIn_by_2);

	// Set the flash OTP wait-states to minimum.
	FLASH_setup(myFlash);

	// disable PIE and all interrupts
	PIE_disable(myPie);
	PIE_disableAllInts(myPie);
	CPU_disableGlobalInts(myCpu);
	CPU_clearIntFlags(myCpu);
}

void rx_callback(char c) {
	static uint16_t cnt = 0;
	static char cmd[16];

	cmd[cnt] = c;

#if 0
	static int pwm_val = 3750;
	PWM_Handle myPwm3 = PWM_init((void *) PWM_ePWM3_BASE_ADDR, sizeof(PWM_Obj));
	if (c == '1')
		pwm_val += 50;
	else if (c == '2')
		pwm_val -= 50;

	PWM_setCmpA(myPwm3, pwm_val);
#endif

	if (c == '\n') {
		parse_command(cmd);
		cnt = 0;
		memset(cmd, 0x0, 16);
	} else {
		if (cnt < 16)
			++cnt;
	}
}

void main() {
	setup_handles();
	init_system();

	// turn off on-board LEDs to save power
	leds_set_all(0);

	// initialize oscillator compensation
	// using on-board temp sensor readings (ADC)
	// at 0.5 Hz, using CPU Timer 0
	osc_comp_init();

	// initialize SCI-A
	// 115200/8/1/N, muxes to on-board FTDI (GPIO28/29)
	sci_init();

	// initialize I2C-A
	// muxes SDA and SCL to GPIO32/33, 40 kHz clock
	i2c_init();

	// initialize ePWM3
	// sets up 50 Hz / 1.5 ms PWM signals to GPIO4 and GPIO5
	pwm_init();

	// set SCI RX callback
	sci_set_callback(rx_callback);

	SCI_putStringBlocking("Ready\r\n");

	while (1)
		;
}
