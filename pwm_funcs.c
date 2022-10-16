#include <xc.h>
#include <pic16f15224.h>
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 32000000

#include "pwm_funcs.h"

#define PWMPORT1 0b00000010;
#define PWMPORT2 0b00000100;

void setup_pwm(uint8_t channel)
{
    // page 215 setup PWM
    // PWM set up
    pwm_off(channel);
    if (channel == 1)
    {
        RC1PPS = 0x03; // Set PortC1 to be PWM3
        PWM3CON = 0x00; // Clear the PWM3CON register
    } else if (channel == 2)
    {
        RC2PPS = 0x04; // Set PortC2 to be PWM4
        PWM4CON = 0x00;
    }
    
    T2PR = 0xFF; // Load T2PR register with period of 255
    set_pwm_duty_cycle(channel, 0); // Set PWM duty cycle to 0;
    // Timer2 setup
    PIR1 = PIR1 & ~0b01000000; // Clear TMR2IF flag
    T2CLKCON = 0b00000001; // Set clock source to Fosc/4
    T2CON = 0b10000000; // Set Timer2 to be on with no prescaler or postscaler
    while((PIR1 & 0b01000000) != 0b01000000) {} // Wait until TMR2IF is set
    
    if (channel == 1)
    {
        PWM3CON = 0b10110000; // Enable PWM3 with polarity inverse
    } else if (channel == 2)
    {
        PWM4CON = 0b10110000; // Enable PWM3 with polarity inverse
    }
    
}

void set_pwm_duty_cycle(uint8_t channel, uint8_t percent)
{
    uint32_t duty_cycle = (uint32_t) percent * 1024 / 100; // Get percentage of 1024
    duty_cycle = 1024 - duty_cycle; // get inverse of percentage for PWM3DC
    if (channel == 1)
    {
        PWM3DC = (uint16_t) duty_cycle  << 6; // Shift left 6 bits to set PWM3DC
    } else if (channel == 2) 
    {
        PWM4DC = (uint16_t) duty_cycle  << 6; // Shift left 6 bits to set PWM4DC
    }
}

void pwm_on(uint8_t channel)
{
    if (channel == 1)
    {
        pwm_off(2); // Make sure channel 2 is off
        __delay_ms(5);  // Ensure transistors are fully off
        TRISC = TRISC & ~PWMPORT1; // Set PORTC1 to output
    } else if (channel == 2)
    {
        pwm_off(1); // Make sure channel 1 is off
        __delay_ms(5); // Ensure transistors are fully off
        TRISC = TRISC & ~PWMPORT2; // Set PORTC1 to output
    }
    
}

void pwm_off(uint8_t channel)
{
    if (channel == 1)
    {
        TRISC = TRISC | PWMPORT1; // Set PORTC1 to not be an output
    } else if (channel == 2) {
        TRISC = TRISC | PWMPORT2; // Set PORTC1 to not be an output
    }
}

