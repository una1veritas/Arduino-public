#include <Arduino.h>
#include "common.h"
#include "sram.h"

void sram_bus_init() {
  DDR(SRAM_ADDRL_PORT) = SRAM_ADDRL_MASK; 
  DDR(SRAM_ADDRH_PORT) = SRAM_ADDRH_MASK; 
  pinMode(SRAM_ADDRX0_PIN, OUTPUT);
  pinMode(SRAM_ADDRX1_PIN, OUTPUT);
  pinMode(SRAM_ADDRX2_PIN, OUTPUT);

  DDR(SRAM_DATA_OUT) = 0x00; 

  pinMode(SRAM_ALE_OE_PIN,OUTPUT);
  digitalWrite(SRAM_ALE_OE_PIN,HIGH);
  pinMode(SRAM_ALE_PIN,OUTPUT);
  digitalWrite(SRAM_ALE_PIN,LOW);
  
  pinMode(SRAM_CS_PIN,OUTPUT);
  pinMode(SRAM_OE_PIN,OUTPUT);
  pinMode(SRAM_WE_PIN,OUTPUT);
  digitalWrite(SRAM_CS_PIN,HIGH);
  digitalWrite(SRAM_OE_PIN,HIGH);
  digitalWrite(SRAM_WE_PIN,HIGH);
}

void sram_bus_release() {
  DDR(SRAM_ADDRL_PORT) = 0; 
  DDR(SRAM_ADDRH_PORT) = 0; 
  DDR(SRAM_DATA_OUT) = 0; 
  pinMode(SRAM_CS_PIN,INPUT);
  pinMode(SRAM_OE_PIN,INPUT);
  pinMode(SRAM_WE_PIN,INPUT);
  digitalWrite(SRAM_CS_PIN,HIGH);
  digitalWrite(SRAM_OE_PIN,HIGH);
  digitalWrite(SRAM_WE_PIN,HIGH);

  pinMode(SRAM_ALE_OE_PIN,OUTPUT);
  digitalWrite(SRAM_ALE_OE_PIN,HIGH);
}

void sram_select() {
  digitalWrite(SRAM_CS_PIN, LOW);
}

void sram_deselect() {
    digitalWrite(SRAM_CS_PIN, HIGH);
}

inline void addr16_set(uint16_t addr) {
  SRAM_ADDRL_PORT = (uint8_t) addr;
  addr >>= 8;
  SRAM_ADDRH_PORT = (uint8_t) addr;
}

void sram_bank(uint8_t bank) {
  bank &= ~SRAM_ADDRX_MASK;
  digitalWrite(SRAM_ADDRX0_PIN, (bank & SRAM_ADDRX0_MASK) != 0);
  digitalWrite(SRAM_ADDRX1_PIN, (bank & SRAM_ADDRX1_MASK) != 0);
  digitalWrite(SRAM_ADDRX2_PIN, (bank & SRAM_ADDRX2_MASK) != 0);
}

uint8_t sram_read(uint16_t addr) {
  unsigned char val;
  SRAM_DATA_OUT = 0x00;
  SRAM_DATA_DDR = 0x00; /*&= ~SRAM_DATA_MASK; */
  addr16_set(addr);
  digitalWrite(SRAM_OE_PIN,LOW);
  //__asm__ __volatile("NOP");
  val = SRAM_DATA_IN;
  digitalWrite(SRAM_OE_PIN,HIGH);
  return val;
}

void sram_write(uint16_t addr, uint8_t data) {
  addr16_set(addr);
  SRAM_DATA_DDR = SRAM_DATA_MASK;
  SRAM_DATA_OUT = data;
  digitalWrite(SRAM_WE_PIN, LOW);
  digitalWrite(SRAM_WE_PIN, HIGH);
}


