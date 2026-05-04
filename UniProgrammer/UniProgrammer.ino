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

SPISRAM spisram(CS_23LC1024, SPISRAM::BUS_MBits);  // CS pin

uint8_t auxmem_write(const uint32_t addr32, const uint8_t val8) {
	spisram.write(addr32, val8);
	return val8;
}

uint8_t auxmem_read(const uint32_t addr32) {
	return spisram.read(addr32);
}

// Configuration
#define SERIAL_BAUD 115200

ProgrammerStatus pgmstatus;
HexRecord record;

char buf128[128];
String line;

unsigned long idleStart;
const unsigned long timeout = 5000;

bool char_isin(const char c, const char * str) {
	char * p;
	for (p = str; *p != 0 and *p != c ; ++p);
	return *p != 0;
}

unsigned int readStringUntilCrLf(String &line, unsigned int limit = 256) {
	char c;
	line = "";
	unsigned int n = 0;
	unsigned long idlestart = millis();

	while (n < limit and millis() - idlestart < timeout) {
		if (Serial.available()) {
			idlestart = millis();
			c = Serial.read();
			// Check for both CR or LF
			if (char_isin(c, "\x0a\x0d\x1b")) {
				if (line.length() > 0 )
					break;
				continue;
			}
			if ( (line.length() == 0 and c == '!') or (line.length() > 0 and line[0] == '!') ) {
				if (isprint(c)) {
					Serial.print(c);
					line += c;
				} else if (c == '\x7f') {
					// BS/delete
					Serial.print("\x08 \x08");
					line.remove(line.length() - 1);
					//Serial.println(content);
				}
			} else {
				line += c;
			}
		}
	}
	return line.length();
}

void setup() {
	Serial.begin(SERIAL_BAUD);

	pinMode(CS_23LC1024, OUTPUT);
	digitalWrite(CS_23LC1024, HIGH);
	SPI.begin();
	spisram.begin();

	while (!Serial) {}
	printWelcome();
	Serial.setTimeout(10000);

//	char str[] = "S9030000FC";
//
//	for (int i = 0; i < strlen(str) + 1; ++i) {
//		auxmem_write(i, 0xff);
//	}
//	for (int i = 0; i < strlen(str) + 1; ++i) {
//		auxmem_write(i, str[i]);
//	}
//	for (int i = 0; i < strlen(str); ++i) {
//		Serial.print((char)auxmem_read(i));
//	}
//	Serial.println();
	
	//pgmstatus.clear();
	idleStart = millis();
	line = "";
}

void loop() {
	if ( readStringUntilCrLf(line, 256) > 0 ) {
		line.trim();
		if ( line.length() == 0 ) {
			return;
		}
		if (line[0] == '!' ) {
			if (line.startsWith("!L") ) {
				Serial.println();
				Serial.println(F("Start to load new data."));
				pgmstatus.clear();
				idleStart = millis();
			} else if (line.startsWith("!S") ) {
				show_pgmstatus();
				idleStart = millis();
			} else if ( line.startsWith("!D") ) {
				Serial.println();
				Serial.println("Dump loaded data:");
				dump_auxmem();
			} else if ( line.startsWith("!H") ) {
				printHelp();
				idleStart = millis();
			} else if ( line.startsWith("!C")) {
				//clearWriterStatus();
				idleStart = millis();
			}
		} else if (line[0] == ':') {
			// Process Intel HEX record
			processIHexRecord(line, record);
		} else if (line[0] == 'S') {
			processS19Record(line, record);
		} else {
			Serial.print(F("comment: "));
			Serial.println(line);
		}

		line = "";
		// // Timeout after 30 seconds of inactivity
		// if (millis() - idleStart > 30000 ) {
		// 	Serial.println(
		// 			F("Loading timeout: No data received for 30 seconds."));
		// }
	}
}

void dump_auxmem() {
	HexRecord t;
	uint32_t rcount = 0;
	uint32_t ix = 0;
	while ( auxmem_read(ix) != 0 and rcount < pgmstatus.recordCount) {
		char * p = (char *) & t;
		for(int i = 0; i < HexRecord::header_size(); ++i) {
			*(p + i) = auxmem_read(ix + i);
		}
		ix += HexRecord::header_size();
		for (int i = 0; i < t.datalength; ++i) {
			t.data[i] = auxmem_read(ix + i);
		}
		if ( t.address >> 16 != 0 ) {
			Serial.print(t.address>>16, HEX);
		}
		Serial.print(t.address>>12 & 0x0f, HEX);
		Serial.print(t.address >> 8 & 0x0f, HEX);
		Serial.print(t.address >> 4 & 0x0f, HEX);
		Serial.print(t.address & 0x0f, HEX);
		Serial.print(": ");
		for(int i = 0; i < t.datalength; ++i) {
			Serial.print(t.data[i]>>4 & 0x0f, HEX);
			Serial.print(t.data[i] & 0x0f, HEX);
			Serial.print(" ");
		}
		Serial.print("(");
		Serial.print(t.datalength, DEC);
		Serial.print(")");
		Serial.println();

		ix += t.datalength;
		rcount += 1;
	}
}


void printWelcome() {
	Serial.println(F("\n========================================"));
	Serial.println(F("   Arduino ihex/s19 Format Loader 202604"));
	Serial.println(F("========================================"));
}

void printHelp() {
	Serial.println(F("Commands:"));
	Serial.println(F("  !L - Wait and load hex data from UART"));
	Serial.println(F("  !P - Show statistics"));
	Serial.println(F("  !V - Verify loaded data"));
	Serial.println(F("  !C - Clear statistics"));
	Serial.println(F("  !H - Help"));
	Serial.println(F("========================================\n"));
}

void show_pgmstatus() {
	Serial.println();
	Serial.println(F("--- Status and Statistics ---"));
	Serial.print(F("Records processed: "));
	Serial.println(pgmstatus.recordCount);
	Serial.print(F("Record errors: "));
	Serial.println(pgmstatus.errorCount);
	Serial.print(F("Checksum errors: "));
	Serial.println(pgmstatus.checksumErrors);
	Serial.print(F("Total bytes loaded: "));
	Serial.println(pgmstatus.totalBytesWritten);
	Serial.print(F("start_ix: "));
	Serial.println(pgmstatus.start_ix);
	Serial.println();
}


