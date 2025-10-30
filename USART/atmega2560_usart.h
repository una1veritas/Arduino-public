#ifndef ATMEGA2560_USART_H
#define ATMEGA2560_USART_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/* Configure these as needed */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define UBRR_VALUE(b) ((F_CPU / (16UL * b)) - 1UL)

/* API (USART0 primary console). */
void usart0_init(uint16_t ubrr);
void usart0_tx(uint8_t b);
bool usart0_tx_ready(void);
uint8_t usart0_rx_blocking(void);
int usart0_getchar_nb(uint8_t *out); /* returns 1 if byte available, 0 otherwise */
void usart0_send_string(const char *s);

/* Hook to allow printf to use USART0 */
int usart0_putchar_printf(char c, FILE *stream);

/* Interrupt-driven RX buffer */
void usart0_rx_enable_interrupt(void);
void usart0_rx_disable_interrupt(void);
uint8_t usart0_rx_available(void);

 /* Simple USART1 init/send example (polling) */
void usart1_init(uint16_t ubrr);
void usart1_tx(uint8_t b);

#endif /* ATMEGA2560_USART_H */
