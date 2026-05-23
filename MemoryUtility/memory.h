/*
 * memory.h
 *
 *  Created on: 2026/05/16
 *      Author: sin
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "ShiftRegister.h"
#include "MCP23S08.h"

class Memory {
private:
	int MEM_CE;
	int MEM_OE;
	int MEM_WE;
	int ShiftReg_CS;
	int MCP23S08_CS;

	ShiftRegister addrbus;
	MCP23S08 databus;
	uint32_t capacity_bits;

public:
	enum CAPACITY_INBITS {
		SRAM32KBITS = 0x8000UL,   // 4k bytes
		SRAM64KBITS = 0x10000UL,   // 8k bytes
		SRAM128KBITS = 0x20000UL,   // 16k bytes
		SRAM832 = 0x40000UL,   // 32kbytes/256k bits
		SRAM256KBITS = 0x40000UL,   // 32kbytes/256k bits
		SRAM512KBITS = 0x80000UL,   // 64kbytes/512kbits
		SRAM1MBITS = 0x100000UL,   // 64kbytes/512kbits
		SRAM8512 = 0x400000UL,   // 64kbytes/512kbits
		SRAM4MBITS = 0x400000UL,   // 64kbytes/512kbits

		EEPROM64KBITS = 0x10000UL,
		EEPROM256KBITS = 0x40000UL,
	};

	Memory(const uint32_t &capabits, uint8_t addrbus_cs = 8,
			uint8_t addrbus_oe = 7, uint8_t databus_cs = 9, uint8_t CE_pin = A1,
			uint8_t OE_pin = A2, uint8_t WE_pin = A0) :
			MEM_CE(CE_pin), MEM_OE(OE_pin), MEM_WE(WE_pin), addrbus(
					ShiftRegister(addrbus_cs, addrbus_oe,
							ShiftRegister::MSB_FIRST)), databus(
					MCP23S08(databus_cs, 0)), capacity_bits(capabits) {
		deselect();
		pinMode(MEM_CE, OUTPUT);
		output_disable();
		pinMode(MEM_OE, OUTPUT);
		write_disable();
		pinMode(MEM_WE, OUTPUT);
	}

	inline static void delay1clock() {
		__asm__ __volatile__("nop\n\t");
	}  // about 62.7 ns at 16MHz

	   // chip select/enable (/CE)
	inline void select() {
		digitalWrite(MEM_CE, LOW);
	}

	inline void deselect() {
		digitalWrite(MEM_CE, HIGH);
	}
	// Output enable (/OE)
	inline void output_enable() {
		digitalWrite(MEM_OE, LOW);
	}
	inline void output_disable() {
		digitalWrite(MEM_OE, HIGH);
	}
	// Write enable (/WE)
	inline void write_enable() {
		digitalWrite(MEM_WE, LOW);
	}
	inline void write_disable() {
		digitalWrite(MEM_WE, HIGH);
	}

// must be called after SPI has begun (otherwise hangs up)
	inline void begin() {
		deselect();
		pinMode(MEM_CE, OUTPUT);
		output_disable();
		pinMode(MEM_OE, OUTPUT);
		write_disable();
		pinMode(MEM_WE, OUTPUT);
		addrbus.begin();
		addrbus.output_enable();
		databus.begin();
		set_databus_mode(INPUT);
	}

	// inactivate
	inline void end() {
		pinMode(MEM_OE, INPUT);
		pinMode(MEM_WE, INPUT);
		digitalWrite(MEM_CE, HIGH);
		pinMode(MEM_CE, INPUT);
		addrbus.output_disable();
		databus.set_gpio_input();
		databus.disable_gpio_pullup();
	}

	// capacity size in byte/mqximum address
	inline uint32_t size() const {
		return capacity_bits >> 3;
	}

private:
	void set_databus_mode(const uint8_t inout);

	inline uint8_t read_databus() {
		return databus.read();
	}

	inline void write_databus(const uint8_t val) {
		databus.write(val);
	}

	// use only 24 bits
	inline void set_address(const uint32_t &addr) {
		addrbus.write_bytes((const uint8_t*) &addr, 3);
	}

	void put_byte(const uint32_t &addr, const uint8_t data);

	bool waitfor_write_cycle_end(const uint8_t &data, uint16_t count);

public:
	uint8_t read(const uint32_t &addr);
	uint8_t write(const uint32_t &addr, const uint8_t data);

	// Write the special six-byte code to turn off Software Data Protection.
	bool disable_SDP();
	bool enable_SDP();

	bool program_byte(const uint32_t &addr, const uint8_t data);
	bool program_page(const uint32_t &addr, const uint8_t data[],
			uint16_t page_size);

};


#endif /* MEMORY_H_ */
