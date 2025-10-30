# ATmega2560 USART examples (AVR-GCC)

This example set shows:
- usart0: polled TX, interrupt-driven RX ring buffer, printf() redirection
- usart1: simple polled TX example
- F_CPU defaults to 16 MHz; change if your board differs.

Files:
- atmega2560_usart.h / atmega2560_usart.c — implementation and API
- example_main.c — shows how to use USART0 & USART1
- Makefile — minimal build

Build (example, requires avr-gcc, avr-libc, avrdude):
  make

Flash (example):
  make flash

Adjust MCU and programmer/port in the Makefile before flashing.

Notes:
- RX buffer size is 128 (power of two). Change RX_BUF_SIZE in the C file if needed.
- For higher baud or different clock, recompute UBRR. The code uses the standard formula for normal speed (USARTn in normal mode).
- If you want double speed (U2Xn), change the calculation and set UCSRnA |= (1<<U2Xn).