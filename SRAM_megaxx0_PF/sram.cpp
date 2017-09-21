/*
 * sram.cpp
 *
 *  Created on: 2017/09/19
 *      Author: sin
 */

#include "sram.h"

void sram_init() {
  ADDRL_DIR = ADDRL_MASK; // fixed to be OUTPUT
  ADDRH_DIR = ADDRH_MASK; // fixed to be OUTPUT
  ADDRX_DIR |= ADDRX_MASK; // fixed to be OUTPUT
  DATA_DIR = DATA_MASK;
  CONTROL_DIR |= ( SRAM_WE | SRAM_OE | SRAM_CS ); //SRAM_ALE );
  CONTROL |= ( SRAM_WE | SRAM_OE | SRAM_CS ); //SRAM_ALE);
}


inline void sram_bank(uint8_t bk) {
  ADDRX &= ~ADDRX_MASK;
  ADDRX |= (bk & ADDRX_MASK);
}

uint8_t sram_read(uint32_t addr) {
  unsigned char val;
  CONTROL &= ~SRAM_CS;
//  DATA_OUT = 0xff; // clear the pull-ups
  DATA_DIR = 0x00;
  addr_set(addr);
  CONTROL &= ~SRAM_OE;
  __asm__ __volatile__ ("nop");
  val = DATA_IN;
  CONTROL |= SRAM_OE; // valid data remains while
  CONTROL |= SRAM_CS;
  return val;
}

void sram_write(uint32_t addr, uint8_t data) {
  CONTROL &= ~SRAM_CS;
  addr_set(addr);
  DATA_DIR = DATA_MASK;
  DATA_OUT = data;
  CONTROL &= ~SRAM_WE;
  CONTROL |= SRAM_WE;
  CONTROL |= SRAM_CS;
}

