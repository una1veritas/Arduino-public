/*
 * I2CEEPROM.h
 *
 *  Created on: 2017/10/19
 *      Author: sin
 */

#ifndef I2CEEPROM_H_
#define I2CEEPROM_H_

#include <Arduino.h>
#include <Wire.h>

#include "ExtRam.h"

class I2CEEPROM : public ExtRam {
private:
	uint8_t _devaddr;
	uint8_t _buswidth;
	uint8_t _txrx_stat;

private:
	static const uint8_t DEV_BASE_ADDRESS = 0x50; // 7-bit-address form

	void wire_disable(void);
	void wire_enable(void);

	uint8_t dev_addr(uint32_t addr);
	uint8_t ready(void);

public:
	enum {
		BUSWIDTH_512KBIT = 16,  // Microchip 24LC512
		BUSWIDTH_1024KBIT = 17, // AT24C1024B
	};

	I2CEEPROM(const unsigned char addr = 0, const unsigned char buswidth = BUSWIDTH_1024KBIT);

	uint8_t status(void) {
		return _txrx_stat;
	}

	void reset(void); // do I2C software reset

	bool begin(void) { return ready(); }

	uint8_t read(uint32_t addr);
	void write(uint32_t addr, uint8_t data);
	void update(uint32_t addr, uint8_t data);

	void read(uint32_t addr, uint8_t * dataptr, size_t nbytes);
	void write(uint32_t addr, uint8_t * dataptr, size_t nbytes);
	void update(uint32_t addr, uint8_t * dataptr, size_t nbytes);
/*
	int read();
	int peek();
	size_t write(uint8_t data);
	int available();
	*/
};



#endif /* I2CEEPROM_H_ */
