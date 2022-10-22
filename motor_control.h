/* 
 * File:   motor_control.h
 * Author: andre
 *
 * Created on October 21, 2022, 1:14 PM
 */

#ifndef MOTOR_CONTROL_H
#define	MOTOR_CONTROL_H

void motor_setup(void);
void motor_forward(uint8_t speed);
void motor_reverse(uint8_t speed);
void motor_coast(void); // All transistors off
void motor_brake(void); // PMOS on, NMOS off
void motor_set_speed(uint8_t speed);

void motor_ground_cutoff(void);

#endif	/* MOTOR_CONTROL_H */

