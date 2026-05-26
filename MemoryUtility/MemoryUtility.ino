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
//#include <MCP23S17.h>
#include <ShiftRegister.h>

#include "promwriter.h"

#include "hex_processor.h"

// address bus 24 bit through SPI by 3 shift register 74hc595 (Output Expander/ShiftRegister),
// data bus 8 through SPI by MCP23S08 IO Expander
// Data buffer memory through SPI by SPI Serial SRAM 23C1024

#include "memory.h"

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

Memory targetmem(addrbus_cs, addrbus_oe, databus_cs, ROM_CE, ROM_OE, ROM_WE);

PROMWriter promwriter;

HexRecord record;

char buf128[128];
String line;
MemoryInfo meminfo;

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

void write_to_rom(const bool);

void setup() {

	Serial.begin(SERIAL_BAUD);
	while (!Serial) ;

	SPI.begin();
	pagearray.begin();
	targetmem.begin();

	while (!Serial) { }
	printWelcome();
	Serial.setTimeout(10000);

	line = "";
	get_meminfo_byindex(0, meminfo);

}

void loop() {
	uint32_t start, stop;
	char * ptr;

	if (readStringUntilCrLf(line, 256) > 0) {
		line.trim();
		if (line.length() == 0) {
			return; // go to the next itertion of the loop()
		}

		if (line[0] == '!') {
			// process the command
			switch (line[1]) {
			case 'C':
			case 'c':
				Serial.println();
				Serial.println(F("Start to load new data."));
				pagearray.clear();
				promwriter.clear();
				break;

			// case 'H':
			// case 'h':
			// 	printHelp();
			// 	break;

//			case 'X':
//			case 'x':
//				Serial.println();
//				Serial.println("Do test.");
//				unsigned long swatch = millis();
//				for(uint16_t i = 0; i < 40000; ++i) {
//					Memory::delay4clocks(200);
//				}
//				swatch = millis() - swatch;
//				Serial.print(swatch);
//				Serial.print(" millis.");
//				break;

			case 'S':
			case 's':
				show_status();
				break;

			case 'P':
				Serial.println();
				if ( line.length() >= 3 and line[2] == 'D' ) {
					targetmem.disable_SDP();
					Serial.println("Software data protection disabled.");
				} else if ( line.length() >= 3 and line[2] == 'E' ) {
					targetmem.enable_SDP();
					Serial.println("Software data protection enabled.");
				}
				break;

			case 'D':
			case 'd':
				Serial.println();
				Serial.println(F("Dump loaded data,"));
				line = line.substring(2);
				start = strtoul(line.c_str(), &ptr, 16);
				stop = strtoul(ptr, &ptr, 16);
				dump_auxmem(start, stop);
				Serial.println(F("Finished."));
				break;

			case 'R':
			case 'r':
				Serial.println();
				Serial.println(F("Reading target memory..."));
				line = line.substring(2);
				start = strtoul(line.c_str(), &ptr, 16);
				stop = strtoul(ptr, &ptr, 16);
				dump_target(start, stop);
				Serial.println(F("Finished."));
				break;

			case 'W':
			case 'w':
				Serial.println();
				line = line.substring(2);
				start = strtoul(line.c_str(), &ptr, 16);
				stop = strtoul(ptr, &ptr, 16);
				write_to_rom(start, stop);
				Serial.println(F("Finished."));
				break;

			case 'T':
			case 't':
				Serial.println();
				list_target_types();
				line = line.substring(2);
				line.trim();
				uint8_t id = 0;
				if ( line.length() > 0) {
					id = strtoul(line.c_str(), &ptr, 10);
					if ( id != 0 ) {
						get_meminfo_byindex(id, meminfo);
					} else {
						get_meminfo_byname(line.c_str(), meminfo);
					}
					Serial.println(F("Selected target memory:"));
				} else {
					Serial.println(F("Current target memory:"));
				}
				Serial.print(meminfo.partname);
				Serial.print(F("  "));
				switch(meminfo.type) {
					case SRAM:
					Serial.print(F("SRAM, "));
					break;
					case DRAM:
					case ROM: 		// mask rom
					break;
					case EPPROM: 	// UV-EPROM
					Serial.print(F("UV ePROM, "));
					break;
					case EEPROM:		// E-EPROM
					Serial.print(F("EEPROM, "));
					break;
					case FLASH: 
					Serial.print(F("Flash, "));
					break;
					default:
					Serial.print(F("Unknown, "));
					break;
				}
				Serial.print(meminfo.capacity_inbits>>13);
				Serial.print(F("K bytes, "));
				Serial.print(F("speed class "));
				Serial.print(meminfo.read_delay);
				Serial.print(", ");
				if (meminfo.page_size == 0) {
					Serial.print(F("no page write"));
				} else {
					Serial.print(meminfo.page_size);
					Serial.print(F(" bytes page write"));
				}
				if (meminfo.SDP) {
					Serial.println(F(", has SDP."));
				} else {
					Serial.println(F("."));
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

void write_to_rom(uint32_t startaddr, uint32_t stopaddr) {
	Page64 page;
	uint32_t lowestaddr = pagearray.lowest_address();
	uint32_t endaddr = pagearray.end_address();

	if (startaddr == 0 and stopaddr == 0) {
		startaddr = lowestaddr;
		stopaddr = endaddr;
	}
	if ( pagearray.size() == 0 or stopaddr - startaddr == 0) {
		Serial.println(F("No data to write."));
		return;
	}
//	Serial.print(F("pages "));
//	Serial.print(pagearray.size());
//	Serial.print(F(", start "));
//	Serial.print(startaddr, HEX);
//	Serial.print(F(", stop "));
//	Serial.println(stopaddr, HEX);

	uint32_t ix;
	for(ix = 0; ix < pagearray.size() ; ++ix) {
		pagearray.load(page, ix);
		if ( (startaddr > page.address + page.length - 1) or (page.address >= stopaddr) ) {
			continue;
		}
		bool err_flag = false;

		snprintf(buf128, 127, "%04lX ", page.address);
		Serial.print(buf128);

		uint32_t addrmask = meminfo.size_inbytes() - 1;
		// determine byte write or page write

		if ( page.address != (addrmask & page.address) ) {
			snprintf(buf128, 127, "(%04X) ", page.address & addrmask);
			Serial.print(buf128);
		}

		if ( meminfo.page_size == 0	// the target memory has no page write mode
				or (! page.is_aligned() ) // start address is not aligned
				) {
			//Serial.println(meminfo.page_size);
			//Serial.println(page.address, HEX);

			Serial.print("Byte write ");
			uint16_t i;
			for(i = 0; i < page.length; ++i) {
				bool succ = targetmem.program_byte_100ns( (page.address + i) & addrmask, page.data[i]);
				if ( succ ) {
					snprintf(buf128, 127, "%02X ", page.data[i]);
					Serial.print(buf128);
				} else {
					promwriter.errorCount += 1;
					err_flag = true;
					Serial.print(F("Error: Write failed at $"));
					Serial.println(page.address + i, HEX);
				}
			}
		} else {
			Serial.print("Page write ");
			bool succ = targetmem.program_page(page.address & addrmask, page.data, meminfo.page_size);
			if ( !succ ) {
				err_flag = true;
				promwriter.errorCount += 1;
                Serial.print("Error: Page write failed at 0x");
                snprintf(buf128, 127, "%04X", page.address & addrmask);
                Serial.println(buf128);
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

void dump_auxmem(uint32_t start, uint32_t stop) {
	Page64 page;
	if ( stop == 0 ) {
		stop = 0xffffffff;
	}
	uint32_t ix;
	for ( ix = 0; ix < pagearray.size(); ++ix) {
		pagearray.load(page, ix);
		if ( start <= page.address and page.address + page.length < stop) {
			page.printOn(Serial);
		}
	}
	Serial.println();
}

void dump_target(const uint32_t & startaddr, const uint32_t & stopaddr) {
	uint32_t addr = startaddr & 0xfffffff0;
	while ( addr < stopaddr ) {
        Serial.print(F("0x"));
        snprintf(buf128, 127, "%04X: ", addr);
        Serial.print(buf128);
        for (int i = 0; i < 16; ++i) {
            uint8_t val = targetmem.read_200ns(addr + i);
            snprintf(buf128, 127, "%02X ", val);
            Serial.print(buf128);
        }
        Serial.println();
        addr += 16;
	}
}

void printWelcome() {
	Serial.println(F("\n========================================"));
	Serial.println(F("   Arduino ihex/s19 Format Loader 202604"));
	Serial.println(F("========================================"));
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


