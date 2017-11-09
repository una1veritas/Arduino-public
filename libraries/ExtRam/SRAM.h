#ifndef _SRAM_H_
#define _SRAM_H_

#include <avr/io.h>
/*
 * pins must be defined
 * in sketch
 *
 */

#include "ExtRam.h"

#include "SRAM_iodef.h"

/*
#define bitset(port, bv) (port) |= (bv)
#define bitclr(port, bv) (port) &= ~(bv)
*/

class SRAM : public ExtRam {
private:
	inline void addr_set32(uint32_t addr) {
	  ADDRL = (uint8_t) addr;
	  addr >>= 8;
	  ADDRH = (uint8_t) addr;
	  addr >>= 8;
	  ADDRX &= ~ADDRX_MASK;
	  ADDRX |= ((uint8_t)addr) & ADDRX_MASK;
	}

	inline void sram_bank_select(uint8_t bk) {
	  ADDRX &= ~ADDRX_MASK;
	  ADDRX |= (bk & ADDRX_MASK);
	}

	inline void addr_set16(uint16_t addr) {
	  ADDRL = (uint8_t) addr;
	  addr >>= 8;
	  ADDRH = (uint8_t) addr;
	  addr >>= 8;
	}

public:
	void bus_setup(void);
	bool begin() { return true; }

	void enable(void);
	void disable(void);

	uint8_t read(uint32_t addr);
	void write(uint32_t addr, uint8_t data);
	void bank_select(uint8_t bk);


};

#endif /* _SRAM_H_ */
