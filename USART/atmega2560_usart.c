#include "atmega2560_usart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>


#ifndef F_CPU
#error F_CPU is not defined.
#endif
/* -------------------- USART0 (console) -------------------- */

/* RX ring buffer */
#define RX_BUF_SIZE 128
static volatile uint8_t rx_buf[RX_BUF_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;


void usart0_init(uint32_t baud) {
    uint16_t ubrr =  (F_CPU / (16UL * baud)) - 1UL ;
    /* Set baud rate */
    UBRR0H = (uint8_t) (ubrr >> 8);
    UBRR0L = (uint8_t) ubrr;
    /* Frame format: 8 data, no parity, 1 stop bit */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    /* Enable TX and RX (RX interrupt left off until explicitly enabled) */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

/* Enable RX interrupt */
void usart0_rx_enable_interrupt(void) {
    UCSR0B |= (1 << RXCIE0);
}

/* Disable RX interrupt */
void usart0_rx_disable_interrupt(void) {
    UCSR0B &= ~(1 << RXCIE0);
}

/* Blocking TX */
void usart0_tx(uint8_t b) {
    while (!(UCSR0A & (1 << UDRE0))); /* wait until data register empty */
    UDR0 = b;
}

bool usart0_tx_ready(void) {
    return (UCSR0A & (1 << UDRE0)) != 0;
}

/* Non-blocking getchar (returns 1 if a byte was available) */
int usart0_getchar_nb(uint8_t *out) {
    if (rx_head == rx_tail) return 0; /* empty */
    uint8_t val = rx_buf[rx_tail];
    rx_tail = (uint8_t)((rx_tail + 1) & (RX_BUF_SIZE - 1));
    *out = val;
    return 1;
}

/* Blocking receive (wait for data) */
uint8_t usart0_rx_blocking(void) {
    uint8_t ch;
    while (!usart0_getchar_nb(&ch));
    return ch;
}

void usart0_send_string(const char *s) {
    while (*s) {
        usart0_tx((uint8_t)*s++);
    }
}

/* Hook for printf */
int usart0_putchar_printf(char c, FILE *stream) {
    (void)stream;
    if (c == '\n') usart0_tx('\r'); /* CRLF */
    usart0_tx((uint8_t)c);
    return 0;
}

/* RX ISR - push received byte into ring buffer */
ISR(USART0_RX_vect) {
    uint8_t data = UDR0;
    uint8_t next = (uint8_t)((rx_head + 1) & (RX_BUF_SIZE - 1));
    if (next == rx_tail) {
        /* Buffer overflow: drop old data (advancing tail) or drop new byte.
           Here we drop oldest byte to make space. */
        rx_tail = (uint8_t)((rx_tail + 1) & (RX_BUF_SIZE - 1));
    }
    rx_buf[rx_head] = data;
    rx_head = next;
}

uint8_t usart0_rx_available(void) {
    return (uint8_t)((RX_BUF_SIZE + rx_head - rx_tail) & (RX_BUF_SIZE - 1));
}

/* -------------------- USART1 (example second port, polled) -------------------- */

void usart1_init(uint32_t baud) {
    uint16_t ubrr = (uint16_t)((F_CPU / (16UL * baud)) - 1UL);
    UBRR1H = (uint8_t)(ubrr >> 8);
    UBRR1L = (uint8_t)ubrr;
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); /* 8N1 */
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);
}

void usart1_tx(uint8_t b) {
    while (!(UCSR1A & (1 << UDRE1)));
    UDR1 = b;
}
