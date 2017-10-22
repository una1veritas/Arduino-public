#ifndef _Z80_H_
#define _Z80_H_

#include <Arduino.h>
#include "Z80_def.h"

void z80_bus_init();
void z80_reset(unsigned long dur);

void z80_busreq(uint8_t val);
byte z80_busack(void);

#endif /* _Z80_H_ */

