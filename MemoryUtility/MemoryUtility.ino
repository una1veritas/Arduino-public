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
//#include <MCP23S17.h>
#include <ShiftRegister.h>

#include "memutil.h"

#include "memory.h"

#include "ihex_processor.h"
#include "srec_processor.h"

enum CAPACITY_IN_BITS {
  SRAM16KBITS   =   0x4000UL,   // 2k bytes
  SRAM64KBITS   =  0x10000UL,   // 8k bytes
  SRAM128KBITS  =  0x20000UL,   // 16k bytes
  SRAM832       =  0x40000UL,   // 32kbytes/256k bits
  SRAM256KBITS  =  0x40000UL,   // 32kbytes/256k bits
  SRAM512KBITS  =  0x80000UL,   // 64kbytes/512kbits
  SRAM1MBITS    = 0x100000UL,   // 64kbytes/512kbits
  SRAM8512      = 0x400000UL,   // 64kbytes/512kbits
  SRAM4MBITS    = 0x400000UL,   // 64kbytes/512kbits

  EEPROM64KBITS  = 0x10000UL,
  EEPROM256KBITS = 0x40000UL,
};

enum MEM_TYPE {
	SRAM = 1,
	DRAM = 2,
	ROM = 3,
	EPPROM = 4, 	// UV-EPROM
	EEPROM = 5,
	FLASH = 6,
};

struct MEM_INFO {
		String partname;
		CAPACITY_IN_BITS capacity_inbits;
		uint8_t memtype;
		int16_t page_size;

		MEM_INFO & operator=(const MEM_INFO & src) {
			partname = src.partname;
			capacity_inbits = src.capacity_inbits;
			memtype = src.memtype;
			page_size = src.page_size;
			return *this;
		}
};

const MEM_INFO meminfo_db [] = {
		{ "AT28C64", 	EEPROM64KBITS, 	EEPROM, 	0 },
		{ "AT28C64B", 	EEPROM64KBITS, 	EEPROM, 	64 },
		{ "AT28C256", 	EEPROM256KBITS, EEPROM, 	64 },
		{ "HN58C256", 	EEPROM256KBITS, EEPROM, 	64 },
		{ "SRAM64KBITS", SRAM64KBITS, 	SRAM, 		0 },
		{ "SRAM1MBITS", SRAM1MBITS, 	SRAM, 		0 },
		{ "SRAM4MBITS", SRAM4MBITS, 	SRAM, 		0 },
		{ "", 0, 0, },
};

const MEM_INFO & get_meminfo(const String & name) {
	uint8_t ix;
	for(ix = 0; meminfo_db[ix].partname != "" ; ++ix) {
		if ( meminfo_db[ix].partname.equalsIgnoreCase(name) )
			break;
	}
	return meminfo_db[ix];
}

// Configuration
#define SERIAL_BAUD 115200

const uint8_t SPISRAM_23LC1024_CS = 10;

PageArray pagearray(SPISRAM_23LC1024_CS);
Memory memory(EEPROM64KBITS);

ProgrammerStatus pgmstatus;
HexRecord record;

char buf128[128];
String line;
MEM_INFO meminfo;

bool char_isin(const char c, const char * str) {
	char * p;
	for (p = str; *p != 0 and *p != c ; ++p);
	return *p != 0;
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
	Serial.println("Hello.");

	SPI.begin();
	pagearray.begin();
	memory.begin();

	while (!Serial) { }
	printWelcome();
	Serial.setTimeout(10000);

	line = "";
	meminfo = meminfo_db[1];
}

void loop() {
	if ( readStringUntilCrLf(line, 256) > 0 ) {
		line.trim();
		if ( line.length() == 0 ) {
			return; // go to the next itertion of the loop()
		}

		if (line[0] == '!' ) {
			// process the command
			switch (line[1]) {
				case 'D':
				case 'd':
					Serial.println();
					line = line.substring(2);
					uint32_t start, stop;
					char * ptr;
					start = strtoul(line.c_str(), &ptr, 0);
					line = line.substring(ptr - line.c_str());
					stop = strtoul(line.c_str(), &ptr, 0);
					//Serial.println(start);
					//Serial.println(stop);
					dump_auxmem(start, stop);
					Serial.println(F("Dump loaded data finished."));
					break;

				case 'L':
				case 'l':
					Serial.println();
					Serial.println(F("Start to load new data."));
					pgmstatus.clear();
					break;

				case 'H':
				case 'h':
					printHelp();
					break;

				case 'X':
				case 'x':
					Serial.println();
					Serial.println("Do test.");
					break;

				case 'S':
				case 's':
					show_pgmstatus();
					break;

				case 'P':
					Serial.println();
					memory.disable_SDP();
					Serial.println("Software data protect disabled.");
					break;

				case 'T':
				case 't':
					Serial.println();
					line = line.substring(2);
					line.trim();
					if ( line.length() > 0 ) {
						MEM_INFO tmp = get_meminfo(line);
						if ( tmp.partname != "" )
							meminfo = tmp;
					} else {
						Serial.println(F("Current target memory type:"));
					}
					Serial.println(meminfo.partname);
					break;

				case 'W':
					Serial.println();
                    write_to_rom();
                    Serial.println(F("Finished."));
                    break;
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

	}
}

void write_to_rom() {
	Page64 page;
	uint32_t rcount;
	uint32_t ix;
	for(ix = 0, rcount = 0; ix < pagearray.size() and rcount < pgmstatus.recordCount; ++ix, ++rcount) {
		pagearray.load(ix, page);

		bool err_flag = false;

		Serial.print("0x");
		if ( page.address >> 16 != 0 ) {
			Serialsnprint(buf128, 127, "%04X", page.address >> 16 & 0xffff);
		}
		Serialsnprint(buf128, 127, "%04X", page.address & 0xffff);
		Serial.print(": ");

		uint32_t addrmask = memory.size() - 1;
		// determine byte write or page write

		if ( meminfo.page_size == 0
				or (! page.is_aligned() ) // start address is not aligned
				or (! page.is_full() ) ) {
			Serial.println(meminfo.page_size);
			Serial.println(page.address & (meminfo.page_size - 1), HEX);
			Serial.println(page.length % meminfo.page_size);
			Serial.println("Use byte write.");
		} else {
			Serial.println("Use page write.");
		}


//		if ( t.datalength < 63 or meminfo.page_size == 0 or page_write == false ) {
//			// supports only byte write
//			for(int i = 0; i < t.datalength; ++i) {
//				bool succ = memory.program_byte( (t.address + i) & addrmask , t.data[i]);
//				if (! succ ) {
//					pgmstatus.errorCount += 1;
//					err_flag = true;
//	                Serial.print("Error: Write failed at 0x");
//	                Serialsnprint(buf128, 127, "%04X", t.address + i);
//	                Serial.println();
//	            }
//			}
//		} else if ( meminfo.page_size > 0) {
//			bool succ = memory.program_page(t.address & addrmask, t.data, meminfo.page_size);
//			if ( !succ ) {
//				err_flag = true;
//				pgmstatus.errorCount += 1;
//                Serial.print("Error: Page write failed at 0x");
//                Serialsnprintln(buf128, 127, "%04X", t.address & addrmask);
//            }
//		} else {
//			Serial.println("no programming method.");
//			break;
//		}

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
	uint32_t rcount;
	uint32_t ix;
	for ( ix = 0, rcount = 0; ix < pagearray.size() and rcount < pgmstatus.recordCount; ++ix, ++rcount) {
		pagearray.load(ix, page);
		if ( start <= page.address and page.address - 1 + page.length <= stop) {
			bool first = true;
			for(int i = 0; i < page.length; ++i) {
				if ( ((page.address + i) & 0x000f) == 0 or first ) {
					Serial.println();
					Serial.print(F("0x"));
					if ( (page.address + i) >> 16 != 0 ) {
						Serialsnprint(buf128, 127, "%04X", (page.address + i) >> 16);
					}
					Serialsnprint(buf128, 127, "%04X: ", (page.address + i) & 0xffff);
					first = false;
				}
				Serialsnprint(buf128, 127, "%02X ", page[i]);

			}
		}
	}
	Serial.println();
}

void dump_target(const uint32_t & startaddr, const uint32_t & stopaddr) {
	uint32_t addr = startaddr & 0xfffffff0;
	while ( addr <= stopaddr ) {
        Serial.print(F("0x"));
        Serialsnprint(buf128, 127, "%04X", addr);
        Serial.print(": ");
        for (int i = 0; i < 16; ++i) {
            uint8_t val = memory.read(addr + i);
            Serialsnprint(buf128, 127, "%02X ", val);
        }
        Serial.println();
        if ( addr == stopaddr ) {
            break;
        }
        addr += 16;
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
	Serial.print(F("Target memory type: "));
	Serial.println(meminfo.partname);
	Serial.println();
}


