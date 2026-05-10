 9009099/*
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
MCP23S17 addrbus_iox(CS_MCP23S17, 1);
MCP23S08 databus_iox(CS_MCP23S08, 0);

enum MEM_TYPE {
	ROM_AT28C64 = 1,
	ROM_AT28C64B = 2,
	ROM_AT28C256 = 3,
	ROM_HN58C256 = 4, 	// P-20
	SRAM_256Kbit = 129,	//  HM62256ALP-10

	ROM_UNKNOWN = 0,
};

struct ROMINFO {
	uint8_t type_id;
	uint32_t capacity_bytes;
    uint16_t page_size;
    char name[16];
};

ROMINFO rominfo[] = {
    { ROM_AT28C64, 8 * 1024UL, 0, "AT28C64" },
    { ROM_AT28C64B, 8 * 1024UL, 64, "AT28C64B" },
    { ROM_AT28C256, 32 * 1024UL, 64, "AT28C256" },
    { ROM_HN58C256, 32 * 1024UL, 64, "HN58C256" },
    { 0, 0, 0, ""},
};

const ROMINFO & get_rominfo(const uint8_t id) {
	uint8_t ix;
	for(ix = 0; rominfo[ix].type_id != 0 and rominfo[ix].type_id != id ; ++ix) { }
	return rominfo[ix];
}

struct ProgrammableROM {
private:
	uint8_t _CE, _OE, _WE;
	MCP23S17 & addrbus;
	MCP23S08 & databus;

	uint32_t capacity; // in bytes
	uint16_t page_size;

public:

	ProgrammableROM(const uint8_t & CE, const uint8_t & OE, const uint8_t & WE,
			MCP23S17 & addrbus, MCP23S08 & databus)
	: _CE(CE), _OE(OE), _WE(WE), addrbus(addrbus), databus(databus) {}

private:
	inline static void delay_62ns() { __asm__ __volatile__ ("nop\n\t"); }  // about 62.7 ns
	inline static void delay_125ns() { __asm__ __volatile__ ("nop\n\t"); __asm__ __volatile__ ("nop\n\t"); }
	// one digitalWrite takes about 3.3 -- 3.6 us.
	// one prot xor PORTC ^= |= takes 188.6 ns (3 clocks)
	// volatile uint8_t & ioport = PORTB; output by reference becomes the same result
	// one 16 bit expander write takes 23.4 us
	// one 16 bit expander 8 bit write takes 19.4 us
	// one 8 bit expander read takes 21.07 us

	inline void chip_enable() { digitalWrite(_CE, LOW);}
	inline void chip_disable() { digitalWrite(_CE, HIGH); }

	inline void output_enable() { digitalWrite(_OE, LOW);}
	inline void output_disable() { digitalWrite(_OE, HIGH); }

	inline void write_enable() { digitalWrite(_WE, LOW);}
	inline void write_disable() { digitalWrite(_WE, HIGH); }

public:
	void begin() {
		digitalWrite(_CE, HIGH); pinMode(_CE, OUTPUT);
	    digitalWrite(_OE, HIGH); pinMode(_OE, OUTPUT);
	    digitalWrite(_WE, HIGH); pinMode(_WE, OUTPUT);

	    databus.begin();
	    databus.enable_gpio_pullup();
	    databus.set_gpio_input();

	    addrbus.begin();
	    addrbus.set_gpio16_output();
	}

	// common CE/OE controlled RAM/ROM byte read
	// high address bits will be ignored by the ROM
	uint8_t read_byte(const uint16_t & addr) {
	  uint8_t val;
	  write_disable(); // only to ensure

	  addrbus = addr; //addrbus.write_gpio16(addr);
	  databus.set_gpio_input();	 // assumes pull-up for data bus is active
	  chip_enable(); // digitalWrite(MEM_CE, LOW);
	  output_enable(); //digitalWrite(MEM_OE, LOW);
	  delay_62ns();  // t_OE = max 50ns, 62.5ns if needed
	  val = databus; //val = databus_iox.read_gpio();

	  output_disable(); //digitalWrite(MEM_OE, HIGH);
	  chip_disable(); //digitalWrite(MEM_CE, HIGH);
	  return val;
	}

	// AT28C64 single byte write,
	// seems taking about 7.89 ms on HN58C256
	bool prog_byte(const uint16_t &addr, const uint8_t val) {
		output_disable(); //digitalWrite(MEM_OE, HIGH); // to ensure
		write_disable(); //digitalWrite(MEM_WE, HIGH); // to ensure pulsee

		addrbus = addr; //.write_gpio16(addr);
		databus.set_gpio_output();
		databus = val;
		chip_enable(); // digitalWrite(MEM_CE, LOW);
		// address, OE, CE setup time = 0 ns
		write_enable(); //digitalWrite(MEM_WE, LOW);
		// write pulse width min 100 ns, address hold min 50 ns, data setup min 50 ns.
		delay_125ns();
		write_disable(); //digitalWrite(MEM_WE, HIGH);
		delay_62ns();

		// restore default i/o mode
		databus.set_gpio_input();

		// t_WPH = min 100ns in page write
		// DATA polling to observe the end of write cycle.
		bool succ = write_cycle_ended(val);

		chip_disable(); //digitalWrite(MEM_CE, HIGH);
		return succ;
	}


	bool write_cycle_ended(uint8_t lastval) {
		uint8_t readout0, readout1;

		// assumes CE is still LOW, just toggle OE to read the data
		output_disable(); //digitalWrite(MEM_OE, HIGH);
		uint16_t count = 2000;	// t_WC write cycle time MAX = 10ms
		for( ; count > 0 ; --count) {
			delayMicroseconds(5);

			output_enable(); //digitalWrite(MEM_OE, LOW);
			readout0 = databus;
			output_disable(); //digitalWrite(MEM_OE, HIGH);

			delayMicroseconds(5);

			output_enable(); //digitalWrite(MEM_OE, LOW);
			readout1 = databus;
			output_disable(); //digitalWrite(MEM_OE, HIGH);

			if ( readout0 == readout1 and readout1 == lastval ) {
				// D6 toggling stoped, D7 read the last written value
				//Serial.println(count);
	            return true;
	        }
		}

		return false;
	}

	// load byte to rom in page write procedure.
	// assumes CE, OE and WE are set high
	// toggles CE and WE
	// takes 63.7 us
	void set_byte(uint16_t addr, uint8_t value){
		addrbus = addr; //setAddress(address);
	    databus = value; //writeDataBus(value);
	    // t_WC write cycle time (wait after byte 63)

	    // t_AS address setup = 0, t_AH address hold = min 50ns, t_DS data setup = min 50ns
	    // t_WP write pulse width min 100ns, t_WPH write pulse width high = min 50ns
	    // t_BLC = max 150 us
	    //delayMicroseconds(1);

	    chip_enable();
	    write_enable();

	    //delayMicroseconds(1);
	    delay_125ns();
	    //__asm__ __volatile__ ("nop\n\t"); // delay 62ns ; // address hold
	    //__asm__ __volatile__ ("nop\n\t"); // + write pulse width
	    write_disable(); //disableWrite();
	    chip_disable(); // disableChip();
	    //__asm__ __volatile__ ("nop\n\t"); //delay_62ns(); // write pulse width high
	    //delayMicroseconds(1);
	    delay_125ns();
	}

	// takes about 4268.64 us.
	uint16_t prog_page(const uint16_t addr, uint8_t data[], const uint16_t len, const uint16_t page_size = 64) {
	    bool status = false;

	    if ( len == 0 )  return 0;

	    // page size must be 2^n
	    uint16_t pagelast_addr = addr | (page_size - 1);

	    output_disable();
	    write_disable();
	    databus.set_gpio_output();

	    // Write all of the bytes in the block out to the chip.  The chip will
	    // program them all at once as long as they are written fast enough.
	    uint8_t lastval;
	    uint16_t ix;
	    for (ix = 0; ix < len ; ++ix) {
	    	if ( !(addr + ix <= pagelast_addr) ) break;
	        lastval = data[ix];

	        // CE and WE are toggled in set_byte()
	        set_byte(addr+ix, lastval);

//	        delay_62ns();
//	        write_enable();
//	        delay_125ns();
//	        write_disable();
//	        delay_125ns();
	    }

	    status = write_cycle_ended(lastval);
	    chip_disable(); //disableChip();

	    if ( !status ) {
        	return 0;
        }
	    return ix;
	}

	// Write the special six-byte code to turn off Software Data Protection.
	void disable_software_data_protect()
	{
	    output_disable();
	    write_disable();
	    databus.set_gpio_output();

	    set_byte(0xaa, 0x5555);
	    set_byte(0x55, 0x2aaa);
	    set_byte(0x80, 0x5555);
	    set_byte(0xaa, 0x5555);
	    set_byte(0x55, 0x2aaa);
	    set_byte(0x20, 0x5555);

	    databus.set_gpio_input();
	    chip_disable();
	}

};

ProgrammableROM rom28C64(MEM_CE, MEM_OE, MEM_WE, addrbus_iox, databus_iox);

// Configuration
#define SERIAL_BAUD 115200

ProgrammerStatus pgmstatus;
HexRecord record;

char buf128[128];
String line;
uint8_t rom_typeid;

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

void get_two_uint32s(String & line, uint32_t & val1, uint32_t & val2) {
	line.trim();
	if (line.length() == 0) {
		val1 = 0;
		val2 = 0;
		return;
	}
    char * ptr;
	val1 = strtoul(line.c_str(), &ptr, 16);
	line = line.substring(ptr - line.c_str());
	line.trim();
	if (line.length() == 0) {
		val2 = 0;
		return;
	}
	val2 = strtoul(line.c_str(), &ptr, 16);
}

void setup() {
	Serial.begin(SERIAL_BAUD);

	// ensure to disable all the SPI slave devices.
	pinMode(CS_23LC1024, OUTPUT);
	digitalWrite(CS_23LC1024, HIGH);
	pinMode(CS_MCP23S08, OUTPUT);
	digitalWrite(CS_MCP23S08, HIGH);
	pinMode(CS_MCP23S17, OUTPUT);
	digitalWrite(CS_MCP23S17, HIGH);

	SPI.begin();
	auxsram.begin();
	rom28C64.begin();

	while (!Serial) { }
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
	rom_typeid = ROM_HN58C256;
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
				line = line.substring(2);
				uint32_t start, stop;
				get_two_uint32s(line, start, stop) ;
				dump_auxmem(start, stop);
				Serial.println(F("Dump loaded data finished."));

			} else if (line.startsWith("!L")) {
				Serial.println();
				Serial.println(F("Start to load new data."));
				pgmstatus.clear();

			} else if (line.startsWith("!H")) {
				printHelp();

			} else if (line.startsWith("!X")) {
				Serial.println();
				Serial.println("Test Start");
				uint8_t val = rom28C64.read_byte(0x107);
				long notimes = 10000;
				unsigned long swatch = millis();
				unsigned long uswatch = micros();
				for(long i = 0 ; i < notimes; ++i) {
					rom28C64.prog_page(0x100, buf128, 64);
				}
				uswatch = micros() - uswatch;
				swatch = millis() - swatch;
				if (val != rom28C64.read_byte(0x107) )
					Serial.println("verify failed.");
				Serial.println("Stop");
				Serial.print(double(uswatch) / notimes);
				Serial.println(" us.");
				Serial.print(double(swatch) / notimes);
				Serial.println(" ms");
			} else if (line.startsWith("!R")) {
				Serial.println();
				Serial.println(F("Read from target ROM"));
				line = line.substring(2);
				uint32_t start, stop;
				get_two_uint32s(line, start, stop) ;
				dump_target(start, stop);
				Serial.println(F("Finished."));

			} else if (line.startsWith("!S")) {
				show_pgmstatus();

			} else if (line.startsWith("!P")) {
				Serial.println();
				rom28C64.disable_software_data_protect();
				Serial.println("Software data protect disabled.");

			} else if (line.startsWith("!T")) {
				Serial.println();
				Serial.println(F("Target memory type:"));
				line = line.substring(2);
				line.trim();
	            if ( line.length() > 0 ) {
	            	rom_typeid = (uint8_t) strtol(line.c_str(), NULL, 0);
	            }
	            const ROMINFO & info = get_rominfo(rom_typeid);
	            rom_typeid = info.type_id;
	            snprintf(buf128, 127, "type id %02d \"%s\" %lu K-bytes", rom_typeid, info.name, info.capacity_bytes>>10);
	            Serial.print(buf128);
	            if (info.page_size > 0) {
	            	Serial.print(" (page write supported)");
	            }
	            Serial.println();

			} else if (line.startsWith("!W")) {
				Serial.println();
				if ( line.startsWith("!WP")) {
                    Serial.println(F("Start to write to ROM with block write if supported."));
                    write_to_rom(true);
				} else {
                    Serial.println(F("Start to write to ROM with byte write."));
                    write_to_rom(false);
                }
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

void write_to_rom(const bool page_write) {
	HexRecord t;
	uint32_t rcount = 0;
	uint32_t ix = 0;
	const ROMINFO & info = get_rominfo(rom_typeid);
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
		uint32_t addrmask = info.capacity_bytes - 1;
		if ( info.page_size == 0 or page_write == false ) {
			// supports only byte write
			for(int i = 0; i < t.datalength; ++i) {
				bool succ = rom28C64.prog_byte( (t.address + i) & addrmask , t.data[i]);
				if (! succ ) {
					pgmstatus.errorCount += 1;
					err_flag = true;
	                Serial.print("Error: Write failed at 0x");
	                Serialsnprint(buf128, 127, "%04X", t.address + i);
	                Serial.println();
	            }
			}
		} else if (info.page_size > 0) {
			uint16_t writtenbytes = rom28C64.prog_page(t.address & addrmask, t.data, t.datalength, info.page_size);
			if ( writtenbytes != t.datalength) {
				err_flag = true;
				pgmstatus.errorCount += 1;
                Serial.print("Error: Page write failed at 0x");
                Serialsnprintln(buf128, 127, "%04X", t.address & addrmask);
            }
		} else {
			Serial.println("no programming method.");
			break;
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

void dump_auxmem(uint32_t start, uint32_t stop) {
	HexRecord t;
	if ( stop == 0 ) {
		stop = 0xffffffff;
	}
	uint32_t rcount = 0;
	uint32_t ix = 0;
	while ( auxsram.read(ix) != 0 and rcount < pgmstatus.recordCount) {
		char * p = (char *) & t;
		for(int i = 0; i < HexRecord::header_size(); ++i) {
			*(p + i) = auxsram.read(ix + i);
		}
		ix += HexRecord::header_size();
		if ( start <= t.address and t.address - 1 + t.datalength <= stop) {
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
            uint8_t val = rom28C64.read_byte(addr + i);
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
	Serial.println(rom_typeid);
	Serial.println();
}


