/*
 * sci.c
 *
 *  Created on: Jul 1, 2016
 *      Author: Twi
 */

#include "f2802x_headers/include/F2802x_Device.h"

#include "f2802x_common/include/clk.h"
#include "f2802x_common/include/gpio.h"
#include "f2802x_common/include/pie.h"
#include "f2802x_common/include/sci.h"

#include <stdlib.h>

static PIE_Handle myPie;
static SCI_Handle mySci;

__interrupt void sciaRxIsr();

static void (*rx_callback)(char c);

static void sci_init_gpio() {
	GPIO_Handle myGpio = GPIO_init((void *) GPIO_BASE_ADDR, sizeof(GPIO_Obj));

	// set SCI pull-ups
	GPIO_setPullUp(myGpio, GPIO_Number_28, GPIO_PullUp_Enable);
	GPIO_setPullUp(myGpio, GPIO_Number_29, GPIO_PullUp_Disable);

	// use asynchronous qualification for the SCI input signal
	GPIO_setQualification(myGpio, GPIO_Number_28, GPIO_Qual_ASync);

	// mux gpio28/29 to SCI-A
	GPIO_setMode(myGpio, GPIO_Number_28, GPIO_28_Mode_SCIRXDA);
	GPIO_setMode(myGpio, GPIO_Number_29, GPIO_29_Mode_SCITXDA);
}

// initialize SCI-A
static void scia_init() {
	CLK_Handle myClk = CLK_init((void *) CLK_BASE_ADDR, sizeof(CLK_Obj));

	// enable the SCI-A clock
	CLK_enableSciaClock(myClk);

	// disable parity
	SCI_disableParity(mySci);

	// 1 stop bit
	SCI_setNumStopBits(mySci, SCI_NumStopBits_One);

	// 8 data bits
	SCI_setCharLength(mySci, SCI_CharLength_8_Bits);

	// enable RX interrupt
	SCI_enableRxInt(mySci);

	// 9600 baud rate
	SCI_setBaudRate(mySci, SCI_BaudRate_115_2_kBaud);

	// enable free run - continue SCI operation regardless of suspend
	SCI_setPriority(mySci, SCI_Priority_FreeRun);

	// enable TX and RX
	SCI_enableTx(mySci);
	SCI_enableRx(mySci);

	// enable the SCI interface
	SCI_enable(mySci);
}

void SCI_putStringBlocking(char *msg) {
	while (*msg != '\0') {
		SCI_putDataBlocking(mySci, *msg);
		++msg;
	}
}

void SCI_putByteBlocking(uint16_t data) {
	SCI_putDataBlocking(mySci, data);
}

void sci_set_callback(void (*callback)(char c)) {
	rx_callback = callback;
}

void sci_init() {
	CPU_Handle myCpu = CPU_init((void *) NULL, sizeof(CPU_Obj));
	myPie = PIE_init((void *) PIE_BASE_ADDR, sizeof(PIE_Obj));
	mySci = SCI_init((void *) SCIA_BASE_ADDR, sizeof(SCI_Obj));

	sci_init_gpio();

	scia_init();

	// enable interrupts
	PIE_enable(myPie);
	CPU_enableGlobalInts(myCpu);

	// Register interrupt handlers in the PIE vector table
	PIE_registerPieIntHandler(myPie, PIE_GroupNumber_9, PIE_SubGroupNumber_1,
			(intVec_t) & sciaRxIsr);

	// SCI-A RX interrupt
	PIE_enableInt(myPie, PIE_GroupNumber_9, PIE_InterruptSource_SCIARX);
	CPU_enableInt(myCpu, CPU_IntNumber_9);
}

__interrupt void sciaRxIsr(void) {
	char data = SCI_getData(mySci);

	if (rx_callback)
		rx_callback(data);
	else
		SCI_putByteBlocking(data);

	// clear PIE interrupt
	PIE_clearInt(myPie, PIE_GroupNumber_9);
}
