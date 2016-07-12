/*
 * osc_comp.c
 *
 *  Created on: Jul 1, 2016
 *      Author: Twi
 */

#include "f2802x_headers/include/F2802x_Device.h"

#include "f2802x_common/include/adc.h"
#include "f2802x_common/include/clk.h"
#include "f2802x_common/include/osc.h"
#include "f2802x_common/include/pie.h"
#include "f2802x_common/include/timer.h"

#include <stdlib.h>

static ADC_Handle myAdc;
static OSC_Handle myOsc;
static PIE_Handle myPie;

__interrupt void adc_isr();

static void timer0_init() {
	TIMER_Handle myTimer0 = TIMER_init((void *) TIMER0_BASE_ADDR,
			sizeof(TIMER_Obj));

	TIMER_stop(myTimer0);

	// SYSCLKOUT = 60 MHz
	// 2 seconds period
	TIMER_setPeriod(myTimer0, 120 * 1000000);
	TIMER_setPreScaler(myTimer0, 0);

	// reload period and prescaler counters
	TIMER_reload(myTimer0);

	TIMER_setEmulationMode(myTimer0,
			TIMER_EmulationMode_StopAfterNextDecrement);

	TIMER_start(myTimer0);
}

void osc_comp_init() {
	CLK_Handle myClk = CLK_init((void *) CLK_BASE_ADDR, sizeof(CLK_Obj));
	CPU_Handle myCpu = CPU_init((void *) NULL, sizeof(CPU_Obj));
	myAdc = ADC_init((void *) ADC_BASE_ADDR, sizeof(ADC_Obj));
	myOsc = OSC_init((void *) OSC_BASE_ADDR, sizeof(OSC_Obj));
	myPie = PIE_init((void *) PIE_BASE_ADDR, sizeof(PIE_Obj));

	CLK_enableAdcClock(myClk);

	ADC_enableBandGap(myAdc);
	ADC_enableRefBuffers(myAdc);
	ADC_powerUp(myAdc);
	ADC_enable(myAdc);
	ADC_setVoltRefSrc(myAdc, ADC_VoltageRefSrc_Int);

	ADC_enableTempSensor(myAdc);

	ADC_setIntPulseGenMode(myAdc, ADC_IntPulseGenMode_Prior);
	ADC_setIntMode(myAdc, ADC_IntNumber_1, ADC_IntMode_ClearFlag);
	ADC_setIntSrc(myAdc, ADC_IntNumber_1, ADC_IntSrc_EOC1);
	ADC_enableInt(myAdc, ADC_IntNumber_1);

	ADC_setSocChanNumber(myAdc, ADC_SocNumber_0, ADC_SocChanNumber_A5);
	ADC_setSocChanNumber(myAdc, ADC_SocNumber_1, ADC_SocChanNumber_A5);
	ADC_setSocTrigSrc(myAdc, ADC_SocNumber_0, ADC_SocTrigSrc_CpuTimer_0);
	ADC_setSocTrigSrc(myAdc, ADC_SocNumber_1, ADC_SocTrigSrc_CpuTimer_0);
	ADC_setSocSampleWindow(myAdc, ADC_SocNumber_0,
			ADC_SocSampleWindow_37_cycles);
	ADC_setSocSampleWindow(myAdc, ADC_SocNumber_1,
			ADC_SocSampleWindow_37_cycles);

	// initialize CPU Timer 0
	// 2 seconds period
	timer0_init();

	// enable interrupts
	PIE_enable(myPie);
	CPU_enableGlobalInts(myCpu);

	// setup ADC interrupt
	PIE_registerPieIntHandler(myPie, PIE_GroupNumber_10, PIE_SubGroupNumber_1,
			(intVec_t) & adc_isr);

	PIE_enableAdcInt(myPie, ADC_IntNumber_1);
	CPU_enableInt(myCpu, CPU_IntNumber_10);
}

// every 2 seconds
__interrupt void adc_isr() {
	uint16_t temp = ADC_readResult(myAdc, ADC_ResultNumber_1);

	OSC_runCompensation(myOsc, OSC_Number_1, temp);
	OSC_runCompensation(myOsc, OSC_Number_2, temp);

	ADC_clearIntFlag(myAdc, ADC_IntNumber_1);
	PIE_clearInt(myPie, PIE_GroupNumber_10);
}
