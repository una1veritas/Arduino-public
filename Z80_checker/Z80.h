#ifndef _Z80_H_
#define _Z80_H_

#include <Arduino.h>
#include "Z80_pindef.h"

void Z80_init();
uint8_t Z80_bus_activate(void);
uint8_t Z80_bus_request();
void Z80_reset(void);
void Z80_wait_request();
void Z80_wait_cancel();

uint8_t Z80_m1();
uint8_t Z80_mreq_read();
uint8_t Z80_mreq_write();
uint8_t Z80_iorq_read();
uint8_t Z80_iorq_write();

#endif /*_Z80_H_ */

