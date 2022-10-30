/* 
 * File:   uart.h
 * Author: andre
 *
 * Created on October 16, 2022, 5:38 PM
 */

#ifndef UART_H
#define	UART_H

void setup_uart(uint8_t address);
uint8_t uart_receive(void);
void uart_send(uint8_t data);



#endif	/* UART_H */