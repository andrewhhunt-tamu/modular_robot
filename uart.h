/* 
 * File:   uart.h
 * Author: andre
 *
 * Created on October 16, 2022, 5:38 PM
 */

#ifndef UART_H
#define	UART_H


#define _XTAL_FREQ 32000000
#define PI_ADDRESS 33

// Message returns
#define ADDRESS_GOOD    129
#define ADDRESS_BAD     128
#define END_OF_FRAME    130
#define ERROR           131
#define FRAME_ERROR     132

#define END_TOKEN       126

uint8_t mcu_address, addr_good = 0, eof_received = 1;

void setup_uart(uint8_t address);
uint8_t uart_receive(void);
void uart_send_byte(uint8_t data);
void uart_send_frame(uint8_t address, const uint8_t * data, uint8_t length);
uint8_t uart_get_address(void);
void uart_reset(void);


#endif	/* UART_H */