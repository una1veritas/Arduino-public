#ifndef _Z80_H_
#define _Z80_H_

#include <Arduino.h>
#include "Z80_pindef.h"

void clock_start(uint8_t presc, uint16_t top);
void clock_enable();
void clock_disable();

void addrbus_inputmode();
uint16_t addrbus_read();
void databus_outputmode();
void databus_inputmode();
uint8_t databus_read();
uint8_t databus_write(uint8_t);

void Z80_init();
uint8_t Z80_busactivate(void);
uint8_t Z80_businactivate();
void Z80_reset(void);
void Z80_wait_request();
void Z80_wait_cancel();

uint8_t Z80_m1();
uint8_t Z80_mreq_read();
uint8_t Z80_mreq_write();
uint8_t Z80_iorq_read();
uint8_t Z80_iorq_write();

#endif /*_Z80_H_ */

