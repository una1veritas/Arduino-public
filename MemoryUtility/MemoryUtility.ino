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

//#include <Arduino.h>

#include <SPI.h>
#include <SPISRAM.h>

#include <MCP23S08.h>
#include <ShiftRegister.h>

#include "exbusmemory.h"
#include "promwriter.h"

#include "hex_processor.h"

// address bus 24 bit through SPI by 3 shift register 74hc595 (Output Expander/ShiftRegister),
// data bus 8 through SPI by MCP23S08 IO Expander
// Data buffer memory through SPI by SPI Serial SRAM 23C1024


enum PIN_ASSIGNMENT {
	addrbus_cs = 8,
	addrbus_oe = 7,
	databus_cs = 9,
	ROM_CE = A1,
	ROM_OE = A2,
	ROM_WE = A0,

	SPISRAM_23LC1024_CS = 10,
};

// Configuration
#define SERIAL_BAUD 115200

PageArray pagearray(SPISRAM_23LC1024_CS);

ExBusMemory exbusmem(addrbus_cs, addrbus_oe, databus_cs, ROM_CE, ROM_OE, ROM_WE);

PROMWriter promwriter;

HexRecord record;

char buf128[128];
String line;
MemoryInfo meminfo;


void memory_type(MemoryInfo & meminfo, const char * s) {
	uint8_t val = strtoul(s, NULL, 10);
	if ( strlen(s) != 0 and val == 0 ) {
		get_meminfo_byname(s, meminfo);
	} else if ( strlen(s) != 0 ){
		get_meminfo_byindex(val, meminfo);
	}
	list_target_types(meminfo);
	meminfo.printOn(Serial);
}

void program_rom(uint32_t startaddr, uint32_t stopaddr) {
	PageBuffer page;
	uint32_t ix;
	uint32_t addrmask = meminfo.size_inbytes() - 1;

	for(ix = 0; ix < pagearray.size() ; ++ix) {

		pagearray.get_byindex(ix, page);

		if ( (page.address + page.length - 1 < startaddr) or ( stopaddr <= page.address ) ) {
			continue;
		}

		bool err_flag = false;

		snprintf(buf128, 127, "%04lX ", page.address);
		Serial.print(buf128);
		if ( page.address != (addrmask & page.address) ) {
			snprintf(buf128, 127, "(%04X) ", page.address & addrmask);
			Serial.print(buf128);
		}

		// determine byte write or page write
		if ( page.is_aligned() and page.length == meminfo.page_size ) {
			Serial.print("Page write ");

			bool succ = exbusmem.program_page(page.address & addrmask, page.bytes, meminfo.page_size);
			if ( !succ ) {
				err_flag = true;
				promwriter.errorCount += 1;
                Serial.print("Error: Page write failed at ");
                snprintf(buf128, 127, "%04X", page.address & addrmask);
                Serial.println(buf128);
            }
		} else {
			Serial.print("Byte write ");

			for(uint16_t i = 0; i < page.length; ++i) {
				bool succ = exbusmem.program_byte( (page.address + i) & addrmask, page.bytes[i]);
				if ( succ ) {
					Serial.print('.');
				} else {
					promwriter.errorCount += 1;
					err_flag = true;
					Serial.print(F("Error: Write failed at "));
					Serial.print(page.address + i, HEX);
					Serial.println(F("H"));
				}
			}
		}

		if (not err_flag) {
			Serial.println(" Ok.");
		} else {
			Serial.println(" Stop writing to ROM.");
            break;
		}
	}
}

void read_pagearray(uint32_t start, uint32_t stop) {
	PageBuffer page;
	if ( stop == 0 ) {
		stop = 0xffffffff;
	}
	uint32_t ix;
	for ( ix = 0; ix < pagearray.size(); ++ix) {
		pagearray.get_byindex(ix, page);
		if ( start <= page.address and page.address + page.length - 1 <= stop) {
			page.printOn(Serial);
		}
	}
	Serial.println();
}

void read_targetmem(const uint32_t & startaddr, const uint32_t & stopaddr) {
	uint32_t addr = startaddr & 0xfffffff0;
	uint8_t val;
	while ( addr <= stopaddr ) {
        snprintf(buf128, 127, "%04X ", addr);
        Serial.print(buf128);
        for (int i = 0; i < 16; ++i) {
        	if (startaddr <= addr + i and addr + i <= stopaddr) {
				if (meminfo.access_time <= 100) {
					val = exbusmem.read(addr + i);
				} else {
					val = exbusmem.read_rom(addr + i);
				}
				snprintf(buf128, 127, "%02X ", val);
				Serial.print(buf128);
        	} else {
        		Serial.print(F("   "));
        	}
        }
        Serial.println();
        addr += 16;
	}
}

void sram_test() {
	Serial.println(F("\nSRAM R/W test"));
	uint32_t start = 0;
	uint32_t end = meminfo.capacity_inbits >> 3 ;
	uint32_t errcount = 0;
	uint32_t tmperr = 0;

	const uint32_t block_size = end > 0x1000 ? 0x1000 : end;
	for(uint32_t base_addr = start; base_addr < end; base_addr += block_size) {
		snprintf(buf128, 127, "%04X", base_addr);
		Serial.print(buf128);
		Serial.print(" -- ");
		snprintf(buf128, 127, "%04X", base_addr + block_size -1);
		Serial.print(buf128);
		Serial.print(" : ");
		Serial.flush();
		tmperr += exbusmem.sram_check(base_addr, block_size);
		if ( tmperr == 0 ) {
			Serial.println(F("OK."));
		} else {
			Serial.print(tmperr);
			Serial.println(F(" errors."));
		}
		errcount += tmperr;
		if ( errcount > block_size ) {
			Serial.println(F("Too many errors, abandon."));
			break;
		}
	}
	Serial.print(F("Total error count = "));
	Serial.println(errcount);
	Serial.println();
}

void printWelcome() {
	Serial.println(F("\n========================================="));
	Serial.println(F(" Arduino ihex/s19 Format Loader 20260603"));
	Serial.println(F("========================================="));
}

void show_status() {
	Serial.println();
	Serial.println(F("--- Status and Statistics ---"));
	Serial.print(F("Records processed: "));
	Serial.println(pagearray.size());
	Serial.print(F("Record errors: "));
	Serial.println(promwriter.errorCount);
	Serial.print(F("Checksum errors: "));
	Serial.println(promwriter.checksumErrors);
	Serial.print(F("Total bytes loaded: "));
	Serial.println(promwriter.totalBytesWritten);
	Serial.print(F("Target memory type: "));
	Serial.println(meminfo.partname);
	Serial.println();
}


// command processors
bool char_isin(const char c, const char * str) {
	const char * p;
	for (p = str; *p != 0 and *p != c ; ++p);
	return *p != 0;
}

uint32_t Stringtoul(const String & line, String & suffix, uint8_t fmt) {
	char * ptr;
	uint32_t valu32 = strtoul(line.c_str(), &ptr, fmt);
	suffix = String(ptr);
	suffix.trim();
	return valu32;
}

unsigned int readStringUntilCrLf(String &line, const unsigned int & limit = 256, const unsigned long & timeout = 10000) {
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
	while (!Serial) ;

	SPI.begin();
	pagearray.begin();
	exbusmem.begin();

	while (!Serial) { }
	printWelcome();
	Serial.setTimeout(10000);

	line = "";
	get_meminfo_byindex(0, meminfo);

}

void loop() {
	uint8_t data, val;
	uint32_t addr, start, stop;
	char * ptr;

	if (readStringUntilCrLf(line, 256) > 0) {
		line.trim();
		if (line.length() == 0) {
			return; // go to the next itertion of the loop()
		}
		if (line[0] == '!') {
			Serial.println();
			// process the command
			switch (line[1]) {
			case 'B':
				sram_test();
				break;

			case 'C':
			case 'c':
				Serial.println(F("Start to load new data."));
				pagearray.clear();
				promwriter.clear();
				break;

			case 'O':
			case 'o':
				Serial.println(F("Organize pages..."));
				pagearray.sort_pages();
				Serial.println(F("Done."));
				break;

			case 'S':
			case 's':
				show_status();
				break;

			case 'P':
				Serial.println(F("Software data protection "));
				if ( line.length() >= 3 and line[2] == 'D' ) {
					exbusmem.disable_SDP();
					Serial.println(F(" disabled."));
				} else if ( line.length() >= 3 and line[2] == 'E' ) {
					exbusmem.enable_SDP();
					Serial.println(F(" enabled."));
				}
				break;

			case 'D':
			case 'd':
				Serial.println(F("Dump loaded data,"));
				line = line.substring(2);
				start = strtoul(line.c_str(), &ptr, 16);
				stop = strtoul(ptr, &ptr, 16);
				read_pagearray(start, stop);
				Serial.println(F("Finished."));
				break;

			case 'R':
			case 'r':
				Serial.println(F("Reading target memory..."));
				line = line.substring(2);
				start = strtoul(line.c_str(), &ptr, 16);
				stop = strtoul(ptr, &ptr, 16);
				read_targetmem(start, stop);
				Serial.println(F("Finished."));
				break;

			case 'T':
			case 't':
				line = line.substring(2);
				line.trim();
				memory_type(meminfo, line.c_str());
				break;

			case 'W':
			case 'w':
				line = line.substring(2);
				start = strtoul(line.c_str(), &ptr, 16);
				stop = strtoul(ptr, &ptr, 16);
				if ( stop == 0 or start >= stop ) {
					start = pagearray.lowest_address();
					stop = pagearray.highest_address();
				}
				if (  start < stop ) {
					program_rom(start, stop);
				} else {
					Serial.println(F("No data to write."));
				}
				Serial.println(F("Done."));
				break;

			case 'X':
			case 'x':
				Serial.println("Memory power ");
				if ( !promwriter.target_power ) {
					exbusmem.begin();
					promwriter.target_power  = true;
					Serial.println("on.");
				} else {
					exbusmem.end();
					promwriter.target_power  = false;
					Serial.println("off.");
				}
				break;

			}
		} else if (line[0] == ':') {
			// Process Intel HEX record
			processiHexRecord(line, record);
		} else if (line[0] == 'S') {
			processS19Record(line, record);
		} else {
			Serial.print(F("comment: "));
			Serial.println(line);
		}

		line = "";

	}
}


