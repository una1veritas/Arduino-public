#include <Arduino.h>
#include "sram.h"

void sram_bus_activate() {
  *((&SRAM_ADDRL_PORT)-1) = 0xff;
  *((&SRAM_ADDRH_PORT)-1) = 0xff;
  *((&SRAM_BANK_PORT)-1) |= SRAM_BANK_MASK;
  pinMode(SRAM_E2_PIN,OUTPUT);
  digitalWrite(SRAM_E2_PIN,HIGH);
  pinMode(SRAM_CS_PIN,OUTPUT);
  pinMode(SRAM_OE_PIN,OUTPUT);
  pinMode(SRAM_WE_PIN,OUTPUT);
  digitalWrite(SRAM_CS_PIN,HIGH);
  digitalWrite(SRAM_OE_PIN,HIGH);
  digitalWrite(SRAM_WE_PIN,HIGH);
}

void sram_bus_inactivate() {
  *((&SRAM_ADDRL_PORT)-1) = 0x00;
  *((&SRAM_ADDRH_PORT)-1) = 0x00;
  *((&SRAM_BANK_PORT)-1) &= ~SRAM_BANK_MASK;
  pinMode(SRAM_E2_PIN,OUTPUT);
  digitalWrite(SRAM_E2_PIN,LOW);
  pinMode(SRAM_CS_PIN,INPUT);
  pinMode(SRAM_OE_PIN,INPUT);
  pinMode(SRAM_WE_PIN,INPUT);
}

void sram_E2_enable() {
  digitalWrite(SRAM_E2_PIN, HIGH);
}

void sram_E2_disable() {
    digitalWrite(SRAM_E2_PIN, LOW);
}

void sram_select() {
  digitalWrite(SRAM_CS_PIN, LOW);
}

void sram_deselect() {
    digitalWrite(SRAM_CS_PIN, LOW);
}

inline void addr16_set(uint16_t addr) {
  SRAM_ADDRL_PORT = (uint8_t) addr;
  addr >>= 8;
  SRAM_ADDRH_PORT = (uint8_t) addr;
}

inline void sram_bank(uint8_t bank) {
  SRAM_BANK_PORT &= ~SRAM_BANK_MASK;
  SRAM_BANK_PORT |= bank & SRAM_BANK_MASK;
}

uint8_t sram_read(uint16_t addr) {
  unsigned char val;
  addr16_set(addr);
  *((&SRAM_DATA_PORT)-1) = 0x00; /*&= ~SRAM_DATA_MASK; */
  digitalWrite(SRAM_OE_PIN,LOW);
  __asm__ __volatile__ ("nop");
  val = *((&SRAM_DATA_PORT)-2);
  digitalWrite(SRAM_OE_PIN,HIGH);
  return val;
}

void sram_write(uint16_t addr, uint8_t data) {
  addr16_set(addr);
  *((&SRAM_DATA_PORT)-1) |= SRAM_DATA_MASK;
  SRAM_DATA_PORT = data;
  digitalWrite(SRAM_WE_PIN, LOW);
  digitalWrite(SRAM_WE_PIN, HIGH);
}

