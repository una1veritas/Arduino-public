#ifndef _SRAM_H_
#define _SRAM_H_

#include "sram_pindef.h"

void sram_bus_activate() ;
void sram_bus_inactivate() ;
void sram_E2_enable();
void sram_E2_disable();
void sram_select();
void sram_deselect();

void sram_write(uint16_t addr, uint8_t data);
uint8_t sram_read(uint16_t addr);

#endif /*  _SRAM_H_ */
