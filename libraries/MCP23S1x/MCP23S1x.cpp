/*
 * MCP23S1x.cpp
 *
 *  Created on: 2014/04/19
 *      Author: sin
 */

#include <Arduino.h>

#include "MCP23S1x.h"

	uint8_t MCP23S1x::read(const uint8_t reg) {
		uint8_t val;
		select();
		SPI.transfer(contbyte | CONTROL_READ);
		SPI.transfer(reg);
		val = SPI.transfer(0x00);
		deselect();
		return val;
	}

	uint8_t MCP23S1x::write(const uint8_t reg, uint8_t val) {
		select();
		SPI.transfer(contbyte | CONTROL_WRITE);
		SPI.transfer(reg);
		val = SPI.transfer(val);
		deselect();
		return val;
	}
/*
	uint16_t MCP23S1x::read16(const uint8_t reg) {
		uint16_t val;
		select();
		SPI.transfer(contbyte | CONTROL_READ);
		SPI.transfer(reg);
		val = SPI.transfer(0x00);
		val = (val<<8) | SPI.transfer(0x00);
		deselect();
		return val;
	}

	uint16_t MCP23S1x::write16(const uint8_t reg, uint16_t val) {
		uint16_t res;
		select();
		SPI.transfer(contbyte | CONTROL_WRITE);
		SPI.transfer(reg);
		res = SPI.transfer(val>>8&0xff);
		res = (res<<8) | SPI.transfer(val&0xff);
		deselect();
		return res;
	}
*/
	void MCP23S1x::portAMode(const uint8_t dbits) {
		write(IODIRA, dbits);
	}

	void MCP23S1x::portBMode(const uint8_t dbits) {
		write(IODIRB, dbits);
	}
