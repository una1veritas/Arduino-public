/*
 * z80_controller.cpp
 *
 *  Created on: 2017/09/26
 *      Author: sin
 */
#include "z80_controller.h"

void z80_clock_start(uint16_t top) {
  const uint8_t MODE = 4;
  const uint8_t PRESCALER = 5;

  TIMSK1 = 0;

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (PRESCALER << CS10);
  TCCR1C |= (1 << FOC1A);

  DDRB |= (1<<PB5);
}

void z80_bus_setup() {
  DDRF = 0xff;
  PORTF = 0x00;
  DDRA = 0x00;
  DDRC = 0x00;

  pinMode(Z80_MREQ_PIN, INPUT);
  pinMode(Z80_IORQ_PIN, INPUT);
  pinMode(Z80_RD_PIN, INPUT);
  pinMode(Z80_WR_PIN, INPUT);

  pinMode(Z80_M1_PIN, INPUT);
  pinMode(Z80_RFSH_PIN, INPUT);
  pinMode(Z80_BUSACK_PIN, INPUT);
  digitalWrite(Z80_BUSACK_PIN, HIGH);
  pinMode(Z80_HALT_PIN, INPUT);
  digitalWrite(Z80_HALT_PIN, HIGH);

  pinMode(Z80_RESET_PIN, OUTPUT);
  digitalWrite(Z80_RESET_PIN, HIGH);
  pinMode(Z80_BUSREQ_PIN, OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, HIGH);
  pinMode(Z80_WAIT_PIN, OUTPUT);
  digitalWrite(Z80_WAIT_PIN, HIGH);
//  pinMode(Z80_NMI_PIN, OUTPUT);
//  digitalWrite(Z80_NMI_PIN, HIGH);
//  pinMode(Z80_INT_PIN, OUTPUT);
//  digitalWrite(Z80_INT_PIN, HIGH);
}

void z80_bus_status(busmode & bus) {
	  bus.M1 = !digitalRead(Z80_M1_PIN);
	  bus.RFSH = !digitalRead(Z80_RFSH_PIN);
	  bus.IORQ = !digitalRead(Z80_IORQ_PIN);
	  bus.MREQ = !digitalRead(Z80_MREQ_PIN);
	  bus.WR = !digitalRead(Z80_WR_PIN);
	  bus.RD = !digitalRead(Z80_RD_PIN);
	  bus.ADDR = ((uint16_t)PINA | (PINC<<8));
}

uint8_t z80_request_bus() {
    digitalWrite(Z80_BUSREQ_PIN, LOW);
    while ( digitalRead(Z80_BUSACK_PIN) );
    return digitalRead(Z80_BUSACK_PIN);
}

void disable_z80_mreq() {
	pinMode(ALLOW_Z80_MREQ_PIN, OUTPUT);
	digitalWrite(ALLOW_Z80_MREQ_PIN, HIGH); // mreq disabled.
}

void enable_z80_mreq() {
	pinMode(ALLOW_Z80_MREQ_PIN, OUTPUT);
	digitalWrite(ALLOW_Z80_MREQ_PIN, LOW); // mreq disabled.
}

void z80_databus_write(uint8_t data) {
	DDRF = 0xff;
	PORTF = data;
	return data;
}

uint8_t z80_databus_read(void) {
	uint8_t data;
	DDRF = 0x00;
	data = PINF;
	return data;
}
