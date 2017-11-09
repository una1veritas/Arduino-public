/*
 * ExtMemory.h
 *
 *  Created on: 2017/11/08
 *      Author: sin
 */

#ifndef EXTMEMORY_H_
#define EXTMEMORY_H_

class ExtMemory {
	virtual byte bus_setup() { return 1; }
	virtual byte bus_release() { return 1; }

	virtual byte read(unsigned int addr);
	virtual byte write(unsigned int addr, unsigned char data);
};

#endif /* EXTMEMORY_H_ */
