/*
 * main.c
 *
 *  Created on: 2025/10/29
 *      Author: sin
 */
/*
 * buffered_usart.c
 * Buffered RX/TX USART for AVR (example: ATmega328P)
 *
 * Compile with: avr-gcc -mmcu=atmega328p ...
 */

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Configuration */
#define BAUD 115200
/* Compute UBRR for asynchronous normal speed: */
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

/* Buffer sizes (power of two) */
#define RX_BUF_BITS 6                 /* 2^6 = 64 bytes */
#define RX_BUF_SIZE (1U << RX_BUF_BITS)
#define RX_BUF_MASK (RX_BUF_SIZE - 1)

#define TX_BUF_BITS 6                 /* 64 bytes */
#define TX_BUF_SIZE (1U << TX_BUF_BITS)
#define TX_BUF_MASK (TX_BUF_SIZE - 1)

/* Buffers and indices */
static volatile uint8_t rx_buf[RX_BUF_SIZE];
static volatile uint8_t tx_buf[TX_BUF_SIZE];

static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;

/* Initialize USART (8N1, no parity) and enable RX interrupt */
void usart_init(void)
{
    /* Baud rate */
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)(UBRR_VALUE & 0xFF);

    /* Frame format: 8 data bits, no parity, 1 stop bit */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    /* Enable receiver, transmitter and RX Complete Interrupt.
       TX interrupt (UDRE) will be enabled on-demand when we have data to send. */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

/* Return number of bytes available in RX buffer */
uint8_t usart_available(void)
{
    int8_t h, t;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        h = rx_head;
        t = rx_tail;
    }
    return uint8_t(h - t) & RX_BUF_MASK;
}

/* Read one byte from RX buffer; returns -1 if none available */
uint8_t usart_read(void)
{
    uint8_t data = 0xff;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        if (rx_head != rx_tail) {
            data = rx_buf[rx_tail];
            rx_tail = (uint8_t)((rx_tail + 1) & RX_BUF_MASK);
        }
    }
    return data;
}

/* Read one byte from RX buffer */
uint8_t usart_read_blocking(void)
{
    uint8_t data = 0xff;
    while (rx_head != rx_tail) { }
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        data = rx_buf[rx_tail];
        rx_tail = (uint8_t)((rx_tail + 1) & RX_BUF_MASK);
    }
    return data;
}

/* Blocking send: wait until UDRE ready then write to UDR0 directly */
void usart_send_blocking(uint8_t b)
{
    while (!(UCSR0A & (1 << UDRE0))) {
        /* wait */
    }
    UDR0 = b;
}

/* Non-blocking (async) send: push into TX buffer and enable UDRE interrupt.
   Returns true if queued, false if buffer full. */
bool usart_send_async(uint8_t b)
{
    bool queued = false;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        uint8_t next = (uint8_t)((tx_head + 1) & TX_BUF_MASK);
        if (next != tx_tail) { /* room */
            tx_buf[tx_head] = b;
            tx_head = next;
            queued = true;
            /* Enable UDRE interrupt so transmitter will drain the buffer */
            UCSR0B |= (1 << UDRIE0);
        } else {
            /* buffer full: drop or handle error */
            queued = false;
        }
    }
    return queued;
}

/* Optional helper: send a C string asynchronously (returns number queued count) */
size_t usart_write_str_async(const char *s)
{
    size_t count = 0;
    while (s && *s) {
        if (usart_send_async((uint8_t)*s++)) count++;
        else break; /* buffer full */
    }
    return count;
}

/* ISR: RX complete — read UDR0 and store into rx_buf */
ISR(USART_RX_vect)
{
    uint8_t data = UDR0;
    uint8_t next = (uint8_t)((rx_head + 1) & RX_BUF_MASK);
    if (next != rx_tail) {
        rx_buf[rx_head] = data;
        rx_head = next;
    } else {
        /* Buffer full: incoming byte dropped. Optionally set overflow flag. */
    }
}

/* ISR: Data Register Empty — called when UDR0 ready for new byte
   We pop from tx_buf and write to UDR0. When buffer empty, disable UDRE interrupt. */
ISR(USART_UDRE_vect)
{
    if (tx_head == tx_tail) {
        /* nothing to send: disable UDRE interrupt */
        UCSR0B &= ~(1 << UDRIE0);
    } else {
        UDR0 = tx_buf[tx_tail];
        tx_tail = (uint8_t)((tx_tail + 1) & TX_BUF_MASK);
    }
}

/* Example usage */
int main(void)
{
    usart_init();
    sei(); /* enable global interrupts */

    /* Example: send a startup message (may queue) */
    usart_write_str_async("USART buffered RX/TX ready\r\n");

    /* Main loop: echo received bytes back (async) */
    for (;;) {
        int c = usart_read();
        if (c >= 0) {
            /* echo */
            /* use async to avoid blocking the main loop */
            if (!usart_send_async((uint8_t)c)) {
                /* If async buffer full, fallback to blocking send */
                usart_send_blocking((uint8_t)c);
            }
        }

        /* Do other non-blocking tasks here... */
    }
    return 0;
}
