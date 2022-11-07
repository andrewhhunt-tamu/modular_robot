#include <xc.h>
#include <pic16f15224.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "motor_control.h"
#include "pwm_funcs.h"
#include "uart.h"


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

    update = 0;
    new_state = 0;
    new_speed = 0;
    check = 0;
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

void motor_receive_uart(void)
{
    message = uart_receive();

    if (message == ADDRESS_GOOD)
    {
        update = 1;      // Address good, updating state
    }
    else if (message == ADDRESS_BAD)
    {
        // Do nothing
    }
    else if (message == ERROR)
    {
        // An error has occurred, ask for a new message
        uint8_t data[1];
        data[0] = ERROR;

        uart_send_frame(PI_ADDRESS, data, 1);
        update = 0;     // Message over
        new_state = 0;  // Reset new state variable
        new_speed = 0;
        check = 0;
    }
    else if (message == END_OF_FRAME)
    {
        update = 0;     // Message over
        new_state = 0;  // Reset new state variable
        new_speed = 0;
        check = 0;
    }
    else
    {
        if (new_state == 0)
        {
            new_state = message;
            // check message value and set the state

            // TESTING
            //uart_send(message);
        }
        else if (new_speed == 0)
        {
            // state already set, set speed
            new_speed = message;
            check = new_state ^ new_speed;

            // TESTING
            //uart_send(message ^ new_state);
        }
        else
        {
            if (check == message)
            {
                // Set the new state and speed

                // If the new state or speed do not make sense,
                // send an error back

                // Set the check byte back to the RPi so it knows
                // the message was received properly

                uint8_t return_data[1];
                return_data[0] = check;

                uart_send_frame(PI_ADDRESS, return_data, 1);
            }
            else
            {
                uint8_t return_data[1];
                return_data[0] = 0xEA;

                uart_send_frame(PI_ADDRESS, return_data, 1);
            }
        }
    }
    
}
