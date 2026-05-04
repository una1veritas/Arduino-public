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
#include <MCP23S08.h>
#include <MCP23S17.h>

#include "common.h"

#include "ihex_processor.h"
#include "srec_processor.h"

enum SPI_SLAVES {
  CS_MCP23S17 = 8,
  CS_MCP23S08 = 9,
  CS_23LC1024 = 10,
};

enum {
  MEM_CE = 14,  // A0, 10k pull-upped
  MEM_OE = 15,  // A1
  MEM_WE = 16,  // A2
};

SPISRAM spisram(CS_23LC1024, SPISRAM::BUS_MBits);  // CS pin
MCP23S17 addrbusx(CS_MCP23S17, 1);
MCP23S08 databusx(CS_MCP23S08, 0);

uint8_t auxmem_write(const uint32_t addr32, const uint8_t val8) {
	spisram.write(addr32, val8);
	return val8;
}

uint8_t auxmem_read(const uint32_t addr32) {
	return spisram.read(addr32);
}

uint8_t AT28C64_read(const uint16_t & addr) {
  uint8_t val;
  digitalWrite(MEM_WE, HIGH); // only to ensure
  addrbusx.write_GPIO16(addr);
  databusx.write_IODIR(databusx.IODIR_INPUT8);
  digitalWrite(MEM_CE, LOW);
  digitalWrite(MEM_OE, LOW);
  val = databusx.read_GPIO();
  digitalWrite(MEM_OE, HIGH);
  digitalWrite(MEM_CE, HIGH);
  return val;
}

// AT28C64 single byte write
uint8_t AT28C64_write(const uint16_t & addr, const uint8_t val) {
  digitalWrite(MEM_OE, HIGH); // to ensure
  addrbusx.write_GPIO16(addr);
  databusx.write_IODIR(databusx.IODIR_OUTPUT8);
  databusx.disable_pullup();
  digitalWrite(MEM_CE, LOW);
  digitalWrite(MEM_WE, LOW);
  __asm__ __volatile__ ("nop\n\t");   // 62.5ns for t_AH min = 50ns
  databusx.write_GPIO(val);
  __asm__ __volatile__ ("nop\n\t");   // 62.5ns for t_DS min = 50ns
  // t_WP > 100ns
  digitalWrite(MEM_WE, HIGH);
  digitalWrite(MEM_CE, HIGH);
  databusx.enable_pullup();
  databusx.write_IODIR(databusx.IODIR_INPUT8);

  // DATA polling to observe the end of write cycle
  uint8_t t;
  //uint8_t count = 0;
  digitalWrite(MEM_CE, LOW);  
  digitalWrite(MEM_OE, LOW);
  do {
    t = databusx.read_GPIO();
  } while ( t != val );
  digitalWrite(MEM_OE, HIGH);
  digitalWrite(MEM_CE, HIGH);
  //Serial.println(count);
  return t;
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

  // ensure to disable all the SPI slave devices. 
  pinMode(CS_23LC1024, OUTPUT); digitalWrite(CS_23LC1024, HIGH);
  pinMode(CS_MCP23S08, OUTPUT); digitalWrite(CS_MCP23S08, HIGH);
  pinMode(CS_MCP23S17, OUTPUT); digitalWrite(CS_MCP23S17, HIGH);

	// EEPROM control lines
  pinMode(MEM_CE, OUTPUT); digitalWrite(MEM_CE, HIGH);
  pinMode(MEM_WE, OUTPUT); digitalWrite(MEM_WE, HIGH);
  pinMode(MEM_OE, OUTPUT); digitalWrite(MEM_OE, HIGH);

  SPI.begin();

  databusx.begin();
  databusx.write_GPPU(databusx.GPPU_ENABLE8);
  databusx.write_IODIR(databusx.IODIR_INPUT8);

  addrbusx.begin();
  addrbusx.write_IODIR16(addrbusx.IODIR_OUTPUT16); // 1 input/0 output, 
  // A0 -- A12 is active, A13 is NC, A14 (pin 1) is NC or RDY/BUSY
  addrbusx.write_GPPU16(addrbusx.GPPU_DISABLE16); // 1 input/0 output, 

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
			} else if ( line.startsWith("!W") ) {
				Serial.println();
				Serial.println("Write loaded data to ROM:");
				auxmem_to_AT28C64();
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

void auxmem_to_AT28C64() {
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
			AT28C64_write(t.address + i, t.data[i]);
		}
		Serial.println(" Ok.");
		ix += t.datalength;
		rcount += 1;
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


