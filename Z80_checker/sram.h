#ifndef _SRAM_H_
#define _SRAM_H_

#include "sram_pindef.h"

void sram_bus_init() ;
void sram_select();
void sram_deselect();

void sram_write(uint32_t addr, uint8_t data);
uint8_t sram_read(uint32_t addr);
void sram_bank(uint8_t bank);

#endif /*  _SRAM_H_ */
