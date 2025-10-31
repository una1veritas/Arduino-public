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


void usart0_init(const uint16_t ubrr) {

    /* Set baud rate */
    UBRR0L = (uint8_t) ubrr;
    UBRR0H = (uint8_t) (ubrr >> 8);

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
    if (rx0_head == rx0_tail) return 0; /* empty */
    uint8_t val = r0_buf[rx0_tail];
    rx0_tail = (uint8_t)((rx0_tail + 1) & RX_BUF_SIZE_MASK);
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
    return (uint8_t)((RX_BUF_SIZE + rx0_head - rx0_tail) & RX_BUF_SIZE_MASK);
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
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);
}

/* Enable RX interrupt */
void usart1_rx_enable_interrupt(void) {
    UCSR1B |= (1 << RXCIE1);
}

/* Disable RX interrupt */
void usart1_rx_disable_interrupt(void) {
    UCSR1B &= ~(1 << RXCIE1);
}

void usart1_tx(uint8_t b) {
    while (!(UCSR1A & (1 << UDRE1)));
    UDR1 = b;
}

bool usart1_tx_ready(void) {
    return (UCSR0A & (1 << UDRE0)) != 0;
}

/* Non-blocking getchar (returns 1 if a byte was available) */
int usart1_getchar_nb(uint8_t *out) {
    if (rx1_head == rx1_tail) return 0; /* empty */
    uint8_t val = rx1_buf[rx1_tail];
    rx1_tail = (uint8_t)((rx1_tail + 1) & RX_BUF_SIZE_MASK);
    *out = val;
    return 1;
}

/* Blocking receive (wait for data) */
uint8_t usart1_rx_blocking(void) {
    uint8_t ch;
    while (!usart1_getchar_nb(&ch));
    return ch;
}

void usart1_send_string(const char *s) {
    while (*s) {
        usart1_tx((uint8_t)*s++);
    }
}

/* Hook for printf */
int usart1_putchar_printf(char c, FILE *stream) {
    (void)stream;
    if (c == '\n') usart1_tx('\r'); /* CRLF */
    usart0_tx((uint8_t)c);
    return 0;
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
/*
 * 0x004
A
jmp USART1_UDRE ; USART1,UDR Empty Handler
0x004
C
jmp USART1_TXC ; USART1 TX Complete Handler
 */
ISR(USART1_TX_vect)
{
	if (next != rx1_tail) {
		data = tx1_buf[tx1_head];
		usart1_tx(data);
	}

}

/* --- */
/* ---------- Configuration ---------- */
#define TX_BAUD 9600U
/* Buffer size must be a power of two for the mask arithmetic below */
#define TX_BUF_SIZE 64U
#if (TX_BUF_SIZE & (TX_BUF_SIZE - 1)) != 0
#error "TX_BUF_SIZE must be a power of two"
#endif
#define TX_BUF_MASK (TX_BUF_SIZE - 1U)
/* ----------------------------------- */

/* TX ring buffer state (shared with ISR) */
static volatile uint8_t tx_buf[TX_BUF_SIZE];
static volatile uint8_t tx_head; /* index where next byte will be written */
static volatile uint8_t tx_tail; /* index of next byte to send */

/* Initialize USART0 for async 8N1 and enable TX + UDRE interrupt only when data */
void usart0_init(uint32_t baud)
{
    uint16_t ubrr = (uint16_t)(F_CPU / 16UL / baud - 1UL);

    /* Set baud rate */
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);

    /* Set frame format: 8 data bits, 1 stop bit */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    /* Enable transmitter (don't enable UDRIE0 yet - only when we have data) */
    UCSR0B = (1 << TXEN0);

    /* initialize buffer indexes */
    tx_head = tx_tail = 0;

    /* enable global interrupts (caller may prefer to do this) */
    sei();
}

/* Return 0 on success, -1 if buffer full (non-blocking) */
int usart0_tx_enqueue(uint8_t c)
{
    uint8_t next_head = (uint8_t)((tx_head + 1U) & TX_BUF_MASK);

    /* Check for full */
    if (next_head == tx_tail) {
        return -1; /* buffer full */
    }

    tx_buf[tx_head] = c;
    /* update head (no need to disable interrupts here because head update is
       atomic for uint8_t; but to be strictly safe you could disable interrupts) */
    tx_head = next_head;

    /* Enable UDRE interrupt to start/continue transmission.
       Enabling is safe even if ISR is already running; it will cause an
       interrupt if UDRE flag is set. */
    UCSR0B |= (1 << UDRIE0);

    return 0;
}

/* Blocking helper: waits until there's space and enqueues */
void usart0_tx_send_blocking(uint8_t c)
{
    while (usart0_tx_enqueue(c) < 0) {
        /* wait for space; small sleep to avoid tight busy loop */
        _delay_us(50);
    }
}

/* Enqueue a null-terminated string (non-blocking: returns number of chars enqueued) */
size_t usart0_tx_enqueue_str_nb(const char *s)
{
    size_t n = 0;
    while (*s) {
        if (usart0_tx_enqueue((uint8_t)*s++) != 0) break;
        n++;
    }
    return n;
}

/* Blocking send string (will wait for buffer space) */
void usart0_tx_send_str_blocking(const char *s)
{
    while (*s) {
        usart0_tx_send_blocking((uint8_t)*s++);
    }
}

/*
 * UDRE interrupt: called when UDR is ready to accept next byte.
 * We load next byte from buffer. If buffer becomes empty, disable UDRIE0.
 *
 * NOTE: Some AVR libc/device headers use USART0_UDRE_vect for USART0;
 * if you get a linker error, try replacing USART_UDRE_vect with
 * USART0_UDRE_vect.
 */
ISR(USART_UDRE_vect)
{
    if (tx_tail != tx_head) {
        /* There is data to send */
        UDR0 = tx_buf[tx_tail];
        tx_tail = (uint8_t)((tx_tail + 1U) & TX_BUF_MASK);
    } else {
        /* Buffer empty - disable UDRE interrupt until new data is enqueued */
        UCSR0B &= ~(1 << UDRIE0);
    }
}
