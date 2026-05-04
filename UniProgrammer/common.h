/*
 * utils.h
 *
 *  Created on: 2026/04/28
 *      Author: sin
 */

#ifndef UTILS_H_
#define UTILS_H_

//enum RecordType {
//	S0 =  0x5300 ,	// S0 - Header record
//	S1 =  0x5301 ,	// S1 - 16-bit address data record
//	S2 =  0x5302 ,	// S2 - 24-bit address data record
//	S3 =  0x5303 ,	// S3 - 32-bit address data record
//	S5 =  0x5305 ,	// S5 - 16-bit record count
//	S6 =  0x5306 ,
//	S7 =  0x5307 ,
//	S8 =  0x5308 ,
//	S9 =  0x5309 ,	// S9 - 16-bit start address
//
//	I0 =  0x3a00 ,	// data
//	I1 =  0x3a01 ,	// end of file
//	I4 =  0x3a04 ,	// 32 bit extended linear address
//	I5 =  0x3a05 ,	// 32 bit Start Linear Address, execution start address
//};

struct HexRecord {
	char type[2];
	uint8_t datalength;
	uint32_t address;	// record's address offset
	uint8_t checksum;
	uint8_t data[256];

	static const uint32_t header_size(void) {
		return sizeof(type) + sizeof(datalength) + sizeof(address) + sizeof(checksum);
	}
};

struct ProgrammerStatus {
	uint32_t record_start_address;
	uint16_t extendedLinearAddress; 	// ihex high 16 bits of 32 bit address
	uint32_t startLinearAddress; 	// SREC start address/80386 EIP value
	// uint16_t startSegmentAddress; 	// CS:IP, reserved
	// uint16_t extSegmentAddress;		// reserved

	uint32_t start_ix;

	uint32_t totalBytesWritten;
	uint32_t recordCount;
	uint32_t errorCount;
	uint32_t checksumErrors;

	ProgrammerStatus(void) {
		clear();
	}

	// clear programmer status
	void clear(void) {
		record_start_address = 0;
		extendedLinearAddress = 0;
		startLinearAddress = 0;

		start_ix = 0;

		totalBytesWritten = 0;
		recordCount = 0;
		errorCount = 0;
		checksumErrors = 0;
	}

};

extern ProgrammerStatus pgmstatus;

extern char buf128[128];

int Serialsnprint(char buf[], unsigned int n, const char *format, ...);
int Serialsnprintln(char buf[], unsigned int n, const char *format, ...);

uint8_t hexToUint8(String hex, int startpos = 0);

uint8_t auxmem_write(const uint32_t addr32, const uint8_t val8);
uint8_t auxmem_read(const uint32_t addr32);

//void verifyData();
void clear_pgmstatus();

void printWelcome();

void printHelp();

#endif /* UTILS_H_ */
