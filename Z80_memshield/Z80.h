#ifndef _Z80_H_
#define _Z80_H_

#include <Arduino.h>
#include "Z80_def.h"
#include "common.h"

void z80_bus_setup();
void z80_reset(unsigned long dur);

void z80_busreq(BYTE val);
BYTE z80_busack(WORD wcnt = 0);
void z80_wait_enable();
void z80_wait_disable();

BYTE z80_mreq_rd();
BYTE z80_mreq_wr();
BYTE z80_iorq_rd();
BYTE z80_iorq_wr();

#endif /* _Z80_H_ */

