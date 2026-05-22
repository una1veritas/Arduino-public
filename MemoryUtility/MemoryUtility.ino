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

#include "hexline_processor.h"

// address bus 24 bit through SPI by 3 shift register 74hc595 (Output Expander/ShiftRegister),
// data bus 8 through SPI by MCP23S08 IO Expander
// Data buffer memory through SPI by SPI Serial SRAM 23C1024

#include "memory.h"


enum MEM_TYPE {
	SRAM = 1,
	DRAM = 2,
	ROM = 3, 		// mask rom
	EPPROM = 4, 	// UV-EPROM
	EEPROM = 5,		// E-EPROM
	FLASH = 6,
};

struct MEM_INFO {
	String partname;
	uint32_t capacity_inbits;
	uint8_t memtype;
	int16_t page_size;

	MEM_INFO& operator=(const MEM_INFO &src) {
		partname = src.partname;
		capacity_inbits = src.capacity_inbits;
		memtype = src.memtype;
		page_size = src.page_size;
		return *this;
	}
};

const MEM_INFO meminfo_db [] = {
		{ "AT28C64", 	Memory::EEPROM64KBITS, 	EEPROM, 	0 },
		{ "AT28C64B", 	Memory::EEPROM64KBITS, 	EEPROM, 	64 },
		{ "AT28C256", 	Memory::EEPROM256KBITS, EEPROM, 	64 },
		{ "HN58C256", 	Memory::EEPROM256KBITS, EEPROM, 	64 },
		{ "SRAM64KBITS", Memory::SRAM64KBITS, 	SRAM, 		0 },
		{ "SRAM1MBITS", Memory::SRAM1MBITS, 	SRAM, 		0 },
		{ "SRAM4MBITS", Memory::SRAM4MBITS, 	SRAM, 		0 },
		{ "", 0, 0, },
};

const MEM_INFO & get_meminfo_byname(const String & name) {
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
Memory targetmemory(Memory::EEPROM64KBITS);

ProgrammerStatus pgmstatus;
HexRecord record;

char buf128[128];
String line;
MEM_INFO meminfo;

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
	Serial.println("Hello.");

	SPI.begin();
	pagearray.begin();
	targetmemory.begin();

	while (!Serial) { }
	printWelcome();
	Serial.setTimeout(10000);

	line = "";
	meminfo = meminfo_db[1];
}

void loop() {
	char *ptr;
	uint32_t start, stop;
	if (readStringUntilCrLf(line, 256) > 0) {
		line.trim();
		if (line.length() == 0) {
			return; // go to the next itertion of the loop()
		}

		if (line[0] == '!') {
			// process the command
			switch (line[1]) {
			case 'D':
			case 'd':
				Serial.println();
				start = Stringtoul(line.substring(2), line, 16);
				stop = Stringtoul(line, line, 16);
				dump_auxmem(start, stop);
				Serial.println(F("Dump loaded data finished."));
				break;

			case 'L':
			case 'l':
				Serial.println();
				Serial.println(F("Start to load new data."));
				pagearray.clear();
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
				if ( line.length() >= 3 and line[2] == 'D' ) {
					targetmemory.disable_SDP();
					Serial.println("Software data protection disabled.");
				} else if ( line.length() >= 3 and line[2] == 'E' ) {
					targetmemory.enable_SDP();
					Serial.println("Software data protection enabled.");
				}
				break;

			case 'R':
			case 'r':
				Serial.println();
				start = Stringtoul(line.substring(2), line, 16);
				stop = Stringtoul(line, line, 16);
				dump_target(start, stop);
				Serial.println(F("Dump memory content finished."));
				break;

			case 'T':
			case 't':
				Serial.println();
				line = line.substring(2);
				line.trim();
				if (line.length() > 0) {
					MEM_INFO tmp = get_meminfo_byname(line);
					if (tmp.partname != "")
						meminfo = tmp;
				} else {
					Serial.println(F("Current target memory type:"));
				}
				Serial.println(meminfo.partname);
				break;

			case 'W':
				Serial.println();
				start = Stringtoul(line.substring(2), line, 16);
				stop = Stringtoul(line, line, 16);
				write_to_rom(start, stop);
				Serial.println(F("Finished."));
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

void write_to_rom(const uint32_t & startaddr, uint32_t stopaddr) {
	Page64 page;
	bool write_all = false;
	Serial.print("startaddr = $"); Serial.println(startaddr, HEX);
	Serial.print("stopaddr = $"); Serial.println(stopaddr, HEX);
	if ( startaddr == 0 and stopaddr == 0) {
		write_all = true;
	}
	uint32_t ix;
	for(ix = 0; ix < pagearray.size() ; ++ix) {
		pagearray.load(ix, page);
		if ( write_all = false and (page.address < startaddr or page.address > stopaddr) ) {
			continue;
		}
		bool err_flag = false;

		Serial.print("0x");
		if ( page.address >> 16 != 0 ) {
			snprintf(buf128, 127, "%04X", page.address >> 16 & 0xffff);
			Serial.print(buf128);
		}
		snprintf(buf128, 127, "%04X: ", page.address & 0xffff);
		Serial.print(buf128);

		uint32_t addrmask = targetmemory.size() - 1;
		// determine byte write or page write

		if ( meminfo.page_size == 0	// the target memory has no page write mode
				or (! page.is_aligned() ) // start address is not aligned
				or (! page.is_full() ) ) {
			//Serial.println(meminfo.page_size);
			//Serial.println(page.address, HEX);
			Serial.print("Byte write ");
			uint16_t i;
			for(i = 0; i < page.length; ++i) {
				bool succ = targetmemory.program_byte( (page.address + i) & addrmask, page.data[i]);
				if (! succ ) {
					pgmstatus.errorCount += 1;
					err_flag = true;
					Serial.print(F("Error: Write failed at $"));
					Serial.println(page.address + i, HEX);
				}
			}
		} else {
			Serial.print("Page write ");
			bool succ = targetmemory.program_page(page.address & addrmask, page.data, meminfo.page_size);
			if ( !succ ) {
				err_flag = true;
				pgmstatus.errorCount += 1;
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
	uint32_t rcount;
	uint32_t ix;
	for ( ix = 0, rcount = 0; ix < pagearray.size() and rcount < pgmstatus.recordCount; ++ix, ++rcount) {
		pagearray.load(ix, page);
		if ( start <= page.address and page.address - 1 + page.length <= stop) {
			page.printOn(Serial);
		}
	}
	Serial.println();
}

void dump_target(const uint32_t & startaddr, const uint32_t & stopaddr) {
	uint32_t addr = startaddr & 0xfffffff0;
	while ( addr <= stopaddr ) {
        Serial.print(F("0x"));
        snprintf(buf128, 127, "%04X: ", addr);
        Serial.print(buf128);
        for (int i = 0; i < 16; ++i) {
            uint8_t val = targetmemory.read(addr + i);
            snprintf(buf128, 127, "%02X ", val);
            Serial.print(buf128);
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


