/*
 * Z80Bus_SIO.cpp
 *
 *  Created on: 2025/11/01
 *      Author: sin
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>


#include "Z80Bus_SIO.h"

#ifndef F_CPU
#error F_CPU is not defined.
#endif

/* RX ring buffer */
#define RX_BUF_SIZE 		(1<<7)
#define RX_BUF_SIZE_MASK 	(RX_BUF_SIZE - 1)
	/* TX1 ring buffer */
#define TX_BUF_SIZE 		(1<<6)
#define TX_BUF_SIZE_MASK 	(TX_BUF_SIZE - 1)

	/* TX1 ring buffer */
static volatile uint8_t rx1_buf[RX_BUF_SIZE];
static volatile uint8_t rx1_head;
static volatile uint8_t rx1_tail;
	/* TX1 ring buffer */
static volatile uint8_t tx1_buf[TX_BUF_SIZE];
static volatile uint8_t tx1_head; /* index where next byte will be written */
static volatile uint8_t tx1_tail; /* index of next byte to send */

/* RX ISR - push received byte into ring buffer */
ISR(USART1_RX_vect) {
	uint8_t data = UDR1;
	uint8_t next = (uint8_t)((rx1_head + 1) & RX_BUF_SIZE_MASK);
	if (next == rx1_tail) {
		/* Buffer overflow: drop old data (advancing tail) or drop new byte.
		   Here we drop oldest byte to make space. */
		rx1_tail = (uint8_t)((rx1_tail + 1) & RX_BUF_SIZE_MASK);
	}
	rx1_buf[rx1_head] = data;
	rx1_head = next;
}

/*
 * UDRE interrupt: called when UDR is ready to accept next byte.
 * We load next byte from buffer. If buffer becomes empty, disable UDRIE0.
 *
 * NOTE: Some AVR libc/device headers use USART0_UDRE_vect for USART0;
 * if you get a linker error, try replacing USART_UDRE_vect with
 * USART0_UDRE_vect.
 */

ISR(USART1_UDRE_vect)
{
    if (tx1_tail != tx1_head) {
        /* There is data to send */
        UDR1 = tx1_buf[tx1_tail];
        tx1_tail = (tx1_tail + (uint8_t) 1 ) & TX_BUF_SIZE_MASK;
    } else {
        /* Buffer empty - disable UDRE interrupt until new data is enqueued */
        UCSR1B &= ~(1 << UDRIE1);
    }
}

void usart1_init(const uint16_t baud) {
	const uint8_t U2Xn = 1;
	uint16_t ubrr = (F_CPU / ((16UL >> U2Xn) * baud)) - 1UL;

    UBRR1L = (uint8_t) ubrr;
    UBRR1H = (uint8_t) (ubrr >> 8);
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); /* 8N1 */
    UCSR1A |= (U2Xn << U2X1); /* double speed async */
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);

    usart1_rx_interrupt_enable();
}

/* Enable RX interrupt */
void usart1_rx_interrupt_enable(void) {
    UCSR1B |= (1 << RXCIE1);
}

/* Disable RX interrupt */
void usart1_rx_interrupt_disable(void) {
    UCSR1B &= ~(1 << RXCIE1);
}

/* Non-blocking getchar (returns 1 if a byte was available) */
uint8_t usart1_rx_buffered(uint8_t *out) {
    if (rx1_head == rx1_tail) return 0; /* empty */
    uint8_t val = rx1_buf[rx1_tail];
    rx1_tail = (uint8_t)((rx1_tail + 1) & RX_BUF_SIZE_MASK);
    *out = val;
    return 1;
}

/* Blocking receive (wait for data) */
uint8_t usart1_rx(void) {
    uint8_t ch;
    while (!usart1_rx_buffered(&ch));
    return ch;
}

uint8_t usart1_rx_available(void) {
	// rx head = 13, rx tail = 119, (13 - 119) & 127 = 22 = 13 + 128 - 119
    return (uint8_t)((rx1_head - rx1_tail) & RX_BUF_SIZE_MASK);
}


inline bool usart1_tx_ready(void) {
    return (UCSR1A & (1 << UDRE1)) != 0;
}

/* Hook for printf */
inline int usart1_putchar_printf(char c, FILE *stream) {
    (void)stream;
    if (c == '\n') usart1_tx('\r'); /* CRLF */
    usart1_tx((uint8_t)c);
    return 0;
}

/* Return 0 on success, -1 if buffer full (non-blocking) */
inline uint8_t usart1_tx_buffered(uint8_t c) {
    uint8_t next_head = (uint8_t)((tx1_head + 1U) & TX_BUF_SIZE_MASK);

    /* Check for full */
    if (next_head == tx1_tail) {
        return 0xff; /* buffer full */
    }

    tx1_buf[tx1_head] = c;
    /* update head (no need to disable interrupts here because head update is
       atomic for uint8_t; but to be strictly safe you could disable interrupts) */
    tx1_head = next_head;

    /* Enable UDRE interrupt to start/continue transmission.
       Enabling is safe even if ISR is already running; it will cause an
       interrupt if UDRE flag is set. */
    UCSR1B |= (1 << UDRIE1);

    return 0;
}

/* Blocking helper: waits until there's space and enqueues */
void usart1_tx(uint8_t c) {
    while (usart1_tx_buffered(c) != 0) {
        /* wait for space; small sleep to avoid tight busy loop */
        _delay_us(50);
    }
}
