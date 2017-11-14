#include <Arduino.h>
#include "common.h"
#include "mem.h"

void sram_bus_setup() {
  DDR(SRAM_ADDRL_PORT) = SRAM_ADDRL_MASK; 
  DDR(SRAM_ADDRH_PORT) = SRAM_ADDRH_MASK; 
  pinMode(SRAM_ADDRX0_PIN, OUTPUT);
  pinMode(SRAM_ADDRX1_PIN, OUTPUT);
  pinMode(SRAM_ADDRX2_PIN, OUTPUT);

  DDR(SRAM_DATA_OUT) = 0x00; 

#ifndef USE_XMEM_ALE
  /* make both ALE input and output pins to be in Z-state */
  pinMode(SRAM_ALE_OE_PIN,OUTPUT);   digitalWrite(SRAM_ALE_OE_PIN,HIGH);
//  pinMode(SRAM_ALE_PIN,OUTPUT); //  digitalWrite(SRAM_ALE_PIN,LOW);
#endif // USE_XMEM_ALE
  
  pinMode(SRAM_CS_PIN,OUTPUT);   digitalWrite(SRAM_CS_PIN,HIGH);
  pinMode(SRAM_OE_PIN,OUTPUT);   digitalWrite(SRAM_OE_PIN,HIGH);
  pinMode(SRAM_WE_PIN,OUTPUT);   digitalWrite(SRAM_WE_PIN,HIGH);
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
  sram_select();
  SRAM_DATA_OUT = 0x00;
  SRAM_DATA_DDR = 0x00; /*&= ~SRAM_DATA_MASK; */
  addr16_set(addr);
  digitalWrite(SRAM_OE_PIN,LOW);
  //__asm__ __volatile("NOP");
  val = SRAM_DATA_IN;
  digitalWrite(SRAM_OE_PIN,HIGH);
  sram_deselect();
  return val;
}

uint8_t sram_write(uint16_t addr, uint8_t data) {
	sram_select();
  addr16_set(addr);
  SRAM_DATA_DDR = SRAM_DATA_MASK;
  SRAM_DATA_OUT = data;
  digitalWrite(SRAM_WE_PIN, LOW);
  digitalWrite(SRAM_WE_PIN, HIGH);
  sram_deselect();
  return data;
}

uint16_t sram_check(uint16_t addr, const uint16_t nbytes) {
  uint16_t errcount = 0;
  uint8_t randbytes[31];
  uint8_t orgval;
  uint8_t readout;

  randomSeed(millis());
  for(uint16_t i = 0; i < 31; i++) {
    randbytes[i] = random(0, 256);
  }

  for(uint16_t i = 0; i < nbytes; i++) {
	  orgval = sram_read(addr+i);
	  sram_write(addr+i, randbytes[(addr+i) % 31]);
	  readout = sram_read(addr+i);
	  if ( readout != randbytes[(addr+i) % 31] ) {
		  errcount++;
	  }
	  sram_write(addr+i, orgval);
  }
  return errcount;
}


