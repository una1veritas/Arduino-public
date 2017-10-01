/*
 * sram.cpp
 *
 *  Created on: 2017/09/19
 *      Author: sin
 */

#include "sram.h"

void sram_bus_setup() {
  ADDRL_DIR = ADDRL_MASK; // fixed to be OUTPUT
  ADDRH_DIR = ADDRH_MASK; // fixed to be OUTPUT
  ADDRX_DIR |= ADDRX_MASK; // fixed to be OUTPUT
  DATA_DIR = DATA_MASK;
  CONTROL_DIR |= ( SRAM_WE | SRAM_OE | SRAM_CS ); //SRAM_ALE );
  CONTROL |= ( SRAM_WE | SRAM_OE | SRAM_CS ); //SRAM_ALE);
}

void sram_enable() {
  CONTROL &= ~SRAM_CS;  
}

void sram_disable() {
  CONTROL |= SRAM_CS;
}

uint8_t sram_read(uint32_t addr) {
  unsigned char val;
  addr_set32(addr);
  DATA_DIR = 0x00;
  DATA_OUT = 0xff;
  CONTROL &= ~SRAM_OE;
  __asm__ __volatile("nop");
  //__asm__ __volatile("nop");
  val = DATA_IN;
  CONTROL |= SRAM_OE; // valid data remains while
  return val;
}

void sram_write(uint32_t addr, uint8_t data) {
  addr_set32(addr);
  DATA_DIR = DATA_MASK;
  DATA_OUT = data;
  CONTROL &= ~SRAM_WE;
  CONTROL |= SRAM_WE;
}

