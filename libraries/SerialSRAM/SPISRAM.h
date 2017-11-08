/*
 Copyright (c) 2010 by arms22 (arms22 at gmail.com)
 Microchip 23x256 SPI SRAM library for Arduino

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 23K256/640
 modified by Sin

 */

#ifndef SPISRAM_H
#define SPISRAM_H

#include <Arduino.h>
#include <SPI.h>
#include <Stream.h>

#include "extmem.h"

class SPISRAM : public Stream, public extmem {
private:
	const byte _csPin;
	const byte _buswidth;
	long _readptr, _writeptr;
//	byte clock_divider;
//	byte spi_mode;
//	byte status_cache;

// INSTRUCTION SET
	static const byte READ = 0x03; // Read data from memory
	static const byte WRITE = 0x02; // Write data to memory
	// EDIO
	// EQIO
	// RSTIO
	static const byte RDSR = 0x05; // Read Status register
	static const byte WRSR = 0x01; // Write Status register

	// STATUS REGISTER
	static const byte BYTE_MODE = 0x00;
	static const byte PAGE_MODE = 0x80;
	static const byte SEQ_MODE = 0x40;

	void set_access(const byte mode, const long & address) {
		SPI.transfer(mode);
		if (_buswidth == BUS_MBits)
			SPI.transfer(address >> 16 & 0xff);
		SPI.transfer(address >> 8 & 0xff);
		SPI.transfer(address & 0xff);
	}

	void writeStatusRegister(byte stat) {
		SPI.transfer(WRSR);
		SPI.transfer(stat);
	}

	byte readStatusRegister() {
		SPI.transfer(RDSR);
		return SPI.transfer(0);
	}

public:
	enum {
		BUS_WIDTH_23K256 = 16, // 23K256
		BUS_WIDTH_23K640 = 16,  // 23K640
		BUS_WIDTH_23LC1024 = 24, // 23A/LC1024
		BUS_KBits = 16,
		BUS_MBits = 24
	};

	SPISRAM(const byte csPin, const byte addrwidth = BUS_WIDTH_23K256);

	byte buswidth() const { return _buswidth; }

	bool init();
	inline bool begin() {
		return init();
	}
	//inline void setSPIMode();

	byte read(uint32_t address);
	byte operator[](const uint32_t & address) { return read(address); }
	void read(uint32_t address, byte *buffer, size_t size);
	void write(uint32_t address, byte data);
	void write(uint32_t address, byte *buffer, size_t size);
/*
	const long & readpoint() { return _readpt; }
	const long & writepoint() { return _writept; }
	*/
	void reset(const long & val = 0) { seek(val); }

	void seek(const long & addr) { _readptr = addr; _writeptr = addr; }
	virtual int read(void) { return read(_readptr++); }
	virtual int peek(void) { return read(_readptr); }
	virtual int available(void);
	virtual size_t write(uint8_t val) { write(_writeptr++, val); return 1; }
	virtual void flush(void) { _readptr = _writeptr; }

	inline void csLow();
	inline void csHigh();
	inline void select(void);
	inline void deselect(void);
};

#endif
