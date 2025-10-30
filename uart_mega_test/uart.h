/*
 * uart library
 * currently specifies UART0
 *
 */
#ifndef _UART_H_
#define _UART_H_

#include <avr/io.h>
#include "uart.h"

void uart_init(unsigned long baud);
unsigned char uart_tx(unsigned char);
unsigned char uart_rx(void);

inline void uart_enable_rxint (void) {
  UCSR0B |= (1<<RXCIE0);
}

inline void uart_disable_rxint (void) {
  UCSR0B &= ~(1<<RXCIE0);
}

inline void uart_enable_txint (void) {
  UCSR0B |= (1<<TXCIE0);
}

inline void uart_disable_txint (void) {
  UCSR0B &= ~(1<<TXCIE0);
}

void uart_putchar(unsigned char c);
void uart_putstr(char * str);
int uart_getchar(void);
int uart_peek(void);
void uart_puthex(unsigned char c);
void uart_putnum_u16(unsigned short n, int digit);
void uart_puts(char *s);
void uart_putsln(char *s);

#endif /* _UART_H_ */

