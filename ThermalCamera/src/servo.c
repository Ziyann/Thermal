#include "servo.h"
#include <avr/io.h>

void set_servo_pos(int pos) {
	OCR1A = ICR1 - pos;
}