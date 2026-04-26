/*
 * Intel HEX EEPROM Programmer for Arduino
 * Reads Intel HEX formatted data from serial (UART)
 * Validates checksums and handles major record types
 * Writes parsed data to aux EEPROM at28c64 etc. throuhg spi bus.

 avrdude -c arduino -p m328p -P /dev/ttyUSB0 -b 115200 -U eeprom:w:yourfile.hex:i

 -c arduino – Programmer type (Arduino bootloader)
-p m328p – Microcontroller (ATmega328P; adjust for your board: m2560, m32u4, etc.)
-P /dev/ttyUSB0 – Serial port (use COM3 on Windows, /dev/ttyUSB0 on Linux, /dev/tty.usbserial-* on macOS)
-b 115200 – Baud rate (must match your sketch: SERIAL_BAUD)
-U eeprom:w:yourfile.hex:i – Write yourfile.hex to EEPROM in Intel HEX format
Adjust for your setup:

Replace m328p with your MCU (e.g., m2560 for Mega, m32u4 for Leonardo)
Replace /dev/ttyUSB0 with your actual serial port
Replace yourfile.hex with your Intel HEX filename

 */

#include <SPI.h>
#include <SPISRAM.h>

enum SPI_SLAVES {
  CS_23LC1024 = 10,
  CS_MCP23S08 = 9,
  CS_MCP23S17 = 8,
};

SPISRAM spisram(CS_23LC1024, SPISRAM::BUS_MBits); // CS pin

// Configuration
#define SERIAL_BAUD 115200
#define MAX_HEX_LINE_LENGTH 256

// Intel HEX Record Types
#define HEX_DATA 0x00
#define HEX_END_OF_FILE 0x01
#define HEX_EXTENDED_LINEAR_ADDR 0x04
#define HEX_START_LINEAR_ADDR 0x05

// Global variables
unsigned int extendedLinearAddress = 0;
unsigned long bytesWritten = 0;
unsigned long checksumErrors = 0;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(100);
  Serial.println("Intel HEX Programmer Ready");
  Serial.println("Send Intel HEX data (e.g., from avrdude)");
  Serial.println("---");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    
    // Skip empty lines and comments
    if (line.length() == 0 || line[0] != ':') {
      return;
    }
    
    // Process Intel HEX record
    processHexRecord(line);
  }
}

/*
 * Process a single Intel HEX record
 * Format: :LLAAAATTDD...CC
 * LL = Byte count (2 hex chars)
 * AAAA = Address (4 hex chars)
 * TT = Record type (2 hex chars)
 * DD... = Data bytes
 * CC = Checksum (2 hex chars)
 */
void processHexRecord(String line) {
  // Validate minimum length (:LLAAAATTCC = 11 chars)
  if (line.length() < 11) {
    Serial.print("ERROR: Invalid line length: ");
    Serial.println(line);
    return;
  }
  
  // Extract byte count
  uint8_t byteCount = hexToInt(line.substring(1, 3));
  
  // Validate line length: :LL + AAAA + TT + DD*2 + CC
  int expectedLength = 11 + (byteCount * 2);
  if (line.length() != expectedLength) {
    Serial.print("ERROR: Line length mismatch. Expected: ");
    Serial.print(expectedLength);
    Serial.print(" Got: ");
    Serial.println(line.length());
    return;
  }
  
  // Extract address
  uint16_t address = (hexToInt(line.substring(3, 5)) << 8) | 
                     hexToInt(line.substring(5, 7));
  
  // Extract record type
  uint8_t recordType = hexToInt(line.substring(7, 9));
  
  // Extract data bytes
  uint8_t data[256];
  for (int i = 0; i < byteCount; i++) {
    data[i] = hexToInt(line.substring(9 + (i * 2), 11 + (i * 2)));
  }
  
  // Extract and validate checksum
  uint8_t checksum = hexToInt(line.substring(9 + (byteCount * 2), 11 + (byteCount * 2)));
  
  if (!validateChecksum(byteCount, address, recordType, data, checksum)) {
    Serial.print("ERROR: Checksum validation failed for line: ");
    Serial.println(line);
    checksumErrors++;
    return;
  }
  
  // Handle record types
  switch (recordType) {
    case HEX_DATA:
      handleDataRecord(address, byteCount, data);
      break;
      
    case HEX_EXTENDED_LINEAR_ADDR:
      handleExtendedLinearAddress(byteCount, data);
      break;
      
    case HEX_START_LINEAR_ADDR:
      handleStartLinearAddress(byteCount, data);
      break;
      
    case HEX_END_OF_FILE:
      handleEndOfFile();
      break;
      
    default:
      Serial.print("WARNING: Unknown record type: 0x");
      Serial.println(recordType, HEX);
      break;
  }
}

/*
 * Handle Data Record (Type 0x00)
 * Writes data to EEPROM at the computed address
 */
void handleDataRecord(uint16_t address, uint8_t byteCount, uint8_t* data) {
  // Compute full address (extended + base address)
  unsigned long fullAddress = (extendedLinearAddress << 16) | address;
  
  // Check if address is within memory bounds
  if (fullAddress + byteCount > 0x20000 ) {
    Serial.print("ERROR: Address out of memory bounds: 0x");
    Serial.print(fullAddress, HEX);
    Serial.print(" (size: ");
    Serial.print(0x20000);
    Serial.println(")");
    return;
  }
  
  // Write data to EEPROM
  for (int i = 0; i < byteCount; i++) {
    spisram.write(fullAddress + i, data[i]);
  }
  
  bytesWritten += byteCount;
  
  Serial.print("OK: Wrote ");
  Serial.print(byteCount);
  Serial.print(" bytes at 0x");
  Serial.println(fullAddress, HEX);
}

/*
 * Handle Extended Linear Address Record (Type 0x04)
 * Sets the upper 16 bits of the address
 */
void handleExtendedLinearAddress(uint8_t byteCount, uint8_t* data) {
  if (byteCount != 2) {
    Serial.print("ERROR: Extended Linear Address record must have 2 bytes, got: ");
    Serial.println(byteCount);
    return;
  }
  
  extendedLinearAddress = (data[0] << 8) | data[1];
  
  Serial.print("OK: Extended Linear Address set to 0x");
  Serial.println(extendedLinearAddress, HEX);
}

/*
 * Handle Start Linear Address Record (Type 0x05)
 * Optional: used for execution start address (informational)
 */
void handleStartLinearAddress(uint8_t byteCount, uint8_t* data) {
  if (byteCount != 4) {
    Serial.print("WARNING: Start Linear Address record should have 4 bytes, got: ");
    Serial.println(byteCount);
    return;
  }
  
  unsigned long startAddr = ((unsigned long)data[0] << 24) |
                            ((unsigned long)data[1] << 16) |
                            ((unsigned long)data[2] << 8) |
                            data[3];
  
  Serial.print("OK: Start Linear Address: 0x");
  Serial.println(startAddr, HEX);
}

/*
 * Handle End of File Record (Type 0x01)
 * Signals end of data transmission
 */
void handleEndOfFile() {
  Serial.println("---");
  Serial.print("Programming complete!");
  Serial.print(" Total bytes written: ");
  Serial.println(bytesWritten);
  Serial.print("Checksum errors: ");
  Serial.println(checksumErrors);
  
  // Reset counters for next session
  bytesWritten = 0;
  checksumErrors = 0;
  extendedLinearAddress = 0;
}

/*
 * Validate Intel HEX checksum
 * Checksum = two's complement of sum of all bytes except checksum
 */
bool validateChecksum(uint8_t byteCount, uint16_t address, uint8_t recordType, 
                      uint8_t* data, uint8_t checksum) {
  uint8_t sum = byteCount;
  sum += (address >> 8) & 0xFF;
  sum += address & 0xFF;
  sum += recordType;
  
  for (int i = 0; i < byteCount; i++) {
    sum += data[i];
  }
  
  // Checksum validation: (sum + checksum) should equal 0x00 (two's complement)
  uint8_t calculatedChecksum = (~sum + 1) & 0xFF;
  
  return checksum == calculatedChecksum;
}

/*
 * Convert two hex characters to integer
 * Returns 0 if invalid
 */
uint8_t hexToInt(String hex) {
  uint8_t val = 0;
  for(int i = 0; i < 2; ++i) {
    char c = hex[i];
    if ( c < '0' ) 
      return 0;
    if ( c <= '9' ) {
      val = c - '0';
    } else {
      c = toupper(c);
      if ( c >= 'A' && c <= 'F' ) {
        val = c - 'A' + 10;
      } else {
        return 0;
      }
    }
    val <<= 8;
  }
  return val;
}

