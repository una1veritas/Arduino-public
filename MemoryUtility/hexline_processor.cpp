/*
 * ihex_processor.cpp
 *
 *  Created on: 2026/04/28
 *      Author: sin
 */

#include <Arduino.h>

#include "hexline_processor.h"

#include "pagearray.h"

// common utilities

uint8_t charToNibble(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }
  return 0;
}

uint8_t hexToUint8(const String & hex, const int & startpos) {
	if (startpos + 2 > hex.length()) {
		return 0;
	}
	uint8_t val = 0;
	val <<= 4;
	val |= charToNibble(hex[startpos]);
	val <<= 4;
	val |= charToNibble(hex[startpos + 1]);
	return val;
}

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

void processiHexRecord(String line, HexRecord &record) {
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

	if (! validateiHexChecksum(record)) {
		Serial.println(F("ERROR: Checksum validation failed for line:"));
		Serial.println(line);
		pgmstatus.checksumErrors++;
		return;
	}

	pgmstatus.recordCount++;

	// Handle record types
	switch ((char) record.type[1]) {
	case '0': //IHEX_DATA:
		handleiHexDataRecord(record);
		break;

	case '4': //IHEX_EXTENDED_LINEAR_ADDR:
		handleiHexExtendedLinearAddress(record);
		break;

	case '5': //IHEX_START_LINEAR_ADDR:
		handleiHexStartLinearAddress(record);
		break;

	case '1': //IHEX_END_OF_FILE:
		handleiHexEndOfFile(record);
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
void handleiHexDataRecord(const HexRecord & record) {
	// Check if address is within memory bounds
	if ( record.address +  record.datalength > 0x20000) {
		pgmstatus.errorCount += 1;
		Serial.print(F("ERROR: Address out of memory bounds: "));
		snprintf(buf128, 127, "0x%04x (size: 0x%04x)",  record.address, 0x20000);
		Serial.println(buf128);
		return;
	}

	// Write data to auxiliary memory
	pagearray.append_bytes(record.address, record.data, record.datalength);
//	uint8_t * ptr = (uint8_t *) & record;
//	for (uint32_t ix = 0; ix < record.header_size(); ++ix, ++ptr) {
//		auxsram.write(pgmstatus.start_ix + ix, *ptr);
//	}
//	for (uint32_t ix = 0; ix < record.datalength; ++ix, ++ptr) {
//		auxsram.write(pgmstatus.start_ix + record.header_size() + ix, *ptr);
//	}
//	pgmstatus.start_ix += record.header_size() + record.datalength;
//	auxsram.write(pgmstatus.start_ix, 0x00);

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
void handleiHexExtendedLinearAddress(HexRecord &hexrecord) {
	if (hexrecord.datalength != 2) {
		pgmstatus.errorCount += 1;
		snprintf(buf128, 127,
				"ERROR: Extended Linear Address record must have 2 bytes, got: %d",
				hexrecord.datalength);
		Serial.println();
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
void handleiHexStartLinearAddress(HexRecord & hexrecord) { //uint8_t byteCount, uint8_t* data) {
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
void handleiHexEndOfFile(HexRecord & hexrecord) {
  //Serial.println(F("---"));
  Serial.println(F("OK: I end-of-file"));
}

/*
 * Validate Intel HEX checksum
 * Checksum = two's complement of sum of all bytes except checksum
 */
bool validateiHexChecksum(HexRecord & record) {
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

boolean processS19Record(const String &line, HexRecord &record) {
	if (line.length() < 4) {
		Serial.println(F("Error: Too short line length "));
		Serial.println(line.length(), DEC);
		pgmstatus.errorCount++;
		return false;
	}
	//SREC_RecordType
	record.type[0] = line[0];
	record.type[1] = line[1];
//	Serial.print("type = ");
//	Serial.print(record.type[0]);
//	Serial.println(record.type[1]);

	// Parse byte count (position 2-3, in hex)
	const uint8_t byteCount = hexToUint8(line, 2);
//	Serial.print("byteCount = ");
//	Serial.println(byteCount, HEX);

	if ( byteCount < 2 ) {
		// At least address + checksum
		Serial.print(F("Error: Too small byte count "));
		Serial.println(byteCount, DEC);
		pgmstatus.errorCount++;
		return false;
	}

	// Verify record length matches
	int expectedLength = 4 + byteCount * 2;
	if (line.length() != expectedLength) {
		Serial.print(F("Error: Length mismatch, expected "));
		Serial.print(expectedLength);
		Serial.print(F(" but got "));
		Serial.println(line.length());
		Serial.println(line);
		pgmstatus.errorCount++;
		return false;
	}

	// read address
	uint8_t addressbytes;
	switch ((char) record.type[1]) {
	// header message record
	case '0': 	//SREC_HEADER: 	// type 0
		addressbytes = 2;
		break;
	// data record
	case '1': 	// SREC_DATA_16: 	// type 1
	case '2': 	// SREC_DATA_24:	// type 2
	case '3': 	// SREC_DATA_32: 	// type 3
		addressbytes = record.type[1] - '1' + 2;
		break;
	case '5': 	// SREC_COUNT_16: // S5
	case '6':	// SREC_COUNT_24: // S6
		// Count records are informational, can be ignored
		addressbytes = record.type[1] - '3';
		break;
	// start address (declare the termination)
	case '7': 	// SREC_START_32:  // S7
	case '8':	// SREC_START_24:  // S8
	case '9':	// SREC_START_16:  // S9
		addressbytes = '9' - record.type[1] + 2;
		break;
	default:
		addressbytes = 2;
		break;
	}
//	Serial.print("address bytes = ");
//	Serial.println(addressbytes);

	record.datalength = byteCount - addressbytes - 1;  // 1 for check sum

	// Parse address
	record.address = 0;
	for(int ix = 0; ix < addressbytes; ++ix) {
		record.address <<= 8;
		record.address |= hexToUint8(line, 4 + (ix << 1) );
	}
	//Serialsnprint(buf128, 127, "addr = %04x\n", record.address);

	// parse data field
	const int dataStartPos = 4 + (addressbytes << 1);
	for (int i = 0; i < record.datalength; i++) {
		record.data[i] = hexToUint8(line, dataStartPos + (i << 1));
	}

	record.checksum = hexToUint8(line, 2 + (byteCount << 1));
	//Serialsnprint(buf128, 127, "chksum = %02x, calced = %02x\n", record.checksum, calcChecksum(line));

	// Verify checksum
	if ( calcS19Checksum(line) != record.checksum ) {
		Serial.println(F("Error: Checksum error."));
		pgmstatus.checksumErrors++;
		return false;
	}

	// record is loaded.
	pgmstatus.recordCount++;

	// Process based on record type
	switch ((char) record.type[1]) {
	// start record
	case '0': 	// SREC_HEADER: 	// type 0
		// S0 header: S0 + count + address(2) + data + checksum
		// Typically contains manufacturer info, can extract and display
		return processS19Header(record); //byteCount);

		// data record
	case '1': 	// SREC_DATA_16: 	// type 1
	case '2': 	// SREC_DATA_24:	// type 2
	case '3': 	// SREC_DATA_32: 	// type 3
		// Subtract the number of bytes for address and checksum
		return processS19DataRecord(record); //byteCount, 2);  // 2 bytes address

		// data record count (not official)
	case '5': 	// SREC_COUNT_16:
	case '6': 	// SREC_COUNT_24:
		// Count records are informational, can be ignored
		return true;

		// start address (declare the termination)
	case '7': 	// SREC_START_32:
	case '8': 	// SREC_START_24:
	case '9': 	// SREC_START_16:
		return processS19StartAddress(record);  // 4 bytes address

		// unknown type. error
	default:
		Serial.print(F("Unknown record type: "));
		Serial.print(record.type[0]);
		Serial.println(record.type[1]);
		return false;
	}
}

boolean processS19Header(const HexRecord & hexrecord) { //uint8_t byteCount) {
  if (hexrecord.datalength > 0) {
    Serial.print(F("OK Header: "));
    for (int i = 0; i < hexrecord.datalength; i++) {
      snprintf(buf128, 127, "%02X ", hexrecord.data[i]);
      Serial.print(buf128);
    }
    Serial.println();
    for (int i = 0; i < hexrecord.datalength; i++) {
    	if ( isprint(hexrecord.data[i]) ) {
    		Serial.print( (char) hexrecord.data[i]);
    	} else {
    		Serial.print(F("?"));
    	}
    }
    Serial.println();  }
  return true;
}

boolean processS19DataRecord(const HexRecord &record) {
	// Check if total data won't exceed 128 KB
	if (record.address + record.datalength > 0x20000) {
		Serial.print(F("Error: Data would exceed 128 KB limit, from 0x"));
		Serial.print(record.address, HEX);
		Serial.print(F(" length "));
		Serial.println(record.datalength);
		pgmstatus.errorCount++;
		return false;
	}

//	for(int ix = 0; ix < record.datalength; ++ix) {
//		Serial.print(record.data[ix], HEX);
//		Serial.print(' ');
//	}
//	Serial.println();
	// Write data to auxiliary memory
	pagearray.append_bytes(record.address, record.data, record.datalength );
//	uint8_t * ptr = (uint8_t *) & record;
//	for (uint32_t ix  = 0; ix < HexRecord::header_size(); ++ix, ++ptr) {
//		auxsram.write(pgmstatus.start_ix + ix, *ptr);
//	}
//	for (uint32_t i = 0; i < record.datalength; ++i, ++ptr) {
//		auxsram.write(pgmstatus.start_ix +  HexRecord::header_size() + i, *ptr);
//	}
//	pgmstatus.start_ix += HexRecord::header_size() + record.datalength;
//	auxsram.write(pgmstatus.start_ix, 0x00);

	pgmstatus.totalBytesWritten += record.datalength;

	Serial.print("OK: S");
	Serial.print(record.type[1]);
	Serial.print(F(" DATA "));
	Serial.print(record.address, HEX);
	Serial.print(F(" -- "));
	Serial.println(record.address + record.datalength - 1, HEX);

	return true;
}

boolean processS19StartAddress(const HexRecord & record) {
  // Parse start address
	// usually used to represent the end
	pgmstatus.startLinearAddress = record.address;
  Serial.print(F("OK: Start address: 0x"));
  Serial.println(record.address, HEX);

  //clearWriterStatus();
  return true;
}

uint8_t calcS19Checksum(const String & line) {
  // Calculate checksum of all bytes except the first byte "Sx" and the checksum itself
  uint8_t calculatedSum = 0;
  uint8_t byteCount = hexToUint8(line, 2);
  for (int i = 0; i < byteCount; i++) {
    calculatedSum += hexToUint8(line, 2 + (i << 1) );
  }

  // Checksum is the one's complement of the calculated sum
  uint8_t expectedChecksum = (~calculatedSum) & 0xFF;
  //uint8_t recordChecksum = hexToUint8(line, 2 + (byteCount << 1) );
  //return expectedChecksum == recordChecksum;
  return expectedChecksum;
}



