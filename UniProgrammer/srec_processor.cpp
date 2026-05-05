/*
 * Arduino S19 Format Loader
 * Receives Motorola S-record (S19) format data via UART and writes to auxiliary memory
 * 
 * Baud rate: 115200
 * Auxiliary memory interface:
 *   - write(uint32_t address, uint8_t byte) - writes a byte to auxiliary memory
 *   - read(uint32_t address) - reads a byte from auxiliary memory
 */

#include <Arduino.h>

#include "srec_processor.h"
#include "common.h"

//#define BUFFER_SIZE 256
//#define MAX_FILE_SIZE (128 * 1024)  // 128 KB

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
	if ( calcChecksum(line) != record.checksum ) {
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
		return processHeader(record); //byteCount);

		// data record
	case '1': 	// SREC_DATA_16: 	// type 1
	case '2': 	// SREC_DATA_24:	// type 2
	case '3': 	// SREC_DATA_32: 	// type 3
		// Subtract the number of bytes for address and checksum
		return processDataRecord(record); //byteCount, 2);  // 2 bytes address

		// data record count (not official)
	case '5': 	// SREC_COUNT_16:
	case '6': 	// SREC_COUNT_24:
		// Count records are informational, can be ignored
		return true;

		// start address (declare the termination)
	case '7': 	// SREC_START_32:
	case '8': 	// SREC_START_24:
	case '9': 	// SREC_START_16:
		return processStartAddress(record);  // 4 bytes address

		// unknown type. error
	default:
		Serial.print(F("Unknown record type: "));
		Serial.print(record.type[0]);
		Serial.println(record.type[1]);
		return false;
	}
}

boolean processHeader(const HexRecord & hexrecord) { //uint8_t byteCount) {
  if (hexrecord.datalength > 0) {
    Serial.print(F("OK Header: "));
    for (int i = 0; i < hexrecord.datalength; i++) {
      Serial.print(hexrecord.data[i], HEX);
    }
    Serial.println();
  }
  return true;
}

boolean processDataRecord(const HexRecord &record) {
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
	uint8_t * ptr = (uint8_t *) & record;
	for (uint32_t ix  = 0; ix < HexRecord::header_size(); ++ix, ++ptr) {
		auxsram.write(pgmstatus.start_ix + ix, *ptr);
	}
	for (uint32_t i = 0; i < record.datalength; ++i, ++ptr) {
		auxsram.write(pgmstatus.start_ix +  HexRecord::header_size() + i, *ptr);
	}
	pgmstatus.start_ix += HexRecord::header_size() + record.datalength;
	auxsram.write(pgmstatus.start_ix, 0x00);

	pgmstatus.totalBytesWritten += record.datalength;

	Serial.print("OK: S");
	Serial.print(record.type[1]);
	Serial.print(F(" DATA "));
	Serial.print(record.address, HEX);
	Serial.print(F(" -- "));
	Serial.println(record.address + record.datalength - 1, HEX);

	return true;
}

boolean processStartAddress(const HexRecord & record) {
  // Parse start address
	// usually used to represent the end
	pgmstatus.startLinearAddress = record.address;
  Serial.print(F("OK: Start address: 0x"));
  Serial.println(record.address, HEX);
  
  //clearWriterStatus();
  return true;
}

uint8_t calcChecksum(const String & line) {
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

