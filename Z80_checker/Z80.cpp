#include "Z80.h"


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

uint8_t Z80_bus_activate() {
  pinMode(Z80_HALT_PIN, INPUT);
  pinMode(Z80_MREQ_PIN, INPUT);
  pinMode(Z80_IORQ_PIN, INPUT);
  pinMode(Z80_RD_PIN, INPUT);
  pinMode(Z80_WR_PIN, INPUT);
  pinMode(Z80_BUSACK_PIN, INPUT);
    
  pinMode(Z80_BUSREQ_PIN, OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, HIGH);
  pinMode(Z80_WAIT_PIN, OUTPUT);
  digitalWrite(Z80_WAIT_PIN, HIGH);
  while ( !digitalRead(Z80_BUSACK_PIN) );
  return digitalRead(Z80_BUSACK_PIN);
}

uint8_t Z80_bus_request() {
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

uint8_t Z80_m1() { return !digitalRead(Z80_M1_PIN); }

