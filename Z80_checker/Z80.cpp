#include "Z80.h"

void addrbus_inputmode() {
  DDRA = 0x00;
  DDRC = 0x00;
}

uint16_t addrbus_read() {
  return ((uint16_t) PINC)<<8 | PINA;
}

void databus_inputmode() {
  DDRF = 0x00;
  PORTF = 0x00;
}

void databus_outputmode() {
  DDRF = 0xff;
}

uint8_t databus_read() {
  return PINF;
}

uint8_t databus_write(uint8_t data) {
  PORTF = data;
  return data;
}

void clock_start(uint8_t presc, uint16_t top) {
  const uint8_t MODE = 4;

  cli();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (presc << CS10);
  TCCR1C |= (1 << FOC1A);

  sei();
}

void clock_disable() {
  pinMode(Z80_CLK_PIN, INPUT);
  digitalWrite(Z80_CLK_PIN, HIGH);
}

void clock_enable() {
  pinMode(Z80_CLK_PIN, OUTPUT);
}

void Z80_init() {
  pinMode(Z80_RESET_PIN, OUTPUT);
  digitalWrite(Z80_RESET_PIN, HIGH);
  
  pinMode(Z80_BUSREQ_PIN, OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, HIGH);
  
}

void Z80_reset() {
  digitalWrite(Z80_RESET_PIN, LOW);
  delay(500);
  digitalWrite(Z80_RESET_PIN, HIGH);
}

uint8_t Z80_busactivate() {
  pinMode(Z80_HALT_PIN, INPUT);
  pinMode(Z80_MREQ_PIN, INPUT);
  pinMode(Z80_IORQ_PIN, INPUT);
  pinMode(Z80_RD_PIN, INPUT);
  pinMode(Z80_WR_PIN, INPUT);
  pinMode(Z80_BUSACK_PIN, INPUT);
  
  addrbus_inputmode();
  databus_inputmode();
  
  pinMode(Z80_BUSREQ_PIN, OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, HIGH);
  pinMode(Z80_WAIT_PIN, OUTPUT);
  digitalWrite(Z80_WAIT_PIN, HIGH);
  while ( !digitalRead(Z80_BUSACK_PIN) );
  return digitalRead(Z80_BUSACK_PIN);
}

uint8_t Z80_businactivate() {
  pinMode(Z80_BUSACK_PIN, INPUT);  
  pinMode(Z80_BUSREQ_PIN, OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, LOW);
  while ( digitalRead(Z80_BUSACK_PIN) );
  return !digitalRead(Z80_BUSACK_PIN);
}

uint8_t Z80_mreq_read() {
  return !(digitalRead(Z80_MREQ_PIN) || digitalRead(Z80_RD_PIN));
}

uint8_t Z80_mreq_write() {
  return !(digitalRead(Z80_MREQ_PIN) || digitalRead(Z80_WR_PIN));
}

uint8_t Z80_iorq_read() {
  return !(digitalRead(Z80_IORQ_PIN) || digitalRead(Z80_RD_PIN));
}

uint8_t Z80_iorq_write() {
  return !(digitalRead(Z80_IORQ_PIN) || digitalRead(Z80_WR_PIN));
}

void Z80_wait_request() { digitalWrite(Z80_WAIT_PIN, LOW); }
void Z80_wait_cancel() { digitalWrite(Z80_WAIT_PIN, HIGH); }

uint8_t Z80_m1() { return digitalRead(Z80_M1_PIN); }

