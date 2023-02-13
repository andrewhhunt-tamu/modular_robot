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
    RC1PPS = 0x00;  // C1 source is Latch C1
    RC2PPS = 0x00;  // C2 source is Latch C2
    TRISC1 = 0;     // C1 is set to output
    TRISC2 = 0;     // C2 is set to output
    motor_ground_cutoff();
    setup_timer();
    setup_pwm(1);
    setup_pwm(2);
    motor_coast();

    reset_status();
}

void motor_forward(uint8_t speed)
{
    if (current_state == FORWARD)
    {
        set_pwm_duty_cycle(1, speed);
    }
    else
    {
        //motor_ground_cutoff();
        pwm_off(1);
        pwm_off(2);
        
        __delay_us(20);    // Delay to ensure all the transistors are off

        pwm_on(1, speed);
        current_state = FORWARD;
    }

    current_speed = speed;
}

void motor_reverse(uint8_t speed)
{
    if (current_state == REVERSE)
    {
        set_pwm_duty_cycle(2, speed);
    }
    else
    {
        //motor_ground_cutoff();
        pwm_off(1);
        pwm_off(2);

        __delay_us(20);    // Delay to ensure all the transistors are off

        pwm_on(2, speed);
        current_state = REVERSE;
    }
    
    current_speed = speed;
}

void motor_coast(void)
{
    pwm_off(1);
    pwm_off(2);
    //motor_ground_cutoff();
    current_state = COAST;
    current_speed = 1;
    // Setting speed to 1, because the UART doesn't seem to like 0s
}

void motor_brake(void)
{
    // Need to find out if brake force goes up with more voltage
    pwm_off(1);
    pwm_off(2);
    current_state = BRAKE;
    current_speed = 0;
}

void motor_ground_cutoff(void)
{
    RC1 = 0;
    RC3 = 0;
}

    /*
    * 4 fields for return_data:
    * 1. this address
    * 2. check value
    * 3. current direction
    * 4. current speed
    */
void send_error(uint8_t error_no)
{
    // Need to set up error types
    uint8_t return_data[4];
    return_data[0] = uart_get_address();
    return_data[1] = error_no;

    uart_send_frame(PI_ADDRESS, return_data, 2);
}

void send_status(void)
{
    uint8_t return_data[4];

    return_data[0] = uart_get_address();
    return_data[1] = check;
    return_data[2] = current_state;
    return_data[3] = current_speed;

    uart_send_frame(PI_ADDRESS, return_data, 4);
}

void reset_status(void)
{
    new_state = 150;
    new_speed = 150;
    check = 0;
    update = 0;
    eof_recv = 1;
    uart_reset();
}

void motor_receive_uart(void)
{
    /*
        Receive entire frame including EOF before sending
        send during EOF check
        Need error/state variable maybe
    */
   // This should be a state machine

    uint8_t message = uart_receive();

    if (message == ADDRESS_GOOD)
    {
        if (update == 1 || eof_recv == 0) // never received end of frame
        {
            reset_status();
            send_error(MISSING_EOF);
        }
        else 
        {
            update = 1;      // Address good, updating state
            eof_recv = 0;
        }
        
    }
    else if (message == ADDRESS_BAD)
    {
        // The frame is not addressed to this MCU, do nothing
        // If this is seen before EOF, send an error
        
        if (update == 1 || eof_recv == 0)
        {
            // New frame but never saw EOF from last frame
            reset_status();
            send_error(BAD_DATA);
        }
    }
    else if (message == ERROR)
    {
        // An error has occurred, ask for a new message
        reset_status();
        send_error(UART_ERROR);
    }
    else if (message == END_OF_FRAME)
    {
        if (update == 1) // Update never finished
        {
            send_error(MISSING_DATA);
        } 
        else // Update finished, send status
        {
            if (new_state == MOTOR_STATUS)
            {
                send_status();
            }
            else if (new_state == FORWARD)
            {
                // Set to the forward state with a valid speed
                motor_forward(new_speed);
                send_status();
            }
            else if (new_state == REVERSE)
            {
                // Set to the reverse state with a valid speed
                motor_reverse(new_speed);
                send_status();
            }
            else if (new_state == COAST)
            {
                // Turn all the motors off
                motor_coast();
                send_status();
            }
            else
            {
                // Something went wrong and the message makes no sense, request a new frame
                send_error(BAD_DATA);
            }
        }
        reset_status(); // Reset variables
    }
    else if (update == 1)
    {
        if (new_state == 150)
        {
            // First byte is the new state, will be checked at the end
            new_state = message;
        }
        else if (new_speed == 150)
        {
            // state already set, check and set speed
            if ((message < 100) && (message > 0))
            {
                new_speed = message;
            }
            else 
            {
                // Speed is bad, reset and inform the controller
                reset_status();
                send_error(BAD_DATA);
            }
        }
        else
        {
            check = new_state ^ new_speed;

            if (check == message)
            {
                update = 0;
            }
            else
            {
                // Check sent doesn't match, reset and inform the controller
                reset_status();
                send_error(BAD_DATA);
            }
        }
    }
    
}
