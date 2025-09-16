/*
 * ExtRam.h
 *
 *  Created on: 2017/11/08
 *      Author: sin
 */

#ifndef EXTRAM_H_
#define EXTRAM_H_

#include <stddef.h>
#include <inttypes.h>

class ExtRam {

public:
	ExtRam(void) {}
	virtual bool begin(void) = 0;

	virtual uint8_t read(uint32_t addr) = 0;
	virtual void write(uint32_t addr, uint8_t data) = 0;

	uint8_t operator[](const uint32_t & addr) { return read(addr); }

	virtual void read(uint32_t addr, uint8_t *buf, size_t n) {
		for (uint32_t i = 0; i < n; ++i) {
			buf[i] = read(addr+i);
		}
	}
	virtual void write(uint32_t addr, uint8_t *buf, size_t n) {
		for (uint32_t i = 0; i < n; ++i) {
			write(addr+i, buf[i]);
		}
	}

};

#endif /* EXTMEM_H_ */
