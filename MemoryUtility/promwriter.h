#ifndef __PROMWRITER_H__
#define __PROMWRITER_H__


#include <string.h>
#include <stdint.h>
#include <SPISRAM.h>

#include "pagearray.h"
#include "memory.h"

enum MEM_TYPE {
	UNKNOWN = 0,
	SRAM = 1,
	DRAM = 2,
	ROM = 3, 		// mask rom
	EPPROM = 4, 	// UV-EPROM
	EEPROM = 5,		// E-EPROM
	FLASH = 6,
};

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

struct MemoryInfo {
	char partname[16];
	uint32_t capacity_inbits;
	uint8_t type;
	uint16_t read_delay; 	// address or /CE to valid data delay time
	int16_t page_size;
	bool SDP;

	uint32_t size_inbytes() const {
		return capacity_inbits >> 3;
	}

	MemoryInfo & operator=(const MemoryInfo &src) {
		strncpy(partname, src.partname, 15);
		partname[15] = 0;
		capacity_inbits = src.capacity_inbits;
		type = src.type;
		page_size = src.page_size;
		return *this;
	}
};

const MemoryInfo MEMINFO_DB[] PROGMEM = {
		{ "AT28C64-15PC", 	EEPROM64KBITS, 	EEPROM, 	150,	0, 	false },
		{ "AT28C64B", 		EEPROM64KBITS, 	EEPROM, 	150, 	64,	true },
		{ "AT28C256-15", 	EEPROM256KBITS, EEPROM, 	150, 	64,	true },
		{ "AT29C256-15", 	EEPROM256KBITS, FLASH,	 	100, 	64,	true },
		{ "HN58C256", 		EEPROM256KBITS, EEPROM, 	200,	64, false },
		{ "HN58C256A", 		EEPROM256KBITS, EEPROM, 	200,	64, true },
		{ "SRAM64KBITS", 	SRAM64KBITS, 	SRAM, 		100, 	0, 	false },
		{ "SRAM256KBITS", 	SRAM64KBITS, 	SRAM, 		100, 	0, 	false },
		{ "SRAM1MBITS", 	SRAM1MBITS, 	SRAM, 		100, 	0, 	false },
		{ "SRAM4MBITS", 	SRAM4MBITS, 	SRAM, 		100, 	0, 	false },
		{ "", 0, 0, 0, },
};

void get_meminfo_byname(const char name[], MemoryInfo & minfo);
void get_meminfo_byindex(const uint8_t ix, MemoryInfo & minfo);

void list_target_types(void);


struct PROMWriter {
	uint32_t record_start_address;
	uint16_t extendedLinearAddress; 	// ihex high 16 bits of 32 bit address
	uint32_t startLinearAddress; 	// SREC start address/80386 EIP value
	// uint16_t startSegmentAddress; 	// CS:IP, reserved
	// uint16_t extSegmentAddress;		// reserved

	uint32_t totalBytesWritten;
	uint32_t errorCount;
	uint32_t checksumErrors;
	uint32_t recordCount;

	PROMWriter() {
		clear();
	}

	// clear programmer status
	void clear(void) {
		record_start_address = 0;
		extendedLinearAddress = 0;
		startLinearAddress = 0;

		totalBytesWritten = 0;
		errorCount = 0;
		checksumErrors = 0;
		recordCount = 0;
	}



};

#endif  // __PROMWRITER_H__
