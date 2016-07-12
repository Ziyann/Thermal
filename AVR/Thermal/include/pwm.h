#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

void pwm_setup(void);
void pwm_set_pulse_duration(uint8_t pin, uint16_t duration);

#endif /* PWM_H_ */