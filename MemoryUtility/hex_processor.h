/*
 * hexline_processor.h
 *
 *  Created on: 2026/04/28
 *      Author: sin
 */

#ifndef HEXLINE_PROCESSOR_H_
#define HEXLINE_PROCESSOR_H_

#include "promwriter.h"

// common definitions

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

uint8_t hexToUint8(const char * str,  const uint16_t & startpos);

// format specific definitions

// Intel HEX Record Types
typedef enum {
	IHEX_DATA 					= 0x00,
	IHEX_END_OF_FILE 			= 0x01,
	IHEX_EXTENDED_LINEAR_ADDR 	= 0x04,
	IHEX_START_LINEAR_ADDR 		= 0x05,
} IHEX_RecordType;

// S19 record types
typedef enum {
  SREC_HEADER = 0,      // S0 - Header record
  SREC_DATA_16 = 1,     // S1 - 16-bit address data record
  SREC_DATA_24 = 2,     // S2 - 24-bit address data record
  SREC_DATA_32 = 3,     // S3 - 32-bit address data record
  SREC_COUNT_16 = 5,    // S5 - 16-bit record count
  SREC_COUNT_24 = 6,    // S6 - 24-bit record count (unofficial)
  SREC_START_32 = 7,    // S7 - 32-bit start address
  SREC_START_24 = 8,    // S8 - 24-bit start address
  SREC_START_16 = 9     // S9 - 16-bit start address
} SREC_RecordType;

extern PageArray pagearray;
extern PROMWriter promwriter;

void processiHexRecord(String line, HexRecord & hexrecord);
void handleiHexDataRecord(const HexRecord & hexrecord);
void handleiHexExtendedLinearAddress(HexRecord & hexrecord);
void handleiHexStartLinearAddress(HexRecord & hexrecord) ;
void handleiHexEndOfFile(HexRecord & hexrecordp);
bool validateiHexChecksum(HexRecord & xrecordp);

uint8_t calcS19Checksum(const String & line);

boolean processS19Record(const String& line, HexRecord & record);
boolean processS19Header(const HexRecord & record); //uint8_t byteCount);
boolean processS19DataRecord(const HexRecord & record);
boolean processS19StartAddress(const HexRecord & record);

#endif /* HEXLINE_PROCESSOR_H_ */
