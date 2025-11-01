/*
 * Z80_SIO.h
 *
 *  Created on: 2025/11/01
 *      Author: sin
 */

#ifndef Z80BUS_SIO_H_
#define Z80BUS_SIO_H_

void usart1_init(const uint16_t baud);
void usart1_rx_interrupt_enable(void);
void usart1_rx_interrupt_disable(void);
uint8_t usart1_rx_buffered(uint8_t *out);
uint8_t usart1_rx(void);
uint8_t usart1_rx_available(void);
bool usart1_tx_ready(void);
uint8_t usart1_tx_buffered(uint8_t c);
void usart1_tx(uint8_t c);

int usart1_putchar_printf(char c, FILE *stream);

#endif /* Z80BUS_SIO_H_ */
