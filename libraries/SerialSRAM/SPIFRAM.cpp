/*
 Copyright (c) 2010 by arms22 (arms22 at gmail.com)
 Microchip 23x256 SPI SRAM library for Arduino

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <Arduino.h>
#include <SPI.h>

#include "SPIFRAM.h"

void SPIFRAM::init() {
	pinMode(_cspin, OUTPUT);
	csHigh();
}

byte SPIFRAM::status(void) {
	byte stat;
	select();
	SPI.transfer(FM25_RDSR);
	stat = SPI.transfer(0);
	deselect();
	return stat;
}

void SPIFRAM::status(byte val) {
	select();
	SPI.transfer(FM25_WRSR);
	SPI.transfer(val);
	deselect();
}

void SPIFRAM::WREN() {
	select();
	SPI.transfer(FM25_WREN);
	deselect();
}

size_t SPIFRAM::read(const uint32_t & addr, byte *buffer, size_t size) {
	select();
	SPI.transfer(FM25_READ);
	if ( _buswidth > 16 )
		SPI.transfer(addr>>16&0xff);
	SPI.transfer( addr >>8 & 0xff);
	SPI.transfer( addr & 0xff);
	for (uint16_t i = 0; i < size; i++)
		*buffer++ = SPI.transfer(0);
	deselect();
	return size;
}

byte SPIFRAM::read(const uint32_t & addr) {
	byte data;
	read(addr,(byte*) &data, 1);
	return data;
}

size_t SPIFRAM::write(const uint32_t & addr, byte *buffer, const size_t size) {
	select();
	SPI.transfer(FM25_WREN); // write must be enabled
	csHigh();
	csLow();
	SPI.transfer(FM25_WRITE);
	if ( _buswidth > 16 )
		SPI.transfer(addr>>16&0xff);
	SPI.transfer( addr >>8 & 0xff);
	SPI.transfer( addr & 0xff);
	for (uint16_t i = 0; i < size; i++)
		SPI.transfer(*buffer++);
	deselect();
	return size;
}

size_t SPIFRAM::write(const uint32_t & addr, byte data) {
	write(addr, &data, 1);
	return 1;
}

