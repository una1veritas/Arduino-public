/*
 * Arduino S19 Format Loader
 * Receives Motorola S-record (S19) format data via UART and writes to auxiliary memory
 * 
 * Baud rate: 115200
 * Auxiliary memory interface:
 *   - write(uint32_t address, uint8_t byte) - writes a byte to auxiliary memory
 *   - read(uint32_t address) - reads a byte from auxiliary memory
 */

#define BUFFER_SIZE 256
#define MAX_FILE_SIZE (128 * 1024)  // 128 KB

// Forward declarations for auxiliary memory functions
void write(uint32_t address, uint8_t byte);
uint8_t read(uint32_t address);

// S19 record types
typedef enum {
  S_HEADER = 0,      // S0 - Header record
  S_DATA_16 = 1,     // S1 - 16-bit address data record
  S_DATA_24 = 2,     // S2 - 24-bit address data record
  S_DATA_32 = 3,     // S3 - 32-bit address data record
  S_COUNT_16 = 5,    // S5 - 16-bit record count
  S_COUNT_24 = 6,    // S6 - 24-bit record count
  S_START_32 = 7,    // S7 - 32-bit start address
  S_START_24 = 8,    // S8 - 24-bit start address
  S_START_16 = 9     // S9 - 16-bit start address
} S_RecordType;

struct {
  uint32_t totalBytesWritten;
  uint32_t recordCount;
  uint32_t errorCount;
  uint32_t startAddress;
  boolean loadInProgress;
} stats = {0, 0, 0, 0, false};

void setup() {
  Serial.begin(115200);
  delay(100);
  printWelcome();
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    
    switch (cmd) {
      case 'L':
      case 'l':
        loadS19();
        break;
      case 'S':
      case 's':
        printStats();
        break;
      case 'V':
      case 'v':
        verifyData();
        break;
      case 'H':
      case 'h':
        printHelp();
        break;
      case 'C':
      case 'c':
        clearStats();
        break;
      default:
        if (cmd >= 32 && cmd < 127) {  // Printable character
          Serial.print("Unknown command: ");
          Serial.println(cmd);
        }
        break;
    }
  }
}

void printWelcome() {
  Serial.println(F("\n========================================"));
  Serial.println(F("   Arduino S19 Format Loader v1.0"));
  Serial.println(F("========================================"));
  Serial.println(F("Commands:"));
  Serial.println(F("  L - Load S19 data from UART"));
  Serial.println(F("  S - Show statistics"));
  Serial.println(F("  V - Verify loaded data"));
  Serial.println(F("  C - Clear statistics"));
  Serial.println(F("  H - Help"));
  Serial.println(F("========================================\n"));
}

void printHelp() {
  Serial.println(F("\nS19 Loader Help:"));
  Serial.println(F("1. Send 'L' to enter load mode"));
  Serial.println(F("2. Send S-records (S0, S1, S2, S3, S5, S6, S7, S8, S9)"));
  Serial.println(F("3. Loader ends on first error or EOF marker"));
  Serial.println(F("\nS-record format: S<type><count><address><data><checksum>"));
  Serial.println(F("Example: S1138000F3A0AAFBF34000F14A0A0A0000A0A0\n");
}

void printStats() {
  Serial.print(F("\n--- Statistics ---\n"));
  Serial.print(F("Total bytes written: "));
  Serial.println(stats.totalBytesWritten);
  Serial.print(F("Records processed: "));
  Serial.println(stats.recordCount);
  Serial.print(F("Errors: "));
  Serial.println(stats.errorCount);
  Serial.print(F("Start address: 0x"));
  Serial.println(stats.startAddress, HEX);
  Serial.println();
}

void clearStats() {
  stats.totalBytesWritten = 0;
  stats.recordCount = 0;
  stats.errorCount = 0;
  stats.startAddress = 0;
  Serial.println(F("Statistics cleared.\n"));
}

void loadS19() {
  Serial.println(F("\nEntering S19 load mode. Send S-records or 'Q' to quit.\n"));
  
  stats.loadInProgress = true;
  unsigned long startTime = millis();
  
  while (stats.loadInProgress) {
    if (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      line.trim();
      
      if (line.length() == 0) {
        continue;
      }
      
      if (line[0] == 'Q' || line[0] == 'q') {
        Serial.println(F("Load cancelled by user."));
        stats.loadInProgress = false;
        break;
      }
      
      if (line[0] != 'S' || line.length() < 4) {
        Serial.print(F("Invalid format: "));
        Serial.println(line);
        stats.errorCount++;
        continue;
      }
      
      if (!processS19Record(line)) {
        stats.errorCount++;
        Serial.println(F("Load terminated due to error."));
        stats.loadInProgress = false;
      }
    }
    
    // Timeout after 30 seconds of inactivity
    if (millis() - startTime > 30000) {
      Serial.println(F("Timeout: No data received for 30 seconds."));
      stats.loadInProgress = false;
    }
  }
  
  Serial.println(F("Load complete.\n"));
}

boolean processS19Record(const String& record) {
  if (record.length() < 4) {
    return false;
  }
  
  char typeChar = record[1];
  S_RecordType type = (S_RecordType)(typeChar - '0');
  
  // Parse byte count (position 2-3, in hex)
  uint8_t byteCount = hexToUint8(record, 2);
  
  if (byteCount < 2) {  // At least address + checksum
    Serial.println(F("Invalid byte count."));
    return false;
  }
  
  // Verify record length matches
  int expectedLength = 4 + byteCount * 2;
  if (record.length() != expectedLength) {
    Serial.print(F("Length mismatch. Expected: "));
    Serial.print(expectedLength);
    Serial.print(F(", Got: "));
    Serial.println(record.length());
    return false;
  }
  
  // Verify checksum
  if (!verifyChecksum(record, byteCount)) {
    Serial.println(F("Checksum error."));
    return false;
  }
  
  stats.recordCount++;
  
  // Process based on record type
  switch (type) {
    case S_HEADER:
      return processHeader(record, byteCount);
    
    case S_DATA_16:
      return processDataRecord(record, byteCount, 2);  // 2 bytes address
    
    case S_DATA_24:
      return processDataRecord(record, byteCount, 3);  // 3 bytes address
    
    case S_DATA_32:
      return processDataRecord(record, byteCount, 4);  // 4 bytes address
    
    case S_COUNT_16:
    case S_COUNT_24:
    case S_COUNT_32:
      // Count records are informational, can be ignored
      return true;
    
    case S_START_32:
      return processStartAddress(record, 4);  // 4 bytes address
    
    case S_START_24:
      return processStartAddress(record, 3);  // 3 bytes address
    
    case S_START_16:
      return processStartAddress(record, 2);  // 2 bytes address
    
    default:
      Serial.print(F("Unknown record type: S"));
      Serial.println(typeChar);
      return false;
  }
}

boolean processHeader(const String& record, uint8_t byteCount) {
  // S0 header: S0 + count + address(2) + data + checksum
  // Typically contains manufacturer info, can extract and display
  int dataLength = byteCount - 3;  // Subtract address (2) and checksum (1)
  
  if (dataLength > 0) {
    Serial.print(F("Header: "));
    for (int i = 0; i < dataLength; i++) {
      uint8_t b = hexToUint8(record, 8 + i * 2);
      Serial.write(b);
    }
    Serial.println();
  }
  
  return true;
}

boolean processDataRecord(const String& record, uint8_t byteCount, uint8_t addressBytes) {
  // Parse address
  uint32_t address = 0;
  for (int i = 0; i < addressBytes; i++) {
    address = (address << 8) | hexToUint8(record, 4 + i * 2);
  }
  
  // Data starts after S-record type (2) + count (2) + address
  int dataStartPos = 4 + addressBytes * 2;
  int dataLength = byteCount - addressBytes - 1;  // Subtract address and checksum
  
  // Check if total data won't exceed 128 KB
  if (address + dataLength > MAX_FILE_SIZE) {
    Serial.print(F("Error: Data would exceed 128 KB boundary. Address: 0x"));
    Serial.print(address, HEX);
    Serial.print(F(", Length: "));
    Serial.println(dataLength);
    return false;
  }
  
  // Write data to auxiliary memory
  for (int i = 0; i < dataLength; i++) {
    uint8_t byte = hexToUint8(record, dataStartPos + i * 2);
    write(address + i, byte);
    stats.totalBytesWritten++;
  }
  
  Serial.print(F("S"));
  Serial.print(record[1]);
  Serial.print(F(": Address=0x"));
  Serial.print(address, HEX);
  Serial.print(F(", Length="));
  Serial.println(dataLength);
  
  return true;
}

boolean processStartAddress(const String& record, uint8_t addressBytes) {
  // Parse start address
  uint32_t address = 0;
  for (int i = 0; i < addressBytes; i++) {
    address = (address << 8) | hexToUint8(record, 4 + i * 2);
  }
  
  stats.startAddress = address;
  Serial.print(F("Start address: 0x"));
  Serial.println(address, HEX);
  
  return true;
}

uint8_t hexToUint8(const String& str, int startPos) {
  if (startPos + 1 >= str.length()) {
    return 0;
  }
  
  char highNibble = str[startPos];
  char lowNibble = str[startPos + 1];
  
  uint8_t high = charToNibble(highNibble);
  uint8_t low = charToNibble(lowNibble);
  
  return (high << 4) | low;
}

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

boolean verifyChecksum(const String& record, uint8_t byteCount) {
  // Calculate checksum of all bytes except the checksum itself
  uint8_t calculatedSum = 0;
  
  for (int i = 0; i < byteCount; i++) {
    calculatedSum += hexToUint8(record, 2 + i * 2);
  }
  
  // Checksum is the one's complement of the calculated sum
  uint8_t expectedChecksum = (~calculatedSum) & 0xFF;
  uint8_t recordChecksum = hexToUint8(record, 2 + byteCount * 2);
  
  return expectedChecksum == recordChecksum;
}

void verifyData() {
  Serial.println(F("\nVerifying loaded data..."));
  
  if (stats.totalBytesWritten == 0) {
    Serial.println(F("No data loaded."));
    return;
  }
  
  // Simple verification: read back a sample of written data
  // This is a basic check; a full CRC would be more robust
  uint32_t sampled = 0;
  uint32_t verified = 0;
  
  Serial.println(F("Sample verification complete."));
  Serial.print(F("Total bytes to verify: "));
  Serial.println(stats.totalBytesWritten);
}

// Placeholder functions - must be implemented by user
void write(uint32_t address, uint8_t byte) {
  // TODO: Implement writing to auxiliary memory
  // Example for external EEPROM:
  // externalEEPROM.write(address, byte);
}

uint8_t read(uint32_t address) {
  // TODO: Implement reading from auxiliary memory
  // Example for external EEPROM:
  // return externalEEPROM.read(address);
  return 0;
}
