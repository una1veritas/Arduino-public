#ifndef ATMEGA2560_USART_H
#define ATMEGA2560_USART_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/* Configure these as needed */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define U2Xn 1UL

#define UBRR_VALUE(baud) ((F_CPU / ((16UL >> U2Xn) * baud)) - 1UL)

/* API (USART0 primary console). */
void usart0_init(uint16_t ubrr);
void usart0_tx(uint8_t b);
bool usart0_tx_ready(void);
uint8_t usart0_rx(void);
uint8_t usart0_rx_buffered(uint8_t *out); /* returns 1 if byte available, 0 otherwise */

/* Hook to allow printf to use USART0 */
int usart0_putchar_printf(char c, FILE *stream);

/* Interrupt-driven RX buffer */
void usart0_rx_interrupt_enable(void);
void usart0_rx_interrupt_disable(void);
uint8_t usart0_rx_available(void);

 /* USART1 init/send */
void usart1_rx_interrupt_enable(void);
void usart1_rx_interrupt_disable(void);
uint8_t usart1_rx_available(void);
void usart1_init(uint16_t ubrr);
void usart1_tx(uint8_t b);
bool usart1_tx_ready(void);
int usart1_putchar_printf(char c, FILE *stream);
uint8_t usart1_tx_buffered(uint8_t c);
uint8_t usart0_rx_available(void);

#endif /* ATMEGA2560_USART_H */
