#include <avr/io.h>
#include "z80io.h"
#include "z80io_pindef.h"
#include <Arduino.h>

void init_Timer1(uint8_t presc, uint16_t top) {
  const uint8_t MODE = 4;
//  noInterrupts();
  
  DDRB |= (1<<PB5);

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (presc << CS10);
  TCCR1C |= (1 << FOC1A);

//  interrupts();
}

void init_z80io() {
  pinMode(Z80_RESET_PIN, OUTPUT);
  digitalWrite(Z80_RESET_PIN, HIGH);
  
  pinMode(Z80_INT_PIN, OUTPUT);
  digitalWrite(Z80_INT_PIN, HIGH);
  pinMode(Z80_NMI_PIN, OUTPUT);
  digitalWrite(Z80_NMI_PIN, HIGH);
  pinMode(Z80_WAIT_PIN, OUTPUT);
  digitalWrite(Z80_WAIT_PIN, HIGH);
  pinMode(Z80_BUSREQ_PIN, OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, HIGH);

}

void z80io_reset() {
  digitalWrite(Z80_RESET_PIN, LOW);
  delay(500);
  digitalWrite(Z80_RESET_PIN, HIGH);
}

bool z80_busreq() {
  uint16_t climit = 1000;
  digitalWrite(Z80_BUSREQ_PIN, LOW);
  while (digitalRead(Z80_BUSACK_PIN) && climit > 0) {
    delay(5);
    --climit;
  }
  return digitalRead(Z80_BUSACK_PIN) == 0;
}

bool z80_busactivate() {
  digitalWrite(Z80_BUSREQ_PIN, HIGH);
  return digitalRead(Z80_BUSACK_PIN) == 0;  
}

