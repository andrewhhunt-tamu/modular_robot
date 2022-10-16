#include <xc.h>
#include <pic16f15224.h>
#include <stdio.h>
#include <stdlib.h>

#include "pwm_funcs.h"

#define PWMPORT 0b00000010;

void setup_pwm(int percent) {
    RC1PPS = 0x03; // Set PortC1 to be PWM3

    // page 215 setup PWM
    // PWM set up
    pwm_off();
    PWM3CON = 0x00; // Clear the PWM3CON register
    T2PR = 0xFF; // Load T2PR register with period of 255
    set_pwm_duty_cycle(percent); // Set PWM duty cycle to the requested percentage
    // Timer2 setup
    PIR1 = PIR1 & ~0b01000000; // Clear TMR2IF flag
    T2CLKCON = 0b00000001; // Set clock source to Fosc/4
    T2CON = 0b10000000; // Set Timer2 to be on with no prescaler or postscaler
    while((PIR1 & 0b01000000) != 0b01000000) {} // Wait until TMR2IF is set
    pwm_on();
    PWM3CON = 0b10110000; // Enable PWM3 with polarity inverse
}

void set_pwm_duty_cycle(int percent) {
    uint32_t duty_cycle = (uint32_t) percent * 1024 / 100; // Get percentage of 1024
    duty_cycle = 1024 - duty_cycle; // get inverse of percentage for PWM3DC
    PWM3DC = (uint16_t) duty_cycle  << 6; // Shift left 6 bits to set PWM3DC
}

void pwm_on() {
    TRISC = TRISC & ~PWMPORT; // Set PORTC1 to output
}

void pwm_off() {
    TRISC = TRISC | PWMPORT; // Set PORTC1 to not be an output
}

