#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "atmega2560_usart.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* Default baud rates used by examples */
#define USART0_BAUD 57600UL
#define USART1_BAUD 9600UL

/* Provide FILE stream for stdout using usart0_putchar_printf */
static FILE usart0_stdout = FDEV_SETUP_STREAM(usart0_putchar_printf, NULL, _FDEV_SETUP_WRITE);

int main(void) {
    /* Initialize USART0 for console at 115200 */
    usart0_init(USART0_BAUD);

    /* Redirect printf to USART0 */
    stdout = &usart0_stdout;

    /* Initialize USART1 for a device at 9600 (example) */
    //usart1_init(USART0_BAUD);

    /* Enable RX interrupt for USART0 and global interrupts */
    usart0_rx_enable_interrupt();
    sei();

    printf("ATmega2560 USART0 test\r\n");


    /* Example: echo bytes received on USART0 back and forward to USART1 */
    for (;;) {
        uint8_t ch;
        if (usart0_getchar_nb(&ch)) {
            /* echo back on console */
            usart0_tx(ch);
            usart0_tx(ch);
            /* forward to USART1 */
            //usart1_tx(ch + 1);
        }
        /* ... other tasks ... */
    }
    return 0;
}
