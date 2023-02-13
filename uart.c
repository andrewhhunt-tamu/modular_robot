#include <xc.h>
#include <pic16f15224.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define _XTAL_FREQ 32000000

#include "uart.h"

/*
    SP1BRG baud rate generator = 25

    Page 231 for baud rate generator values

*/

void setup_uart(uint8_t address)
{
    mcu_address = address;

    RC1IE = 1;  // Enable UART recieve interrupt

    // 115200 baud rate
    SP1BRG = 68;    // Set baud rate generator for 115200
    SYNC = 0;       // Asynchronous
    SPEN = 1;       // Serial port enable
    BRGH = 1;       // Set high baud rate
    BRG16 = 1;      // 16 bit baud rate generator

    // Transmit
    TRISA5 = 1;
    RA5PPS = 0x00;
    //RA5PPS = 0x05;  // Transmit pin
    TX9 = 0;        // 8 bit transmission
    SCKP = 0;       // Set clock polarity
    TXEN = 1;       // Enable transmission

    // Receive
    RX1PPSbits.PORT = 0;                // Receive on port A
    RX1PPSbits.PIN = 4;                 // Receive on pin 4
    ANSELA = ANSELA & ~(0b00010000);    // Clear ansel for A4
    RX9 = 0;                            // Disable receiving 9 bits
    ADDEN = 0;                          // Enable Address 
    CREN = 1;                           // Enable continuous receive
}

uint8_t uart_receive(void)
{
    // Maybe should set a timer here to reset the receiver in case
    // the end byte is corrupted or not received

    uint8_t message, recv_address;
    
    message = RC1REG;

    if ((FERR == 0) & (OERR == 0))  // No frame error or overrun
    {    
        if (((message & 128) >> 7) == 1)    // Check if this is an address byte
        {
            // Need to check if still waiting for previous end of frame

            recv_address = message & 127;   // remove msb to get address

            if (!eof_received)
            {
                return FRAME_ERROR;         // A new address has been sent but missing eof
            }
            else if (mcu_address == recv_address)
            {
                addr_good = 1;              // Address is for this MCU
                eof_received = 0;           // New frame, so no eof yet
                return ADDRESS_GOOD;        // Address received
            }
            else
            {
                addr_good = 0;              
                return ADDRESS_BAD;         // Not for this MCU, ignore
            }
        }
        else if (addr_good)
        {
            if (message == 126)
            {
                addr_good = 0;              // Message end check addr in further messages
                eof_received = 1;           // EOF has been received, safe to start new frames
                return END_OF_FRAME;        // End token received
            }
            else
            {
                return message;             // Send the received message to the requester
            }
        }

        addr_good = 0;                      
        return ADDRESS_BAD;                 // Ignore
    }
    else
    {
        addr_good = 0;
        return ERROR;                       // Frame error or overrun, message not good
    }
}

// Need to have transmit disabled until ready to send
void uart_send_byte(uint8_t data)
{
    TX1REG = data;   // Send data byte
}


void uart_send_frame(uint8_t address, const uint8_t * data, uint8_t length)
{
    GIE = 0;                    // Disable interrupts
    while(!TRMT) {  }           // Wait for the transmit register to empty
    RA5PPS = 0x05;              // Set port A5 to be TX
    TX1REG = 128 + address;     // Transmit the address
    while (length)
    {
        while(!TRMT) {  }       // Wait for the transmit register to empty
        TX1REG = *(data++);     // Transmit the next byte
        length--;
    }
    while(!TRMT) {  }           // Wait for the transmit register to empty
    TX1REG = END_TOKEN;         // Send the frame end token
    while(!TRMT) {  }           // Wait for the transmit register to empty
    RA5PPS = 0x00;              // Set port A5 back to high impedance
    GIE = 1;                    // Enable interrupts
}

uint8_t uart_get_address(void)
{
    return mcu_address;
}

void uart_reset(void)
{
    addr_good = 0;
    eof_received = 1;
    // clear out receive FIFO
    uint8_t msg;
    msg = RC1REG;
    msg = RC1REG;
    msg = RC1REG;
}