#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <conf_board.h>

#include <stdlib.h>

void uart_setup() {
	#define USART_BAUDRATE 9600
	#define BAUD_PRESCALE (((F_CPU/(USART_BAUDRATE*16UL)))-1)
	
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);   // Enable receiver and transmitter
	UCSR0B |= (1 << RXCIE0);                 // Enable receiever interrupt
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // 8 data bits, 1 stop bit
	sei(); // enable interrupts
	
	/* Set baud rate */
	UBRR0H  = (BAUD_PRESCALE >> 8);
	UBRR0L  = BAUD_PRESCALE;
}

void uart_enable_rx_pullup() {
	// set PD0 as input
	DDRD &= ~(1 << DDD0);

	// set PD0 to high
	PORTD |= (1 << PD0);
}

void uart_sendint(int32_t number) {
	char buffer[16];
	
	uart_sendstring(ltoa(number, buffer, 10));
	uart_sendchar('\n');
}

void uart_sendchar(char data) {
    // Wait until buffer is empty
    while ((UCSR0A & (1 << UDRE0)) == 0) {};

	UDR0 = data;
}

void uart_sendstring(char* StringPtr) {
	while (*StringPtr != 0x0) {
		uart_sendchar(*StringPtr);
		++StringPtr;
	}
}

char uart_getchar(void) {
	// Wait until data is received
	while ((UCSR0A & (1 << RXC0)) == 0) {};
	
	return UDR0;
}
