/*
 * uni_programmer.h
 *
 *  Created on: 2026/05/07
 *      Author: sin
 */

#ifndef UNI_PROGRAMMER_H_
#define UNI_PROGRAMMER_H_

#include <inttypes.h>

struct RandomAccessMemory {
	virtual uint8_t read_byte(const uint32_t & address) {
		uint8_t val;
		write_disable();
		set_databus_input();
		set_address(address);
		chip_enable();
		output_enable();
		wait_ns(50);
		val = get_databus();
		output_disable();
		chip_disable();
		return val;
	}

	virtual uint8_t write_byte(const uint32_t & address, const uint8_t value) {
		output_disable();
		write_disable();
		set_databus_output();
		set_databus(value);
		set_address(address);
		chip_enable();
		write_enable();
		wait_ns(50);
		write_disable();
		chip_disable();
		return value;
	}

	virtual uint16_t read_block(const uint32_t & address, const uint8_t array[], const uint16_t n) = 0;
	virtual uint16_t write_block(const uint32_t & address, const uint8_t array[], const uint16_t n) = 0;

	void chip_enable();
	void chip_disable();
	void chip_enable2();
	void chip_disable2();

	void write_enable();
	void write_disable();
	void output_enable();
	void output_disable();

	void wait_ns(const uint16_t &);
	void wait_ms(const uint16_t &);
};

struct EEPROM : public Memory {
	enum TYPE_IDS {
		NOTSPECIFIED = 0,

		EEPROM_AT28C64 = 1,
		EEPROM_AT28C64B,
		EEPROM_AT28C256,

		EEPROM_HN58C256,

		UNKNOWN = 255,
	};

	uint8_t type_id;

	struct ChipType {
		uint8_t type_id;
		char name[32];
		uint8_t addrbus_width;
		uint8_t databus_width;
		uint16_t page_size;
	};

	static const ChipType chiptype_db[8] {
			{ NOTSPECIFIED, "Not Specified", 16, 8, 64 },
			{ EEPROM_AT28C64, "AT28C64", 13, 8, 0 },
			{ EEPROM_AT28C64B, "AT28C64B", 13, 8, 64 },
			{ EEPROM_AT28C256, "AT28C256", 15, 8, 64 },
			{ EEPROM_HN58C256, "HN58C256", 15, 8, 64 },
			{ UNKNOWN, "Unknown chip", 0, 0, 0 },
			{ 0, "", 0, 0, 0},
	};

	EEPROM(const uint8_t chiptypeid = NOTSPECIFIED) : type_id(chiptypeid) {}

	uint8_t read(const uint32_t & address) {

	}

	uint8_t write(const uint32_t & address, const uint8_t value) {

	}

	uint16_t write_block(const uint32_t & address, const uint8_t array[], const uint16_t n) {

	}

};

#endif /* UNI_PROGRAMMER_H_ */
