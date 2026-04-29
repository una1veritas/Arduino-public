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

void processHexRecord(String line, HexRecord & hexrecord) {
  // Validate minimum length (:LLAAAATTCC = 11 chars)
  if (line.length() < 11) {
	  wstatus.errorCount += 1;
	  Serialsnprintln("ERROR: Invalid line length: %d", line.length());
	  return;
  }

  // Extract byte count
  //uint8_t byteCount
  hexrecord.datalength = hexToUint8(line, 1);

  // Validate line length: :LL + AAAA + TT + DD*2 + CC
  int expectedLength = 11 + (hexrecord.datalength * 2);
  if (line.length() != expectedLength) {
    // Serial.print("ERROR: Line length mismatch. Expected: ");
    // Serial.print(expectedLength);
    // Serial.print(" Got: ");
    // Serial.println(line.length());
	  wstatus.errorCount += 1;
	  Serialsnprintln("ERROR: Line length mismatch. Expected: %d Got: %d", expectedLength, line.length());
	  Serialsnprintln("Line: %s", line);
	  return;
  }

  // Extract address
//  uint16_t address = (hexToUint8(line.substring(3, 5)) << 8) |
//                     hexToUint8(line.substring(5, 7));
  hexrecord.address = ( uint16_t(hexToUint8(line, 3)) << 8) | hexToUint8(line, 5);
  // Extract record type
  //uint8_t recordType
  hexrecord.type = hexToUint8(line.substring(7, 9));

  // Extract data bytes
  //uint8_t data[256];
  for (int i = 0; i < hexrecord.datalength; i++) {
    hexrecord.data[i] = hexToUint8(line, 9 + (i<<1));
  }

  // Extract and validate checksum
  //uint8_t checksum =
  hexrecord.checksum = hexToUint8(line, 9 + (hexrecord.datalength<<1));

  if ( !validateChecksum(hexrecord) ) {
    Serialsnprintln("ERROR: Checksum validation failed for line: %d", line);
    wstatus.checksumErrors++;
    return;
  }

  // Handle record types
  switch (hexrecord.type) {
    case IHEX_DATA:
      handleDataRecord(hexrecord);
      break;

    case IHEX_EXTENDED_LINEAR_ADDR:
      handleExtendedLinearAddress(hexrecord);
      break;

    case IHEX_START_LINEAR_ADDR:
      handleStartLinearAddress(hexrecord);
      break;

    case IHEX_END_OF_FILE:
      handleEndOfFile(hexrecord);
      break;

    default:
      Serialsnprintln("WARNING: Unknown record type: 0x%02x", hexrecord.type);
      break;
  }
}

/*
 * Handle Data Record (Type 0x00)
 * Writes data to EEPROM at the computed address
 */
void handleDataRecord(HexRecord & hexrecord) {
  // Compute full address (extended + base address)
  unsigned long fullAddress = (uint32_t(addrcontext.extLinearAddress) << 16) | hexrecord.address;

  // Check if address is within memory bounds
  if (fullAddress + hexrecord.datalength > 0x20000 ) {
	  wstatus.errorCount += 1;
	  Serialsnprintln("ERROR: Address out of memory bounds: 0x%04x (size: 0x%04x)", fullAddress, 0x20000);
    return;
  }

  // Serial.print("OK: Wrote ");
  // Serial.print(byteCount);
  // Serial.print(" bytes at 0x");
  // Serial.println(fullAddress, HEX);
  Serialsnprint("I DATA %04X : ", fullAddress);
  for (int i = 0; i < hexrecord.datalength; i++) {
    Serialsnprint("%02X ", hexrecord.data[i]);
  }
  Serialsnprintln(" (%d bytes)", hexrecord.datalength);

  // Write data to EEPROM
  for (int i = 0; i < hexrecord.datalength; i++) {
    write(fullAddress + i, hexrecord.data[i]);
  }

  wstatus.totalBytesWritten += hexrecord.datalength;

  // Serial.print("OK: Wrote ");
  // Serial.print(byteCount);
  // Serial.print(" bytes at 0x");
  // Serial.println(fullAddress, HEX);
}

/*
 * Handle Extended Linear Address Record (Type 0x04)
 * Sets the upper 16 bits of the address
 */
void handleExtendedLinearAddress(HexRecord & hexrecord) {
  if (hexrecord.datalength != 2) {
	  wstatus.errorCount += 1;
    Serialsnprintln("ERROR: Extended Linear Address record must have 2 bytes, got: %d", hexrecord.datalength);
    return;
  }

  addrcontext.extLinearAddress = (uint16_t(hexrecord.data[0]) << 8) | hexrecord.data[1];

  Serialsnprintln("OK: Extended Linear Address (high 16bit) set to 0x%04x", addrcontext.extLinearAddress);
}

/*
 * Handle Start Linear Address Record (Type 0x05)
 * Optional: used for execution start address (informational)
 */
void handleStartLinearAddress(HexRecord & hexrecord) { //uint8_t byteCount, uint8_t* data) {
  if (hexrecord.datalength != 4) {
    Serial.print("WARNING: Start Linear Address record should have 4 bytes, got: ");
    Serial.println(hexrecord.datalength);
    return;
  }

  addrcontext.startLinearAddress = hexrecord.data[0];
  addrcontext.startLinearAddress <<= 8;
  addrcontext.startLinearAddress |= hexrecord.data[1];
  addrcontext.startLinearAddress <<= 8;
  addrcontext.startLinearAddress |= hexrecord.data[2];
  addrcontext.startLinearAddress <<= 8;
  addrcontext.startLinearAddress |= hexrecord.data[3];

  Serial.print("OK: Start Linear Address (32 bits): 0x");
  Serial.println(addrcontext.startLinearAddress, HEX);
}

/*
 * Handle End of File Record (Type 0x01)
 * Signals end of data transmission
 */
void handleEndOfFile(HexRecord & hexrecord) {
  Serial.println("---");
  Serial.print("iHex load complete!");
  Serial.print(" Total bytes loaded: ");
  Serial.println(wstatus.totalBytesWritten);
  Serial.print("Checksum errors: ");
  Serial.println(wstatus.checksumErrors);

  // Reset counters for next session
  //clearWriterStatus();
//  wstatus.totalBytesWritten = 0;
//  wstatus.checksumErrors = 0;
//  wstatus.extendedLinearAddress = 0;
  clearWriterStatus(); // wstatus.loadInProgress = false;
}

/*
 * Validate Intel HEX checksum
 * Checksum = two's complement of sum of all bytes except checksum
 */
bool validateChecksum(HexRecord & hexrecord) {
  uint8_t sum = hexrecord.datalength;
  sum += (hexrecord.address >> 8) & 0xFF;
  sum += hexrecord.address & 0xFF;
  sum += hexrecord.type;

  for (int i = 0; i < hexrecord.datalength; i++) {
    sum += hexrecord.data[i];
  }

  // Checksum validation: (sum + checksum) should equal 0x00 (two's complement)
  uint8_t calculatedChecksum = (~sum + 1) & 0xFF;

  return hexrecord.checksum == calculatedChecksum;
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


