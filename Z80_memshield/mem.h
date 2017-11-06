#ifndef _MEM_H_
#define _MEM_H_

#include "mem_def.h"

void sram_bus_init() ;
void sram_bus_release();
void sram_select();
void sram_deselect();

uint8_t sram_read(uint16_t addr);
uint8_t sram_write(uint16_t addr, uint8_t data);
void sram_bank(uint8_t bank);

uint16_t sram_check(uint16_t addr, const uint16_t nbytes);

#endif /*  _MEM_H_ */
