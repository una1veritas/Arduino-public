// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _libraries_SRAM_H_
#define _libraries_SRAM_H_

#include "Arduino.h"

#define set_bits_macro(port,mask) ((port) |= (mask))

class SRAM {
	uint8_t _bank;

	struct IOPortPin {
		volatile uint8_t * out;
		uint8_t mask;
	};

	IOPortPin LATCH_OE = { &PORTD, (1 << 7) },
			LATCH_CLK = { &PORTL, (1 << 7) },
			SRAM_CS = { &PORTG, (1 << 2) },
			SRAM_OE = { &PORTG, (1 << 1) },
			SRAM_WE = { &PORTG, (1 << 0) },
			SRAM_ADDRL = { &PORTA, 0xff },
			SRAM_ADDRH = { &PORTC, 0xff },
			SRAM_ADDRBANK = { &PORTL, (1 << 0) },
			SRAM_DATA = { &PORTA, 0xff };

public:
	SRAM() {
		//SRAM_WE.port = &PORTG; SRAM_WE.pin = 1<<0;
		//SRAM_OE.port = &PORTG; SRAM_OE.pin = 1<<1;
		//SRAM_CS.port = &PORTG; SRAM_CS.pin = 1<<2;

		//LATCH_OE.port = &PORTD; LATCH_OE.pin = 1<<7 ;
		//LATCH_CLK.port = &PORTL; LATCH_CLK.pin = 1<<7;
		_bank = 0;
	}

	bool begin() {
		*(SRAM_ADDRL.out-1) |= SRAM_ADDRL.mask; //DDRA = 0xff;   	// output for default
		*(SRAM_ADDRH.out-1) |= SRAM_ADDRH.mask; //DDRC = 0xff;   	// always output
		*(SRAM_ADDRBANK.out-1) |= SRAM_ADDRBANK.mask; //DDRL |= (1<<0); // A16, always output

		*(SRAM_CS.out-1) |= SRAM_CS.mask;
		*SRAM_CS.out |= SRAM_CS.mask;
		*(SRAM_OE.out-1) |= SRAM_OE.mask;
		*SRAM_OE.out |= SRAM_OE.mask;
		*(SRAM_WE.out-1) |= SRAM_WE.mask;
		*SRAM_WE.out |= SRAM_WE.mask;

		*(LATCH_OE.out-1) |= LATCH_OE.mask; // latch ~OE
		*(LATCH_CLK.out-1) |= LATCH_CLK.mask; // latch ~CLK

		*LATCH_OE.out &= ~LATCH_OE.mask; // LATCH_OE to LOW permanently
		*LATCH_CLK.out &= ~LATCH_CLK.mask;

		return true;
	}

	void enable() {
		//*LATCH_OE.out &= ~LATCH_OE.mask; // latch ~OE low
		*SRAM_CS.out &= ~SRAM_CS.mask; //PORTG &= ~(1<<2);
	}

	void disable() {
		//*LATCH_OE.out |= LATCH_OE.mask; // latch ~OE high
		*SRAM_CS.out |= SRAM_CS.mask; //PORTG |= (1<<2);
	}

	void bank(const uint8_t bkn) { _bank = bkn; }

	void write(uint16_t addr, byte val) {
		*SRAM_ADDRL.out = addr; //PORTA = addr & 0xff;
		*SRAM_ADDRH.out = addr>>8; //PORTC = addr>>8 & 0xff;
		*SRAM_ADDRBANK.out &= ~SRAM_ADDRBANK.mask;
		*SRAM_ADDRBANK.out |= _bank & SRAM_ADDRBANK.mask;

		*LATCH_CLK.out &= ~LATCH_CLK.mask;
		*LATCH_CLK.out |= LATCH_CLK.mask;

		*(SRAM_DATA.out) = val;
		*SRAM_WE.out &= ~SRAM_WE.mask; // PORTG &= ~(1<<0);
		*SRAM_WE.out |= SRAM_WE.mask; // PORTG |= (1<<0); //
	}

	byte read(uint16_t addr) {
		byte val;
		*SRAM_ADDRL.out = addr; //PORTA = addr & 0xff;
		*SRAM_ADDRH.out = addr>>8; //PORTC = addr>>8 & 0xff;
		*SRAM_ADDRBANK.out &= ~SRAM_ADDRBANK.mask;
		*SRAM_ADDRBANK.out |= _bank & SRAM_ADDRBANK.mask;

		*LATCH_CLK.out &= ~LATCH_CLK.mask;
		*LATCH_CLK.out |= LATCH_CLK.mask;

		*(SRAM_DATA.out-1) = 0x00;
		*SRAM_OE.out &= ~SRAM_OE.mask;
		__asm__ __volatile__ ("nop");
		val = *(SRAM_DATA.out-2);
		*SRAM_OE.out |= SRAM_OE.mask;
		*(SRAM_DATA.out-1) = 0xff;
		return val;
	}
};

//Do not add code below this line
#endif /* _libraries_SRAM_H_ */
