/*
 * srec_processor.h
 *
 *  Created on: 2026/04/28
 *      Author: sin
 */

#ifndef SREC_PROCESSOR_H_
#define SREC_PROCESSOR_H_

#include "common.h"

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


boolean processS19Record(const String& record,HexRecord & hexrecord);
boolean verifyChecksum(const String& record, HexRecord & hexrecord); //uint8_t byteCount);
boolean processHeader(const String& record, HexRecord & hexrecord); //uint8_t byteCount);
boolean processDataRecord(const String& record, HexRecord & hexrecord /* uint8_t byteCount*/, uint8_t addressBytes);
boolean processStartAddress(const String& record, uint8_t addressBytes);

#endif /* SREC_PROCESSOR_H_ */
