/*
 Copyright (c) 2010 by arms22 (arms22 at gmail.com)
 Microchip 23x256 SPI SRAM library for Arduino

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <Arduino.h>
#include <SPI.h>

#include "SPISRAM.h"

SPISRAM::SPISRAM(const byte csPin, const byte addr_width) :
		_csPin(csPin), _buswidth(addr_width), _readptr(0), _writeptr(0) {
}

bool SPISRAM::init() {
	pinMode(_csPin, OUTPUT);
	csHigh();
	//
	select();
	writeStatusRegister(SEQ_MODE);
	deselect();
	select();
	byte t = readStatusRegister();
	deselect();
	_writeptr = 0;
	_readptr = 0;
	return (t == SEQ_MODE);
}

byte SPISRAM::read(uint32_t address) {
	byte data;
	select();
	set_access(READ, address);
	data = SPI.transfer(0);
	deselect();
	return data;
}

void SPISRAM::read(uint32_t address, byte *buffer, size_t size) {
	select();
//	SPI.transfer(WRSR);
//	SPI.transfer(SEQ_MODE);
	//
	byte * p = buffer;
	set_access(READ, address);
	for (unsigned int i = 0; i < size; i++)
		*p++ = SPI.transfer(0);
	deselect();
}

void SPISRAM::write(uint32_t address, byte data) {
	select();
	set_access(WRITE, address);
	SPI.transfer(data);
	deselect();
}

void SPISRAM::write(uint32_t address, byte *buffer, size_t size) {
	select();
//	SPI.transfer(WRSR);
//	SPI.transfer(SEQ_MODE);
	//
	set_access(WRITE, address);
	for (unsigned int i = 0; i < size; i++)
		SPI.transfer(*buffer++);
	deselect();
}


int SPISRAM::available(void) {
	if (_buswidth == 24 ) {
		if ( _writeptr >= _readptr )
			return _writeptr - _readptr;
		return _writeptr + 0x1000000 - _readptr;
	} else {
	//	if (_buswidth == 16 ) {
		if ( _writeptr >= _readptr )
			return _writeptr - _readptr;
		return _writeptr + 0x10000 - _readptr;
	}
}

/*
void SPISRAM::setSPIMode(void) {
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setDataMode(SPI_MODE0);
}
*/
void SPISRAM::csLow() {
	digitalWrite(_csPin, LOW);
}

void SPISRAM::csHigh() {
	digitalWrite(_csPin, HIGH);
}

void SPISRAM::select(void) {
	//SPI.beginTransaction(SPISettings(20000000,MSBFIRST,SPI_MODE0));
	SPI.beginTransaction(SPISettings(20000000,MSBFIRST,SPI_MODE0));
	csLow();
	//setSPIMode();
}

void SPISRAM::deselect(void) {
	csHigh();
	SPI.endTransaction();
}
