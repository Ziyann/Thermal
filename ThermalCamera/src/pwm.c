#include "pwm.h"
#include <avr/io.h>

#include "control.h"

void pwm_setup() {
	DDRB |= (1 << PB1) | (1 << PB2); // set PB1 and PB2 as output pin
	
	// Inverted mode (LOW at bottom, HIGH on Match)
	// Clock / 8 prescaling
	// Fast PWM
	TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

	ICR1 = 19999; // 50 Hz frequency
}

void pwm_set_pulse_duration(uint8_t pin, uint16_t duration) {
	switch (pin) {
		case PIN_HORIZ_SERVO:
		OCR1A = ICR1 - duration;
		break;
		case PIN_VERT_SERVO:
		OCR1B = ICR1 - duration;
		break;
	}
}