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

SPISRAM auxsram(CS_23LC1024, SPISRAM::BUS_MBits);  // CS pin
MCP23S17 addrbusx(CS_MCP23S17, 1);
MCP23S08 databusx(CS_MCP23S08, 0);

enum MEM_TYPE {
	ROM_AT28C64 = 0,
	ROM_AT28C64B = 1,
	ROM_HN58C256 = 2, 	// P-20
	SRAM = 0x81,	//  HM62256ALP-10
	ROM_UNKNOWN = 0xff,
};

uint8_t rom_type = ROM_UNKNOWN;

enum ADDRESS_MASK {
	ADDRMASK_64K = 0x1fff, // 8KB/13 bits
	ADDRMASK_256K = 0x7fff,
};

uint32_t get_addr_mask(const uint8_t & rom_type) {
    switch (rom_type) {
    case ROM_AT28C64:
    case ROM_AT28C64B:
        return ADDRMASK_64K;
    case ROM_HN58C256:
        return ADDRMASK_256K;
    default:
        return 0xffff;
    }
}

// common CE/OE controlled RAM/ROM byte read
uint8_t mem_read(const uint16_t & addr) {
  uint8_t val;
  digitalWrite(MEM_WE, HIGH); // only to ensure
  addrbusx.write_GPIO16(addr);
  databusx.write_IODIR(databusx.IODIR_INPUT8);
  // assumes pull-up for data bus is active
  digitalWrite(MEM_CE, LOW);
  digitalWrite(MEM_OE, LOW);
  __asm__ __volatile__ ("nop\n\t");   // t_OE = max 50ns, 62.5ns if needed
  val = databusx.read_GPIO();
  digitalWrite(MEM_OE, HIGH);
  digitalWrite(MEM_CE, HIGH);
  return val;
}

// AT28C64 single byte write
uint8_t eeprom_byte_write(const uint16_t &addr, const uint8_t val) {
	digitalWrite(MEM_OE, HIGH); // to ensure
	digitalWrite(MEM_WE, HIGH); // to ensure puls
	addrbusx.write_GPIO16(addr);
	databusx.write_IODIR(databusx.IODIR_OUTPUT8);
	databusx.disable_pullup();
	databusx.write_GPIO(val);
	digitalWrite(MEM_CE, LOW);
	digitalWrite(MEM_WE, LOW);
	__asm__ __volatile__ ("nop\n\t"); 	// 62.5ns for t_AH min = 50ns
	__asm__ __volatile__ ("nop\n\t"); 	// 62.5ns for t_DS min = 50ns
	// t_WP > 100ns
	digitalWrite(MEM_WE, HIGH);
	__asm__ __volatile__ ("nop\n\t");
	__asm__ __volatile__ ("nop\n\t");
	// t_WPH = min 100ns
	digitalWrite(MEM_CE, HIGH);

	// restore default i/o mode
	databusx.enable_pullup();
	databusx.write_IODIR(databusx.IODIR_INPUT8);

	// verify the written byte
	//if (rom_type == ROM_AT28C64 or rom_type == ROM_AT28C64B or rom_type == ROM_HN58C256) {
	// DATA polling to observe the end of write cycle.
	uint8_t t;
	unsigned long start_millis = millis();
	do {
		delayMicroseconds(1); // t_WC write cycle time MAX = 10ms
		// assumes pull-up for data bus is active
		digitalWrite(MEM_CE, LOW);
		digitalWrite(MEM_OE, LOW);
		__asm__ __volatile__ ("nop\n\t");
		// t_OE = max 50ns, 62.5ns if needed
		t = databusx.read_GPIO();
		digitalWrite(MEM_OE, HIGH);
		digitalWrite(MEM_CE, HIGH);
	} while (t != val and millis() - start_millis < 150); // t_WC write cycle time MAX = 10ms
	return t;
}

// AT28C64B 6 bit border aware page write.
bool eeprom_page64_write(const uint16_t & start_addr, const uint8_t * valptr, const uint8_t & n) {
  digitalWrite(MEM_OE, HIGH); // to ensure
  digitalWrite(MEM_WE, HIGH); // to ensure puls
  Serial.println("starts sequence.");
  uint32_t addr; 	// 6 bit in the page

  uint8_t val = 0, t = 0;
  uint32_t lastaddr;
  for (addr = start_addr; addr < start_addr + n; ) {
	  databusx.write_IODIR(databusx.IODIR_OUTPUT8);
	  databusx.disable_pullup();
	  uint32_t currentpage = addr & 0xffc0;
	  for ( ; addr < start_addr + n and addr < currentpage + 64 ; ++addr) {
		  lastaddr = addr;
		  val= *valptr++;
		  addrbusx.write_GPIO16(lastaddr);
		  databusx.write_GPIO(val);
		  digitalWrite(MEM_CE, LOW);
		  digitalWrite(MEM_WE, LOW);
			__asm__ __volatile__ ("nop\n\t");
			__asm__ __volatile__ ("nop\n\t");
		  digitalWrite(MEM_WE, HIGH);
		  digitalWrite(MEM_CE, HIGH);
	  }
	  //Serialsnprintln(buf128, 127, "page %04x", currentpage);
	  // DATA polling to observe the end of write cycle.
	  databusx.enable_pullup();
	  databusx.write_IODIR(databusx.IODIR_INPUT8);
	  digitalWrite(MEM_CE, LOW);
	  unsigned long start_millis = millis();
	  do {
		  delayMicroseconds(1); // t_WC write cycle time MAX = 10ms
		  digitalWrite(MEM_OE, LOW);
		  __asm__ __volatile__ ("nop\n\t");   // 62.5ns if needed
		  t = databusx.read_GPIO();
		  digitalWrite(MEM_OE, HIGH);
	  } while ( t != val and millis() - start_millis < 200); // t_WC write cycle time MAX = 10ms
	  digitalWrite(MEM_CE, HIGH);
	  //Serialsnprintln(buf128, 127, "count %d", count);
	  currentpage = addr & 0xffc0;
	  if ( addr < start_addr + n)
		  break;
  }
  digitalWrite(MEM_CE, HIGH);
  return t != val;
}


void rom_SDP_set(const bool &enable) {
	// AT28C64B has software data protection (SDP) feature, which requires a series of write-commands.
	// This function is used to enable or disable the SDP feature.
	uint8_t buf[64];
	for(int i = 0; i < 64; ++i) {
        buf[i] = mem_read(i);
    }
	uint32_t addrmask = get_addr_mask(rom_type);
	if (enable) {
		eeprom_byte_write(0x5555 & addrmask, 0xaa);
		eeprom_byte_write(0x2aaa & addrmask, 0x55);
		eeprom_byte_write(0x5555 & addrmask, 0xa0);
		for(int i = 0; i < 64; ++i) {
            eeprom_byte_write(i, buf[i]);
        }
	} else {
		Serial.println(eeprom_byte_write(0x5555 & addrmask, 0xaa), HEX);
		Serial.println(eeprom_byte_write(0x2aaa & addrmask, 0x55), HEX);
		Serial.println(eeprom_byte_write(0x5555 & addrmask, 0x80), HEX);
		Serial.println(eeprom_byte_write(0x5555 & addrmask, 0xaa), HEX);
		Serial.println(eeprom_byte_write(0x2aaa & addrmask, 0x55), HEX);
		Serial.println(eeprom_byte_write(0x5555 & addrmask, 0x20), HEX);
		/*
		for(int i = 0; i < 64; ++i) {
			Serial.print(rom_write(i, buf[i]) == buf[i]);
            Serial.print(" ");
            if ( (i & 0x0f) == 0x0f ) {
                Serial.println();
            }
        }
		*/
	}
	return;
}


// Configuration
#define SERIAL_BAUD 115200

ProgrammerStatus pgmstatus;
HexRecord record;

char buf128[128];
String line;

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

	auxsram.begin();

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
	line = "";
	rom_type = ROM_UNKNOWN;
}

void loop() {
	if ( readStringUntilCrLf(line, 256) > 0 ) {
		line.trim();
		if ( line.length() == 0 ) {
			return;
		}
		if (line[0] == '!' ) {
			if (line.startsWith("!D")) {
				Serial.println();
				dump_auxmem();
				Serial.println(F("Dump loaded data finished."));

			} else if (line.startsWith("!L")) {
				Serial.println();
				Serial.println(F("Start to load new data."));
				pgmstatus.clear();

			} else if (line.startsWith("!H")) {
				printHelp();

			} else if (line.startsWith("!PD")) {
				Serial.println();
				rom_SDP_set(false);
				Serial.println(F("Software protection disabled."));

			} else if (line.startsWith("!PE")) {
				Serial.println();
				rom_SDP_set(true);
				Serial.println(F("Software protection enabled."));

			} else if (line.startsWith("!R")) {
				Serial.println();
				Serial.println(F("Read memory:"));
				line = line.substring(2);
				line.trim();
	            char * ptr;
				uint32_t startaddr = strtoul(line.c_str(), &ptr, 0);
				line = line.substring(ptr - line.c_str());
				line.trim();
				Serial.println(line);
				uint32_t stopaddr = strtoul(line.c_str(), &ptr, 0);
				Serialsnprintln(buf128, 127, "from %04X to %04X", startaddr, stopaddr);
				dump_target(startaddr, stopaddr);
				Serial.println(F("Finished."));

			} else if (line.startsWith("!S")) {
				show_pgmstatus();

			} else if (line.startsWith("!T")) {
				Serial.println();
				Serial.println(F("Target memory type:"));
				line = line.substring(2);
				line.trim();
	            uint8_t type_id;
	            if ( line.length() > 0 ) {
	            	type_id  = (uint8_t) strtol(line.c_str(), NULL, 0);
					switch (type_id) {
					case ROM_AT28C64:
						Serial.println(F("AT28C64 (8KB)"));
						rom_type = ROM_AT28C64;
						break;
					case ROM_AT28C64B:
						Serial.println(F("AT28C64B (8KB with software data protection)"));
						rom_type = ROM_AT28C64B;
						break;
					case ROM_HN58C256:
						Serial.println(F("HN58C256 (32KB)"));
						rom_type = ROM_HN58C256;
						break;
					default:
						Serial.print(F("Unknown type id: "));
						Serial.println(type_id);
						rom_type = ROM_UNKNOWN;
						break;
					}
	            } else {
	            	Serial.println(rom_type);
	            }

			} else if (line.startsWith("!W")) {
				Serial.println();
				write_to_rom(get_addr_mask(rom_type));
				Serial.println(F("Finished."));
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

void write_to_rom(const uint16_t & addr_mask) {
	HexRecord t;
	uint32_t rcount = 0;
	uint32_t ix = 0;
	while ( auxsram.read(ix) != 0 and rcount < pgmstatus.recordCount) {
		char * p = (char *) & t;
		for(int i = 0; i < HexRecord::header_size(); ++i) {
			*(p + i) = auxsram.read(ix + i);
		}
		ix += HexRecord::header_size();
		for (int i = 0; i < t.datalength; ++i) {
			t.data[i] = auxsram.read(ix + i);
		}
		bool err_flag = false;
		Serial.print("0x");
		if ( t.address >> 16 != 0 ) {
			Serialsnprint(buf128, 127, "%04X", t.address >> 16 & 0xffff);
		}
		Serialsnprint(buf128, 127, "%04X", t.address & 0xffff);
		Serial.print(": ");
		uint32_t addrmask = get_addr_mask(rom_type);
		if (rom_type == ROM_AT28C64 or rom_type == ROM_UNKNOWN) {
			// supports only byte write
			for(int i = 0; i < t.datalength; ++i) {
				uint8_t wval = eeprom_byte_write( (t.address + i) & addrmask , t.data[i]);
				if (wval != t.data[i]) {
					pgmstatus.errorCount += 1;
					err_flag = true;
	                Serial.print("Error: Write failed at 0x");
	                Serialsnprint(buf128, 127, "%04X", t.address + i);
	                Serial.print(": expected 0x");
	                Serialsnprint(buf128, 127, "%02X", t.data[i]);
	                Serial.print(" but results 0x");
	                Serialsnprint(buf128, 127, "%02X", wval);
	                Serial.println();
	            }
			}
		} else if (rom_type == ROM_AT28C64B or rom_type == ROM_HN58C256) {
			err_flag = eeprom_page64_write(t.address & addrmask, t.data, t.datalength);
			if ( err_flag == false) {
				pgmstatus.errorCount += 1;
                Serial.print("Error: Write failed at 0x");
                Serialsnprintln(buf128, 127, "%04X", t.address & addrmask);
            }
		}

		if (not err_flag) {
			Serial.println(" Ok.");
		} else {
			Serial.println(" Stop writing to ROM.");
            break;
		}
		ix += t.datalength;
		rcount += 1;
	}
}

void dump_auxmem() {
	HexRecord t;
	uint32_t rcount = 0;
	uint32_t ix = 0;
	while ( auxsram.read(ix) != 0 and rcount < pgmstatus.recordCount) {
		char * p = (char *) & t;
		for(int i = 0; i < HexRecord::header_size(); ++i) {
			*(p + i) = auxsram.read(ix + i);
		}
		ix += HexRecord::header_size();
		for (int i = 0; i < t.datalength; ++i) {
			t.data[i] = auxsram.read(ix + i);
		}
		bool first = true;
		for(int i = 0; i < t.datalength; ++i) {
			if ( ((t.address + i) & 0x000f) == 0 or first ) {
				Serial.println();
				Serial.print(F("0x"));
				if ( (t.address + i) >> 16 != 0 ) {
					Serialsnprint(buf128, 127, "%04X", (t.address + i) >> 16);
				}
				Serialsnprint(buf128, 127, "%04X: ", (t.address + i) & 0xffff);
		        first = false;
			}
			Serialsnprint(buf128, 127, "%02X ", t.data[i]);

		}
		ix += t.datalength;
		rcount += 1;
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
            uint8_t val = mem_read(addr + i);
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
	Serial.println(rom_type);
	Serial.println();
}


