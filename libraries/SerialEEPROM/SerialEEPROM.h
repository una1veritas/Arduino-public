/*
 * SerialEEPROM.h
 *
 *  Created on: 2017/10/19
 *      Author: sin
 */

#ifndef SERIALEEPROM_H_
#define SERIALEEPROM_H_

#include <Arduino.h>
#include <Wire.h>

class EEPROM_I2C {
private:
	unsigned char _devaddr;
	unsigned char _buswidth;
	uint8_t _txrx_stat;

private:
	static const byte DEV_BASE_ADDRESS = 0x50; // 7-bit-address form

	void wire_disable(void);
	void wire_enable(void);

	byte dev_addr(uint32_t addr);
	byte ready(void);

public:
	enum {
		BUSWIDTH_512KBIT = 16,  // Microchip 24LC512
		BUSWIDTH_1024KBIT = 17, // AT24C1024B
	};

	EEPROM_I2C(const unsigned char addr = 0, const unsigned char buswidth = BUSWIDTH_1024KBIT);

	byte status(void) {
		return _txrx_stat;
	}

	void reset(void); // do I2C software reset

	byte begin(void) { return ready(); }

	byte read(uint32_t addr);
	byte write(uint32_t addr, byte data);
	byte update(uint32_t addr, byte data);

	byte * read(uint32_t addr, byte * dataptr, uint16_t nbytes);
	byte * write(uint32_t addr, byte * dataptr, uint16_t nbytes);
	byte * update(uint32_t addr, byte * dataptr, uint16_t nbytes);
};



#endif /* SERIALEEPROM_H_ */
