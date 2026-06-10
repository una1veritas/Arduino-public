/*
 * memory.h
 *
 *  Created on: 2026/05/16
 *      Author: sin
 */

#ifndef EXBUSMEMORY_H_
#define EXBUSMEMORY_H_

#include "ShiftRegister.h"
#include "MCP23S08.h"

#include "MemoryDevice.h"

class ExBusMemory : public MemoryDevice{
private:
	int MEM_CE;
	int MEM_OE;
	int MEM_WE;
	int ShiftReg_CS;
	int MCP23S08_CS;

	ShiftRegister addrbus;
	MCP23S08 databus;

	//int power_en;  // power enable pin

public:

	ExBusMemory(uint8_t addrbus_cs = 8,
			uint8_t addrbus_oe = 7, uint8_t databus_cs = 9, uint8_t CE_pin = A1,
			uint8_t OE_pin = A2, uint8_t WE_pin = A0, uint8_t power = 2) :
			MEM_CE(CE_pin), MEM_OE(OE_pin), MEM_WE(WE_pin),
			addrbus( ShiftRegister(addrbus_cs, addrbus_oe, ShiftRegister::MSB_FIRST) ),
			databus( MCP23S08(databus_cs, 0) )
			//power_en(power)
			{
		deselect();
		pinMode(MEM_CE, OUTPUT);
		output_disable();
		pinMode(MEM_OE, OUTPUT);
		write_disable();
		pinMode(MEM_WE, OUTPUT);
		//power_off();
		//pinMode(power_en, OUTPUT);
	}

	inline static void delay1nop() {
		__asm__ __volatile__("nop\n\t");
	}  // about 62.5 ns at 16MHz

	inline static void delay2nops() {
		__asm__ __volatile__("nop\n\t");
		__asm__ __volatile__("nop\n\t");
	}  // about 125 ns at 16MHz

	inline static void delay4nops() {
		__asm__ __volatile__("nop\n\t");
		__asm__ __volatile__("nop\n\t");
		__asm__ __volatile__("nop\n\t");
		__asm__ __volatile__("nop\n\t");
	}  // about 250 ns at 16MHz

	// delays n + 3 clocks
	static void delayNnops(uint8_t n = 1) {
		for(; n > 0; --n)
			__asm__ __volatile__("nop\n\t");
	}

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
		//
		//power_on();
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
		//
		//power_off();
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
	inline void write_addressbus(const uint32_t &addr) {
		addrbus.write_bytes((const uint8_t*) &addr, 3); 	// LSB first
	}

//	inline void write_addressbus16(const uint16_t & addr) {
//		addrbus.write16(addr); 	// LSB first
//	}

	// two nops inserted
	uint8_t get_byte(const uint32_t &addr) const;
	void put_byte(const uint32_t &addr, const uint8_t data);

	// verify the value of the last address
	bool waitfor_write_cycle_end(const uint8_t &data, const uint16_t & count);


public:
	uint8_t read(const uint32_t &addr) const;
	uint8_t read_rom(const uint32_t & addr) const;

	void write(const uint32_t &addr, const uint8_t data);

	// Write the special six-byte code to turn off Software Data Protection.
	bool disable_SDP();
	bool enable_SDP();

	bool program_byte(const uint32_t& addr, const uint8_t data);
	//bool program_byte_100ns(const uint32_t& addr, const uint8_t data);

	bool program_page(const uint32_t &addr, const uint8_t data[], uint16_t page_size);
	//bool program_page(const uint32_t &addr, const uint8_t data[], uint16_t page_size, const uint8_t n);

private:
	void down_write(const uint32_t & base_addr, const uint32_t block_size, uint8_t val);
	void up_write(const uint32_t & base_addr, const uint32_t block_size, uint8_t val);
	uint32_t down_read_verify_write(const uint32_t & base_addr, const uint32_t block_size, uint8_t vval, uint8_t wval);
	uint32_t up_read_verify_write(const uint32_t & base_addr, const uint32_t block_size, uint8_t vval, uint8_t wval);

public:
	uint32_t sram_check(const uint32_t & start, const uint32_t & end);
};


#endif /* EXBUSMEMORY_H_ */
