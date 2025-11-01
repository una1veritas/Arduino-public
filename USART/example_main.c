#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <ctype.h>
#include "atmega2560_usart.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* Default baud rates used by examples */
#define USART0_BAUD 115200UL
#define USART1_BAUD 19200UL

/* Provide FILE stream for stdout using usart0_putchar_printf */
static FILE usart0_stdout = FDEV_SETUP_STREAM(usart0_putchar_printf, NULL, _FDEV_SETUP_WRITE);

int main(void) {
    /* Initialize USART0 for console at 115200 */
    usart0_init(UBRR_VALUE(USART0_BAUD));

    /* Redirect printf to USART0 */
    stdout = &usart0_stdout;

    /* Initialize USART1 for a device at 9600 (example) */
    /* Enable RX interrupt for USART0 and global interrupts */
    usart1_init(UBRR_VALUE(USART1_BAUD));

    usart0_rx_interrupt_enable();
    usart1_rx_interrupt_enable();
    sei();

    printf("ATmega2560 USART0 test\r\n");
    printf("Hello boys and girls!\r\n");


    /* Example: echo bytes received on USART0 back and forward to USART1 */
    for (;;) {
        uint8_t ch;
        if (usart0_rx_buffered(&ch)) {
            /* echo back on console */
            usart1_tx(ch);
        	if (isalpha(ch)) {
        		ch = toupper(ch);
        		if (ch == 'Z') {
        			ch = 'A';
        		} else {
        			ch += 1;
        		}
        	}
            usart0_tx(ch);
            /* forward to USART1 */
        }
        if (usart1_rx_buffered(&ch)) {
        	usart0_tx(ch);
        }
        /* ... other tasks ... */
    }
    return 0;
}
