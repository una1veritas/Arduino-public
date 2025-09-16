/*
 * MCP23S1x.h
 *
 *  Created on: 2014/04/19
 *      Author: sin
 */

#ifndef MCP23S1X_H_
#define MCP23S1X_H_

#include <Arduino.h>
#include <SPI.h>

class MCP23S1x {
private:
	const uint8_t pin_CS;
	const uint8_t contbyte;

	void init(void) {
		pinMode(pin_CS, OUTPUT);
		digitalWrite(pin_CS, HIGH);
		write(IOCON, 0x00);
	}

public:
	static const uint8_t CONTROL_FIXEDADDR = 0x40;
	static const uint8_t CONTROL_READ = 0x01;
	static const uint8_t CONTROL_WRITE = 0x00;
	static const uint8_t CONTROL_USERADDRMASK = 0x07 << 1;

	static const uint16_t IODIR_INPUT = 0xff;
	static const uint16_t IODIR_OUTPUT = 0x00;

	// assumes BANK = 0
	enum {
		IODIRA = 0x00,
		IODIRB = 0x01,
		IOCON = 0x0A,
		GPPUA = 0x0C,
		GPPUB = 0x0D,
		GPIOA = 0x12,
		GPIOB = 0x13,
		OLATA = 0x14,
		OLATB = 0x15,
	};
public:

	MCP23S1x(const uint8_t cs, const uint8_t addr = 0x07) :
		pin_CS(cs), contbyte(CONTROL_FIXEDADDR | (CONTROL_USERADDRMASK & (addr<<1)) ) { }

	inline void begin() { init(); }
	inline void select() { digitalWrite(pin_CS, LOW); SPI.setClockDivider(SPI_CLOCK_DIV4); SPI.setDataMode(SPI_MODE0); }
	inline void deselect() { digitalWrite(pin_CS, HIGH); }

	uint8_t read(const uint8_t reg);
	uint8_t write(const uint8_t reg, const uint8_t val);
/*
	uint16_t read16(const uint8_t reg);
	uint16_t write16(const uint8_t reg, const uint16_t val);
*/
	void portAMode(const uint8_t dbits);
	void portBMode(const uint8_t dbits);
};

#endif /* MCP23S1X_H_ */
