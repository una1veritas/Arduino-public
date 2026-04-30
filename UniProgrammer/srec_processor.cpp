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

/*
void loadS19() {
  Serial.println(F("\nEntering S19 load mode. Send S-records or 'Q' to quit.\n"));
  
  wstatus.loadInProgress = true;
  unsigned long startTime = millis();
  
  while (wstatus.loadInProgress) {
    if (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      line.trim();
      
      if (line.length() == 0) {
        continue;
      }
      
      if (line[0] == 'Q' || line[0] == 'q') {
        Serial.println(F("Load cancelled by user."));
        wstatus.loadInProgress = false;
        break;
      }
      
      if (line[0] != 'S' || line.length() < 4) {
        Serial.print(F("Invalid format: "));
        Serial.println(line);
        wstatus.errorCount++;
        continue;
      }
      
      if (!processS19Record(line, hexrecord)) {
        wstatus.errorCount++;
        Serial.println(F("Load terminated due to error."));
        wstatus.loadInProgress = false;
      }
    }
    
    // Timeout after 30 seconds of inactivity
    if (millis() - startTime > 30000) {
      Serial.println(F("Timeout: No data received for 30 seconds."));
      wstatus.loadInProgress = false;
    }
  }
  
  Serial.println(F("Load complete.\n"));
}
*/

boolean processS19Record(const String& line, HexRecord & hexrecord) {
  if (line.length() < 4) {
	  Serialsnprintln("Error: line is too short %d", line.length());
	  wstatus.errorCount++;
	  return false;
  }
  
  //SREC_RecordType
  hexrecord.type = (SREC_RecordType)(line[1] - '0');
  
  // Parse byte count (position 2-3, in hex)
  hexrecord.byteCount = hexToUint8(line, 2);
  
  if ( hexrecord.byteCount < 2 ) {
	  // At least address + checksum
    Serialsnprintln("Too small byte count %d: %s", hexrecord.byteCount, line);
    wstatus.errorCount++;
    return false;
  }
  
  // Verify record length matches
  int expectedLength = 4 + hexrecord.byteCount * 2;
  if (line.length() != expectedLength) {
    Serial.print(F("Length mismatch. Expected: "));
    Serial.print(expectedLength);
    Serial.print(F(", Got: "));
    Serial.println(line.length());
    Serialsnprintln("Line: %s", line);
    wstatus.errorCount++;
    return false;
  }
  
  // Verify checksum
  if (!verifyChecksum(line, hexrecord)) {
    Serial.println(F("Checksum error."));
    return false;
  }
  
  wstatus.recordCount++;
  
  // Process based on record type
  switch (hexrecord.type) {
  // start record
    case SREC_HEADER: 	// type 0
	  // S0 header: S0 + count + address(2) + data + checksum
	  // Typically contains manufacturer info, can extract and display
    	hexrecord.datalength = hexrecord.byteCount - 3;  // Subtract address (2) and checksum (1)
      return processHeader(line, hexrecord); //byteCount);
    
    // data record
    case SREC_DATA_16: 	// type 1
    case SREC_DATA_24:	// type 2
    case SREC_DATA_32: 	// type 3
    	// Subtract the number of bytes for address and checksum
    	// hexrecord.byteCount - (hexrecord.type + 1) - 1;
    	hexrecord.datalength = hexrecord.byteCount - hexrecord.type;
      return processDataRecord(line, hexrecord, hexrecord.type + 1); //byteCount, 2);  // 2 bytes address

    // data record count (not official)
    case SREC_COUNT_16:
    case SREC_COUNT_24:
      // Count records are informational, can be ignored
      return true;
    
      // start address (declare the termination)
    case SREC_START_32:
    	hexrecord.datalength = 0; //hexrecord.byteCount - 4 - 1;
      return processStartAddress(line, 4);  // 4 bytes address
    case SREC_START_24:
    	hexrecord.datalength = 0; //hexrecord.byteCount - 3 - 1;
      return processStartAddress(line, 3);  // 3 bytes address
    case SREC_START_16:
    	hexrecord.datalength = 0; //hexrecord.byteCount - 2 - 1;
      return processStartAddress(line,  2);  // 2 bytes address
    
      // unknown type. error
    default:
      Serial.print(F("Unknown record type: S"));
      Serial.println(hexrecord.type, DEC);
      return false;
  }
}

boolean processHeader(const String& record, HexRecord & hexrecord) { //uint8_t byteCount) {
  if (hexrecord.datalength > 0) {
    Serial.print(F("Header: "));
    for (int i = 0; i < hexrecord.datalength; i++) {
      uint8_t b = hexToUint8(record, 8 + i * 2);
      Serial.write(b);
    }
    Serial.println();
  }
  
  return true;
}

boolean processDataRecord(const String& record, HexRecord & hexrecord, /* uint8_t byteCount*/ uint8_t addressBytes) {
  // Parse address
	hexrecord.address = 0;
  for (int i = 0; i < addressBytes; i++) {
	  hexrecord.address = (hexrecord.address << 8) | hexToUint8(record, 4 + i * 2);
  }
  //Serialsnprintln("address = %04x", hexrecord.address);
  
  // Data starts after S-record type (2) + count (2) + address
  int dataStartPos = 4 + addressBytes * 2;
  
  //Serialsnprintln("data length %d", hexrecord.datalength);
  // Check if total data won't exceed 128 KB
  if (hexrecord.address + hexrecord.datalength > 0x20000) {
    Serial.print(F("Error: Data would exceed 128 KB boundary. Address: 0x"));
    Serial.print(hexrecord.address, HEX);
    Serial.print(F(", Length: "));
    Serial.println(hexrecord.datalength);
    wstatus.errorCount++;
    return false;
  }
  
  // Write data to auxiliary memory
  Serialsnprint("S%d DATA %04x : ", hexrecord.type, hexrecord.address);
  for (int i = 0; i < hexrecord.datalength; i++) {
    //uint8_t byte = hexToUint8(record, dataStartPos + i * 2);
    hexrecord.data[i] = hexToUint8(record, dataStartPos + i * 2);
    Serialsnprint("%02x ", hexrecord.data[i]);
    //write(hexrecord.address + i, byte);
    wstatus.totalBytesWritten++;
  }
  Serialsnprintln("(%d bytes)", hexrecord.datalength);

  
//  Serial.print(F("S"));
//  Serial.print(record[1]);
//  Serial.print(F(": Address=0x"));
//  Serial.print(hexrecord.address, HEX);
//  Serial.print(F(", Length="));
//  Serial.println(hexrecord.length);
//
  return true;
}

boolean processStartAddress(const String& record, uint8_t addressBytes) {
  // Parse start address
	// usually used to represent the end
  uint32_t address = 0;
  for (int i = 0; i < addressBytes; i++) {
    address = (address << 8) | hexToUint8(record, 4 + i * 2);
  }
  addrcontext.startLinearAddress = address;
  Serial.print(F("Start address: 0x"));
  Serial.println(address, HEX);
  
  //clearWriterStatus();
  return true;
}

boolean verifyChecksum(const String& record, HexRecord & hexrecord) { //uint8_t byteCount) {
  // Calculate checksum of all bytes except the checksum itself
  uint8_t calculatedSum = 0;
  
  for (int i = 0; i < hexrecord.byteCount; i++) {
    calculatedSum += hexToUint8(record, 2 + i * 2);
  }
  
  // Checksum is the one's complement of the calculated sum
  uint8_t expectedChecksum = (~calculatedSum) & 0xFF;
  uint8_t recordChecksum = hexToUint8(record, 2 + hexrecord.byteCount * 2);
  
  return expectedChecksum == recordChecksum;
}

