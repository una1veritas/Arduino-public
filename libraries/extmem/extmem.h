/*
 * extmem.h
 *
 *  Created on: 2017/11/08
 *      Author: sin
 */

#ifndef EXTMEM_H_
#define EXTMEM_H_

class extmem {

public:
	extmem(void) {}
	virtual bool begin(void) = 0;

	virtual byte read(uint32_t addr) = 0;
	virtual void write(uint32_t addr, byte data) = 0;

	byte operator[](const uint32_t & addr) { return read(addr); }

	virtual void read(uint32_t addr, byte *buf, size_t n) {
		for (unsigned long i = 0; i < n; ++i) {
			buf[i] = read(addr+i);
		}
	}
	virtual void write(uint32_t addr, byte *buf, size_t n) {
		for (unsigned long i = 0; i < n; ++i) {
			write(addr+i, buf[i]);
		}
	}

};

#endif /* EXTMEM_H_ */
