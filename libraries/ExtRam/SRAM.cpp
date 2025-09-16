/*
 * SRAM.cpp
 *
 *  Created on: 2017/09/19
 *      Author: sin
 */

#include "SRAM.h"

void SRAM::bus_setup() {
  ADDRL_DIR = ADDRL_MASK; // fixed to be OUTPUT
  ADDRH_DIR = ADDRH_MASK; // fixed to be OUTPUT
  ADDRX_DIR |= ADDRX_MASK; // fixed to be OUTPUT
  DATA_DIR = DATA_MASK;
  CONTROL_DIR |= ( SRAM_WE | SRAM_OE | SRAM_CS ); //SRAM_ALE );
  CONTROL |= ( SRAM_WE | SRAM_OE | SRAM_CS ); //SRAM_ALE);
}

uint8_t SRAM::read(uint32_t addr) {
  unsigned char val;
  addr_set32(addr);
  enable();
  DATA_DIR = 0x00;
  DATA_OUT = 0xff;
  CONTROL &= ~SRAM_OE;
  __asm__ __volatile("nop");
  //__asm__ __volatile("nop");
  val = DATA_IN;
  CONTROL |= SRAM_OE; // valid data remains while
  disable();
  return val;
}

void SRAM::write(uint32_t addr, uint8_t data) {
  addr_set32(addr);
  enable();
  DATA_DIR = DATA_MASK;
  DATA_OUT = data;
  CONTROL &= ~SRAM_WE;
  CONTROL |= SRAM_WE;
  disable();
}

