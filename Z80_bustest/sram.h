/*
 * sram.h
 *
 *  Created on: 2017/09/26
 *      Author: sin
 */

#ifndef SRAM_H_
#define SRAM_H_

#include <avr/io.h>
#include "sram_pindef.h"

/*
#define bitset(port, bv) (port) |= (bv)
#define bitclr(port, bv) (port) &= ~(bv)
*/

void sram_bus_setup(void);
void sram_enable(void);
void sram_disable(void);
uint8_t sram_read(uint32_t addr);
void sram_write(uint32_t addr, uint8_t data);
void sram_bank_select(uint8_t bk);

inline void addr_set32(uint32_t addr) {
  ADDRL = (uint8_t) addr;
  addr >>= 8;
  ADDRH = (uint8_t) addr;
  addr >>= 8;
  ADDRX &= ~ADDRX_MASK;
  ADDRX |= ((uint8_t)addr) & ADDRX_MASK;
}

inline void sram_bank_select(uint8_t bk) {
  ADDRX &= ~ADDRX_MASK;
  ADDRX |= (bk & ADDRX_MASK);
}

inline void addr_set16(uint16_t addr) {
  ADDRL = (uint8_t) addr;
  addr >>= 8;
  ADDRH = (uint8_t) addr;
  addr >>= 8;
}


#endif /* SRAM_H_ */
