#include <xc.h>
#include <pic16f15224.h>
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 32000000

#include "motor_control.h"
#include "pwm_funcs.h"

/*
    AP = C4
    AN = C3
    BP = C2
    BN = C1

    Forward: C2 & C3
    Reverse: C4 & C1
    Brake: C3 & C1
    Coast: all open

*/

#define NMOS1 RC1    // C1
#define NMOS2 RC3    // C3

#define FORWARD 1
#define REVERSE 2
#define COAST   3
#define BRAKE   4
uint8_t state = 0;


void motor_setup(void)
{
    RC1 = 0;
    RC2 = 0;
    RC3 = 0;
    RC4 = 0;
    RC1PPS = 0x00;  // C1 source is Latch C1
    RC2PPS = 0x00;  // C2 source is Latch C2
    RC3PPS = 0x00;  // C3 source is Latch C3
    RC4PPS = 0x00;  // C4 source is Latch C4
    TRISC1 = 0;     // C1 is set to output
    TRISC2 = 0;     // C2 is set to output
    TRISC3 = 0;     // C3 is set to output
    TRISC4 = 0;     // C4 is set to output
    motor_ground_cutoff();
    setup_timer();
    setup_pwm(1);
    setup_pwm(2);
    motor_coast();
}

void motor_forward(uint8_t speed)
{
    if (state == FORWARD)
    {
        set_pwm_duty_cycle(1, speed);
    }
    else
    {
        motor_ground_cutoff();
        pwm_off(1);
        pwm_off(2);
        
        __delay_us(100);    // Delay to ensure all the transistors are off

        pwm_on(1, speed);
        RC3 = 0;
        state = FORWARD;
    }
}

void motor_reverse(uint8_t speed)
{
    if (state == REVERSE)
    {
        set_pwm_duty_cycle(2, speed);
    }
    else
    {
        motor_ground_cutoff();
        pwm_off(1);
        pwm_off(2);

        __delay_us(100);    // Delay to ensure all the transistors are off

        pwm_on(2, speed);
        RC1 = 0;
        state = REVERSE;
    }
}

void motor_coast(void)
{
    pwm_off(1);
    pwm_off(2);
    motor_ground_cutoff();
    state = COAST;
}

void motor_brake(void)
{
    // Need to find out if brake force goes up with more voltage
    pwm_off(1);
    pwm_off(2);
    RC1 = 1;
    RC3 = 1;
    state = BRAKE;
}

void motor_ground_cutoff(void)
{
    RC1 = 0;
    RC3 = 0;
}