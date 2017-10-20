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
	const unsigned char _addr;
	uint8_t _rx_stat, _tx_stat;

private:
	static const byte DEV_ADDRESS = 0x50; // 7 bits followed by R/W bit

public:
	EEPROM_I2C(const unsigned char addr = 0) : _addr(addr), _rx_stat(0), _tx_stat(0) {
	}

	uint8_t reset(void) {
		_rx_stat = 0;
		_tx_stat = 0;
		return 0;
	}

	uint8_t begin(void) {
		return reset();
	}

	uint8_t read(uint16_t addr);
	uint8_t write(uint16_t addr, uint8_t data);
};



#endif /* SERIALEEPROM_H_ */
