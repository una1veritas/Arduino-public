/*
 * ihex_processor.cpp
 *
 *  Created on: 2026/04/28
 *      Author: sin
 */

#include <Arduino.h>

#include "ihex_processor.h"
#include "common.h"

// Global variables
//unsigned int extendedLinearAddress = 0;
//unsigned long bytesWritten = 0;
//unsigned long checksumErrors = 0;

/*
 * Process a single Intel HEX record
 * Format: :LLAAAATTDD...CC
 * LL = Byte count (2 hex chars)
 * AAAA = Address (4 hex chars)
 * TT = Record type (2 hex chars)
 * DD... = Data bytes
 * CC = Checksum (2 hex chars)
 */

void processIHexRecord(String line, HexRecord &record) {
	// set record type
	record.type[0] = line[0];
	record.type[1] = line[8];
//	Serial.print("type = ");
//	Serial.println(record.type, HEX);

	// Validate minimum length (:LLAAAATTCC = 11 chars)
	if (line.length() < 11) {
		pgmstatus.errorCount += 1;
		Serial.print("ERROR: Invalid line length: ");
		Serial.println(line.length(), DEC);
		return;
	}

	// Extract byte count
	//uint8_t byteCount
	record.datalength = hexToUint8(line, 1);

	// Validate line length: :LL + AAAA + TT + DD*2 + CC
	int expectedLength = 11 + (record.datalength * 2);
	if (line.length() != expectedLength) {
		// Serial.print("ERROR: Line length mismatch. Expected: ");
		// Serial.print(expectedLength);
		// Serial.print(" Got: ");
		// Serial.println(line.length());
		pgmstatus.errorCount += 1;
		Serial.print(F("ERROR: Line length mismatch. Expected: "));
		Serial.print(expectedLength);
		Serial.print(F(" Got: "));
		Serial.println(line.length());
		for (int i = 0; i < line.length(); ++i) {
			if (isprint(line[i])) {
				Serial.print(line[i]);
			} else {
				Serial.print('[');
				Serial.print(line[i] >> 4, HEX);
				Serial.print(line[i] & 0x0f, HEX);
				Serial.print(']');
			}
		}
		Serial.println();
		return;
	}

	// Extract address (low 16 bit)
	//  uint16_t address = (hexToUint8(line.substring(3, 5)) << 8) |
	//                     hexToUint8(line.substring(5, 7));
	record.address = pgmstatus.extendedLinearAddress;
	record.address <<= 16;
	record.address |= (uint16_t(hexToUint8(line, 3)) << 8) | hexToUint8(line, 5);

	// Extract data bytes
	//uint8_t data[256];
	for (int i = 0; i < record.datalength; i++) {
		record.data[i] = hexToUint8(line, 9 + (i << 1));
	}

	// Extract and validate checksum
	//uint8_t checksum =
	record.checksum = hexToUint8(line, 9 + (record.datalength << 1));

	if (!validateChecksum(record)) {
		Serial.println(F("ERROR: Checksum validation failed for line:"));
		Serial.println(line);
		pgmstatus.checksumErrors++;
		return;
	}

	pgmstatus.recordCount++;

	// Handle record types
	switch ((char) record.type[1]) {
	case '0': //IHEX_DATA:
		handleDataRecord(record);
		break;

	case '4': //IHEX_EXTENDED_LINEAR_ADDR:
		handleExtendedLinearAddress(record);
		break;

	case '5': //IHEX_START_LINEAR_ADDR:
		handleStartLinearAddress(record);
		break;

	case '1': //IHEX_END_OF_FILE:
		handleEndOfFile(record);
		break;

	default:
		Serial.print(F("WARNING: Unknown record type: 0x0"));
		Serial.print((char) record.type[1]);
		Serial.println(F(", ignored."));
		break;
	}
}

/*
 * Handle Data Record (Type 0x00)
 * Writes data to EEPROM at the computed address
 */
void handleDataRecord(const HexRecord & record) {
	// Check if address is within memory bounds
	if ( record.address +  record.datalength > 0x20000) {
		pgmstatus.errorCount += 1;
		Serial.print(F("ERROR: Address out of memory bounds: "));
		Serialsnprintln(buf128, 127, "0x%04x (size: 0x%04x)",  record.address, 0x20000);
		return;
	}

	// Write data to auxiliary memory
	uint8_t * ptr = (uint8_t *) & record;
	for (uint32_t ix = 0; ix < record.header_size(); ++ix, ++ptr) {
		auxsram.write(pgmstatus.start_ix + ix, *ptr);
	}
	for (uint32_t ix = 0; ix < record.datalength; ++ix, ++ptr) {
		auxsram.write(pgmstatus.start_ix + record.header_size() + ix, *ptr);
	}
	pgmstatus.start_ix += record.header_size() + record.datalength;
	auxsram.write(pgmstatus.start_ix, 0x00);

	pgmstatus.totalBytesWritten +=  record.datalength;

	Serial.print(F("OK: I DATA "));
	Serial.print( record.address, HEX);
	Serial.print(F(" -- "));
	Serial.println( record.address +  record.datalength - 1, HEX);

	pgmstatus.totalBytesWritten +=  record.datalength;
}

/*
 * Handle Extended Linear Address Record (Type 0x04)
 * Sets the upper 16 bits of the address
 */
void handleExtendedLinearAddress(HexRecord &hexrecord) {
	if (hexrecord.datalength != 2) {
		pgmstatus.errorCount += 1;
		Serialsnprintln(buf128, 127,
				"ERROR: Extended Linear Address record must have 2 bytes, got: %d",
				hexrecord.datalength);
		return;
	}

	pgmstatus.extendedLinearAddress = (uint16_t(hexrecord.data[0]) << 8) | hexrecord.data[1];

	Serial.print(F("OK: Extended Linear Address (high 16bit) set to 0x"));
	Serial.println(pgmstatus.extendedLinearAddress, HEX);
}

/*
 * Handle Start Linear Address Record (Type 0x05)
 * Optional: used for execution start address (informational)
 */
void handleStartLinearAddress(HexRecord & hexrecord) { //uint8_t byteCount, uint8_t* data) {
  if (hexrecord.datalength != 4) {
    Serial.print(F("WARNING: Start Linear Address record should have 4 bytes, but got: "));
    Serial.println(hexrecord.datalength);
    return;
  }

  uint32_t startaddress = hexrecord.data[0];
  startaddress <<= 8;
  startaddress |= hexrecord.data[1];
  startaddress <<= 8;
  startaddress |= hexrecord.data[2];
  startaddress <<= 8;
  startaddress |= hexrecord.data[3];
  pgmstatus.startLinearAddress = startaddress;

  Serial.print(F("OK: Start Linear Address changed to: 0x"));
  Serial.println(pgmstatus.startLinearAddress, HEX);
}

/*
 * Handle End of File Record (Type 0x01)
 * Signals end of data transmission
 */
void handleEndOfFile(HexRecord & hexrecord) {
  //Serial.println(F("---"));
  Serial.println(F("OK: I end-of-file"));
}

/*
 * Validate Intel HEX checksum
 * Checksum = two's complement of sum of all bytes except checksum
 */
bool validateChecksum(HexRecord & record) {
  uint8_t sum = record.datalength;
  sum += (record.address >> 8) & 0xFF;
  sum += record.address & 0xFF;
  sum += (record.type[1] - '0') & 0xff;

  for (int i = 0; i < record.datalength; i++) {
    sum += record.data[i];
  }

  // Checksum validation: (sum + checksum) should equal 0x00 (two's complement)
  uint8_t calculatedChecksum = (~sum + 1) & 0xFF;

  return record.checksum == calculatedChecksum;
}

/*
 * Convert two hex characters to integer
 * Returns 0 if invalid
 */
 /*
uint8_t hexToUint8(String hex) {
  if (hex.length() != 2) {
    return 0;
  }

  char buffer[3];
  hex.toCharArray(buffer, 3);
  return (uint8_t)strtol(buffer, NULL, 16);
}
*/


