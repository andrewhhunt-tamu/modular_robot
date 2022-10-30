#include <xc.h>
#include <pic16f15224.h>
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 32000000

#include "uart.h"

/*
    SP1BRG baud rate generator = 25

    Page 231 for baud rate generator values

*/

uint8_t mcu_address, recv_address, message, addr_good = 0;


void setup_uart(uint8_t address)
{
    mcu_address = address;

    RC1IE = 1;  // Enable UART recieve interrupt
    //TX1IE = 1;
    

    // 115200 baud rate
    SP1BRG = 68;    // Set baud rate generator for 115200
    SYNC = 0;       // Asynchronous
    SPEN = 1;       // Serial port enable
    BRGH = 1;       // Set high baud rate
    BRG16 = 1;      // 16 bit baud rate generator

    // Transmit
    RA5PPS = 0x05;  // Transmit pin
    TX9 = 0;        // 9 bit transmission
    SCKP = 0;       // Set clock polarity
    TXEN = 1;       // Enable transmission

    // Receive
    RX1PPSbits.PORT = 0;    // Port A
    RX1PPSbits.PIN = 4;     // Pin 4
    //RX1PPS = RA4;                       // Receive pin
    ANSELA = 0;//ANSELA & ~(0b00010000);    // Clear ansel for A4
    RX9 = 0;                            // Enable receiving 9 bits
    ADDEN = 0;                          // Enable Address 
    CREN = 1;                           // Enable continuous receive
}

uint8_t uart_receive(void)
{
    message = RC1REG;

    if ((FERR == 0) & (OERR == 0))  // No frame error or overrun
    {    
        if (((message & 128) >> 7) == 1)
        {
            recv_address = message & 127;   // remove msb

            if (mcu_address == recv_address)
            {
                addr_good = 1;  // Address is for this MCU
                return 129;     // Address received
            }
            else
            {
                return 128;     // Not for this MCU, ignore
            }
        }
        else if (addr_good)
        {
            if (message == 126)
            {
                addr_good = 0;  // Message end, stop reading messages
                return 130;     // End token received
            }
            else
            {
                return message; // Send the received message to the requester
            }
        }

        return 128; // Ignore
    }
    else
    {
        return 131; // Frame error or overrun, message not good
    }
}

void uart_send(uint16_t data)
{
    //TX9D = data >> 8;       // Send 9th bit
    TX1REG = data & 0xff;   // Send data byte
}