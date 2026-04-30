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

#include "common.h"

#include "ihex_processor.h"
#include "srec_processor.h"

enum SPI_SLAVES {
  CS_23LC1024 = 10,
  CS_MCP23S08 = 9,
  CS_MCP23S17 = 8,
};

SPISRAM spisram(CS_23LC1024, SPISRAM::BUS_MBits); // CS pin

uint8_t write(const uint32_t addr32, const uint8_t val8) {
	spisram.write(addr32, val8);
	return val8;
}

uint8_t read(const uint32_t addr32) {
	return spisram.read(addr32);
}

// Configuration
#define SERIAL_BAUD 115200

writer_status wstatus = { 0, 0, 0, 0, false };
HexRecord hexrecord;
AddressContext addrcontext = { 0, 0, 0 };

unsigned long idleStart;
String line = "";

bool processRecord(String & line) {
    if (line[0] == ':') {
      // Process Intel HEX record
    	hexrecord.start_code = ':';
    	processHexRecord(line, hexrecord);
      return true;
    } else if (line[0] == 'S') {
    	hexrecord.start_code = 'S';
    	processS19Record(line, hexrecord);
      return true;
    } else {
      Serial.print("comment: ");
      Serial.println(line);
      return true;
    }
    return false;
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(500);
  printWelcome();

  SPI.begin();
  spisram.begin();

  idleStart = millis();
}

void loop() {
  if (Serial.available()) {
    if ( wstatus.loadInProgress ) {
      line = Serial.readStringUntil('\n');
    } else {
      char c;
      while ( Serial.available() ) {
        c = (char) Serial.read();
        if ( c == '\n' )
          break;
        line += c;
        Serial.print(c);
      }
      if ( c == '\n' ) {
        Serial.println();
      } else {
        return;
      }
    }
    line.trim();
    // Skip empty lines
    if (line.length() == 0 ) {
      return;
    }
    if ( line[0] == '!' ) {
      if ( line.startsWith("!L") ) {
  			Serial.println("Waiting data...");
	  		wstatus.loadInProgress = true;
			idleStart = millis();
      } else if ( line.startsWith("!P") ) {
		    printWriterStatus();
		    idleStart = millis();
      } else if ( line.startsWith("!V") )  {
  		  verifyData();
        idleStart = millis();
      } else if ( line.startsWith("!H") )  {
  		  printHelp();
        idleStart = millis();
      } else if ( line.startsWith("!C") )  {
  		  clearWriterStatus();
        idleStart = millis();
      }
      line = "";
    } else {
      processRecord(line);
      wstatus.loadInProgress = true;
    	idleStart = millis();
      line = "";
    }
  }
  // Timeout after 30 seconds of inactivity
  if (millis() - idleStart > 30000 and wstatus.loadInProgress ) {
    Serial.println(F("Loading timeout: No data received for 30 seconds."));
    wstatus.loadInProgress = false;
  }
}


void printWelcome() {
  Serial.println(F("\n========================================"));
  Serial.println(F("   Arduino ihex/s19 Format Loader 202604"));
  Serial.println(F("========================================"));
  Serial.println(F("Commands:"));
  Serial.println(F("  !L - Wait and load hex data from UART"));
  Serial.println(F("  !P - Show statistics"));
  Serial.println(F("  !V - Verify loaded data"));
  Serial.println(F("  !C - Clear statistics"));
  Serial.println(F("  !H - Help"));
  Serial.println(F("========================================\n"));
}

void printHelp() {
  Serial.println(F("\nS19 Loader Help:"));
  Serial.println(F("1. Send 'L' to enter load mode"));
  Serial.println(F("2. Send S-records (S0, S1, S2, S3, S5, S6, S7, S8, S9)"));
  Serial.println(F("3. Loader ends on first error or EOF marker"));
  Serial.println(F("\nS-record format: S<type><count><address><data><checksum>"));
  Serial.println(F("Example: S1138000F3A0AAFBF34000F14A0A0A0000A0A0\n"));
}

void printStatus() {
  Serial.print(F("\n--- Statistics ---\n"));
  Serial.print(F("Total bytes written: "));
  Serial.println(wstatus.totalBytesWritten);
  Serial.print(F("Records processed: "));
  Serial.println(wstatus.recordCount);
  Serial.print(F("Errors: "));
  Serial.println(wstatus.errorCount);
  Serial.print(F("Start address: 0x"));
  Serial.println(addrcontext.startLinearAddress, HEX);
  Serial.println();
}
