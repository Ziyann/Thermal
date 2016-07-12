#include "include/servo.h"
#include "include/pwm.h"
#include "include/control.h"
#include "config/conf_board.h"

#include <util/delay.h>

void set_servo_pos_relative_deg(uint8_t pin, int16_t deg) {
    uint16_t duration;
    float one_deg;

    // safety
    if (deg > 600 || deg < -600) {
        return;
    }

    if (pin == PIN_HORIZ_SERVO) {
        duration = HORIZ_SERVO_CENTER;
        if (deg > 0) {
            one_deg = (HORIZ_SERVO_LEFT_45D - HORIZ_SERVO_CENTER) / (float)450;
        } else {
            one_deg = (HORIZ_SERVO_RIGHT_45D - HORIZ_SERVO_CENTER) / (float)450;
        }
    } else {
        duration = VERT_SERVO_CENTER;
        if (deg > 0) {
            one_deg = (VERT_SERVO_UP_45D - VERT_SERVO_CENTER) / (float)450;
        } else {
            one_deg = (VERT_SERVO_DOWN_45D - VERT_SERVO_CENTER) / (float)450;
        }
    }

    if (deg > 0) {
        duration += (int)(deg * one_deg);
    } else {
        duration -= (int)(deg * one_deg);
    }

    pwm_set_pulse_duration(pin, duration);
}

void servo_stabilize(uint8_t pin) {
    set_servo_pos_relative_deg(pin, -10);
    _delay_ms(250);
    set_servo_pos_relative_deg(pin, 10);
    _delay_ms(250);
    set_servo_pos_relative_deg(pin, 0);
}