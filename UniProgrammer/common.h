/*
 * utils.h
 *
 *  Created on: 2026/04/28
 *      Author: sin
 */

#ifndef UTILS_H_
#define UTILS_H_

typedef struct HexRecord {
	uint8_t start_code;
	uint8_t type;
	uint8_t byteCount;
	uint8_t datalength;
	uint8_t data[256];
	uint32_t address;	// address offset
	uint8_t checksum;
} HexRecord;

typedef struct AddressContext {
	uint16_t startSegmentAddress; 	// CS:IP
	uint16_t extSegmentAddress;
	uint16_t extLinearAddress;		// high 16 bits
	uint32_t startLinearAddress; 	// 80386 EIP value
} AddressContext;

extern AddressContext addrcontext;

typedef struct {
	uint32_t totalBytesWritten;
	uint32_t recordCount;
	uint32_t errorCount;
	uint32_t checksumErrors;
	boolean loadInProgress;
} writer_status;

extern writer_status wstatus;

void clearWriterStatus();
void printWriterStatus();

int Serialsnprint(const char *format, ...);
int Serialsnprintln(const char *format, ...);

uint8_t hexToUint8(String hex, int startpos = 0);

uint8_t write(const uint32_t addr32, const uint8_t val8);

void verifyData();

void printWelcome();

void printHelp();

#endif /* UTILS_H_ */
