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
#define RX_BUF_SIZE (1<<7)
#define RX_BUF_SIZE_MASK (RX_BUF_SIZE - 1)

/* RX0 */
static volatile uint8_t r0_buf[RX_BUF_SIZE];
static volatile uint8_t rx0_head = 0;
static volatile uint8_t rx0_tail = 0;

/* assumes U2Xn = 1 */
void usart0_init(const uint16_t ubrr) {

    /* Set baud rate */
    UBRR0L = (uint8_t) ubrr;
    UBRR0H = (uint8_t) (ubrr >> 8);

    /* Async, parity disabled, stop bit 1, character size 8-bit,
     * clock polarity TX data change at rising, RX falling
     */
    UCSR0C = (0 << UMSEL00) | (0 << UPM00) | (0 << USBS0 ) | (3 << UCSZ00) | (0 << UCPOL0) ;

    /* double speed mode */
    UCSR0A |= (U2Xn << U2X0);

    /* Enable TX and RX
     * interrupts left off until explicitly enabled */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (0 << UCSZ02);
}

/* Enable RX interrupt */
void usart0_rx_interrupt_enable(void) {
    UCSR0B |= (1 << RXCIE0);
}

/* Disable RX interrupt */
void usart0_rx_interrupt_disable(void) {
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

/* Non-blocking receive (returns 1 if a byte was available) */
uint8_t usart0_rx_buffered(uint8_t *out) {
    if (rx0_head == rx0_tail) return 0; /* empty */
    uint8_t val = r0_buf[rx0_tail];
    // isr does not change the value of tail unless buffer is full
    rx0_tail = (uint8_t)((rx0_tail + 1) & RX_BUF_SIZE_MASK);
    *out = val;
    return 1;
}

/* Blocking receive (wait for data) */
uint8_t usart0_rx(void) {
    uint8_t ch;
    while (!usart0_rx_buffered(&ch));
    return ch;
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
    uint8_t next = (uint8_t)((rx0_head + 1) & RX_BUF_SIZE_MASK);
    if (next == rx0_tail) {
        /* Buffer overflow: drop old data (advancing tail) or drop new byte.
           Here we drop oldest byte to make space. */
        rx0_tail = (uint8_t)((rx0_tail + 1) & RX_BUF_SIZE_MASK);
    }
    r0_buf[rx0_head] = data;
    rx0_head = next;
}

uint8_t usart0_rx_available(void) {
    return (uint8_t) ((rx0_head - rx0_tail) & RX_BUF_SIZE_MASK);
}

/* -------------------- USART1 (example second port, polled) -------------------- */
/* RX1 */
static volatile uint8_t rx1_buf[RX_BUF_SIZE];
static volatile uint8_t rx1_head = 0;
static volatile uint8_t rx1_tail = 0;

void usart1_init(const uint16_t ubrr) {
    UBRR1L = (uint8_t) ubrr;
    UBRR1H = (uint8_t) (ubrr >> 8);
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); /* 8N1 */
    UCSR1A |= (U2Xn << U2X1); /* double speed async */
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);
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
int usart1_rx_buffered(uint8_t *out) {
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

/* TX1 ring buffer */
#define TX_BUF_SIZE (1<<6)
#define TX_BUF_SIZE_MASK (TX_BUF_SIZE - 1)

/* TX1 ring buffer state (shared with ISR) */
static volatile uint8_t tx1_buf[TX_BUF_SIZE];
static volatile uint8_t tx1_head; /* index where next byte will be written */
static volatile uint8_t tx1_tail; /* index of next byte to send */

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

