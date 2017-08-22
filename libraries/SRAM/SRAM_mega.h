// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _libraries_SRAM_H_
#define _libraries_SRAM_H_

#include "Arduino.h"

class SRAM {
	struct {
		volatile byte * port;
		byte pin;
	} SRAM_WE, SRAM_OE, SRAM_CS, LATCH_OE, LATCH_CLK;
public:
	SRAM() {
		SRAM_WE.port = PORTG; SRAM_WE.pin = 1<<0;
		SRAM_OE.port = PORTG; SRAM_OE.pin = 1<<1;
		SRAM_CS.port = PORTG; SRAM_CS.pin = 1<<2;

		LATCH_OE.port = PORTD; LATCH_OE.pin = 1<<7 ;
		LATCH_CLK.port = PORTG; LATCH_CLK.pin = 1<<7;
	}

	bool begin() {
		DDRA = 0xFF;   // A0 - A7 / D0 - D7, output in default
		DDRC = 0xFF;   // A8 - A15
		DDRL |= 0x01;  // A16

		DDRG |= 0x07;  // WR, RD, ALE -> WE, OE, CS
		DDRD |= 0x80;  // hc574 OE
		DDRL |= 0x80;  // hc574 CLK

		return true;
	}

	void enable() {
		*SRAM_CS.port &= ~SRAM_CS.pin;
	}

	void disable() {
		*SRAM_CS.port |= SRAM_CS.pin;
	}

	void write(uint16_t addr, uint8_t val) {
		uint8_t * addr8 =(uint8_t *) &addr;

		// address bus makeup
		PORTA = addr8;
		PORTC = addr8+1;
		PORTL &= ~1;

		PORTL &= ~0x80;
		PORTL |= 0x80;
		PORTD &= ~0x80;
		// address bus ready

		PORTA = val;
		*SRAM_WE.port &= ~SRAM_WE.pin;
		__asm__ __volatile__ ("nop");
		*SRAM_WE.port |= SRAM_WE.pin;
	}

	byte read(uint16_t addr) {
		uint8_t * addr8 =(uint8_t *) &addr;
		byte val;

		// address bus makeup
		PORTA = addr8;
		PORTC = addr8+1;
		PORTL &= ~1;

		*LATCH_CLK.port &= ~LATCH_CLK.pin;
		*LATCH_CLK.port |= LATCH_CLK.pin;
		*LATCH_OE.port &= ~LATCH_OE.pin;
		// address bus ready

		DDRA = 0x00;
		*SRAM_OE.port &= ~SRAM_OE.pin;
		val = PINA;
		*SRAM_OE.port |= SRAM_OE.pin;
	    DDRA = 0xff;

		return val;
	}
};

//Do not add code below this line
#endif /* _libraries_SRAM_H_ */
