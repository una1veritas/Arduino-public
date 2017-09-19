#include <avr/io.h>
#include "z80io.h"

void init_Timer1(uint8_t presc, uint16_t top) {
  const uint8_t MODE = 4;
//  noInterrupts();
  
  DDRB |= (1<<PB5);

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = 1600 - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (presc << CS10);
  TCCR1C |= (1 << FOC1A);

//  interrupts();
}


