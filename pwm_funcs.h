/* 
 * File:   pwm_funcs.h
 * Author: andre
 *
 * Created on October 16, 2022, 12:42 AM
 */

#ifndef PWM_FUNCS_H
#define	PWM_FUNCS_H

void setup_pwm(uint8_t channel);
void set_pwm_duty_cycle(uint8_t channel, uint8_t percent);
void pwm_on(uint8_t channel);
void pwm_off(uint8_t channel);


#endif	/* PWM_FUNCS_H */

