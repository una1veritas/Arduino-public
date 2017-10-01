/*
 * z80_controller.h
 *
 *  Created on: 2017/09/26
 *      Author: sin
 */

#ifndef Z80_CONTROLLER_H_
#define Z80_CONTROLLER_H_

#include <Arduino.h>
#include <inttypes.h>
#include "z80_pindef.h"

struct busmode {
	bool M1, RFSH, IORQ, MREQ, WR, RD;
	uint16_t ADDR;
};

void z80_clock_start(uint16_t top);
void z80_bus_setup();
void z80_bus_status(busmode & bus);

uint8_t z80_request_bus();

void z80_databus_write(uint8_t data);
uint8_t z80_databus_read(void);

void enable_z80_mreq(void);
void disable_z80_mreq(void);

#endif /* Z80_CONTROLLER_H_ */
