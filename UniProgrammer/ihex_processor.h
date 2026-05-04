/*
 * ihex_processor.h
 *
 *  Created on: 2026/04/28
 *      Author: sin
 */

#ifndef IHEX_PROCESSOR_H_
#define IHEX_PROCESSOR_H_

#include "common.h"

// Intel HEX Record Types
typedef enum {
	IHEX_DATA 					= 0x00,
	IHEX_END_OF_FILE 			= 0x01,
	IHEX_EXTENDED_LINEAR_ADDR 	= 0x04,
	IHEX_START_LINEAR_ADDR 		= 0x05,
} IHEX_RecordType;

extern unsigned long checksumErrors;
extern unsigned int extendedLinearAddress;
extern unsigned long bytesWritten;

void processIHexRecord(String line, HexRecord & hexrecord);
void handleDataRecord(const HexRecord & hexrecord);
void handleExtendedLinearAddress(HexRecord & hexrecord);
void handleStartLinearAddress(HexRecord & hexrecord) ;
void handleEndOfFile(HexRecord & hexrecordp);
bool validateChecksum(HexRecord & xrecordp);

#endif /* IHEX_PROCESSOR_H_ */
