#include <Arduino.h>
#include "sram.h"

#define DDR(port)  (*((&port)-1))
#define PIN(port)  (*((&port)-2))

void sram_bus_init() {
  DDR(SRAM_ADDRL_PORT) = 0x00; 
  DDR(SRAM_ADDRH_PORT) = 0xff; 
  DDR(SRAM_ADDRX_PORT) |= SRAM_ADDRX_MASK;  
  pinMode(SRAM_ALE_PIN,OUTPUT);
  digitalWrite(SRAM_ALE_PIN,HIGH);
  
  pinMode(SRAM_CS_PIN,OUTPUT);
  pinMode(SRAM_OE_PIN,OUTPUT);
  pinMode(SRAM_WE_PIN,OUTPUT);
  digitalWrite(SRAM_CS_PIN,HIGH);
  digitalWrite(SRAM_OE_PIN,HIGH);
  digitalWrite(SRAM_WE_PIN,HIGH);
}

void sram_select() {
  digitalWrite(SRAM_CS_PIN, LOW);
}

void sram_deselect() {
    digitalWrite(SRAM_CS_PIN, HIGH);
}

inline void addr32_set(uint32_t addr) {
  DDR(SRAM_ADDRL_PORT) = SRAM_ADDRL_MASK;
  digitalWrite(SRAM_ALE_PIN,LOW);
  SRAM_ADDRL_PORT = (uint8_t) addr;
  digitalWrite(SRAM_ALE_PIN,HIGH);
  addr >>= 8;
  SRAM_ADDRH_PORT = (uint8_t) addr;
  addr >>= 8;
  SRAM_ADDRX_PORT &= ~SRAM_ADDRX_MASK;
  SRAM_ADDRX_PORT |= addr & SRAM_ADDRX_MASK;
  
}

void sram_bank(uint8_t bank) {
  SRAM_ADDRX_PORT &= ~SRAM_ADDRX_MASK;
  SRAM_ADDRX_PORT |= bank & SRAM_ADDRX_MASK;
}

uint8_t sram_read(uint32_t addr) {
  unsigned char val;
  addr32_set(addr);
  SRAM_DATA_DDR = 0x00; /*&= ~SRAM_DATA_MASK; */
  digitalWrite(SRAM_OE_PIN,LOW);
  __asm__ __volatile__ ("nop");
  val = SRAM_DATA_IN;
  digitalWrite(SRAM_OE_PIN,HIGH);
  return val;
}

void sram_write(uint32_t addr, uint8_t data) {
  addr32_set(addr);
  SRAM_DATA_DDR = SRAM_DATA_MASK;
  SRAM_DATA_OUT = data;
  digitalWrite(SRAM_WE_PIN, LOW);
  digitalWrite(SRAM_WE_PIN, HIGH);
}


