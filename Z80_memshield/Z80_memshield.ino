#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "SPISRAM.h"
#include "I2CEEPROM.h"

#include "common.h"
#include "mem.h"
#include "Z80.h"


const uint8_t rom_0000[0x100] PROGMEM = {
		/* 0000 */
		0x21, 0x00, 0x01, 0xF9, 0xC3, 0x00, 0x10, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0020 */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xC9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t rom_1000[0x100] PROGMEM = {
		/* 1000 */
		0x21, 0x0F, 0x10, 0xCD, 0x07, 0x10, 0x76, 0x7E,
		0xA7, 0xC8, 0xD3, 0x02, 0x23, 0x18, 0xF8, 0x48,
		0x69, 0x2C, 0x20, 0x74, 0x68, 0x65, 0x72, 0x65,
		0x21, 0x0D, 0x0A, 0x00,
};

void sram_load(const uint16_t addr, const uint8_t * mem, uint16_t msize) {
  for(uint16_t i = 0; i < msize; ++i) {
    uint8_t code = pgm_read_byte(mem+i);
    sram_select();
    sram_write(addr+i,code);
    sram_deselect();
  }
  for(uint16_t i = 0; i < msize; ++i) {
    if ( (i& 0x0f) == 0x00 ) {
      Serial.print(hexstr(addr+i, 4));
      Serial.print(": ");
    }
    sram_select();
    Serial.print(hexstr(sram_read(addr+i), 2));
    sram_deselect();
    Serial.print(" ");
    if ( (i& 0x0f) == 0x0f ) {
      Serial.println();
    }
  }
  Serial.println();  
}

char * hexstr(uint32_t val, uint8_t digits) {
  static char buf[8+1];
  digits = digits >= 8 ? 8 : digits;
  for(uint8_t d = digits; d > 0; --d ) {
    buf[d-1] = (val & 0x0f);
    if ( buf[d-1] < 0x0a ) {
      buf[d-1] += '0';
    } else {
      buf[d-1] += ('A' - 10);
    }
    val >>= 4;
  }
  buf[digits] = 0;
  return buf;
}

class OC1AClock {
  private:
    uint8_t prescaler;
    uint16_t topvalue;
  public:
  
  OC1AClock(uint8_t presc, uint16_t top) : prescaler(presc), topvalue(top) {}

  void start(void) {
    const uint8_t MODE = 4;
    cli();
    
    TCCR1A = 0; TCCR1B = 0; TCCR1C = 0; 
    
    TCNT1 = 0;
    OCR1A = topvalue - 1;
  
    TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
    TCCR1B |= ((MODE >> 2 & 0x03) << 3) |  (prescaler<<CS10);
    TCCR1C |= (1 << FOC1A);
  
    sei();
  }
  
  void stop() {
    TCCR1B &= ~(0x07<<CS10);
  }
  
  void restart(void) {
    TCCR1B |= (prescaler<<CS10);
  }

  void restart(uint8_t presc, uint16_t top) {
    stop();
    topvalue = top;
    prescaler = presc;
    cli();
    TCNT1 = 0;
    OCR1A = topvalue - 1;
    sei();
    restart();
  }
};

OC1AClock clock(4, 1600);

I2CEEPROM eeprom;

SPISRAM spisram(SPISRAM_CS_PIN);

BYTE eeprom_load_ihex(I2CEEPROM & rom, File & file);

uint8_t io_read(uint16_t port);
void io_write(uint16_t port, uint8_t data);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(57600);
  while (!Serial);
  Serial1.begin(57600);
  while (!Serial1);

  Serial.println(F("This is Serial."));
  Serial1.println(F("This is Z80 via Serial1."));

  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  pinMode(SPISRAM_CS_PIN, OUTPUT);
  digitalWrite(SPISRAM_CS_PIN, HIGH);
  SPI.begin();
  Wire.begin();

  Serial.print(F("I2C EEPROM "));
  if ( eeprom.begin() ) {
	  Serial.print(F(" started."));
  } else {
	  Serial.print(F(" failed."));
	  while (1);
  }
  Serial.print(F("SPI SRAM "));
  if ( spisram.begin() ) {
	  Serial.println(F(" started."));
  } else {
	  Serial.println(F("start failed."));
	  while (1);
  }
  Serial.print(F("SD "));
  if ( SD.begin(SD_CS_PIN) ) {
	  Serial.println(F("started."));
	  Serial.print(F("open ROM.HEX "));
	  File romfile = SD.open(F("ROM.HEX"));
	  if ( romfile ) {
		  Serial.println(F("succeeded."));
		  BYTE errcode;
		  if ( (errcode = eeprom_load_ihex(eeprom, romfile)) ) {
			  Serial.print(F("ihex read error "));
			  Serial.println(errcode, HEX);
		  }
		  romfile.close();
	  } else {
		  Serial.println(F("failed."));
	  }
  } else {
	  Serial.println(F("start failed."));
	  while (1);
  }
  
  Serial.println(F("starting Z80.."));
  z80_bus_setup();
  clock.start();
  z80_reset(500);

  Serial.print(F("BUSREQ "));
  z80_busreq(LOW);
  if ( !z80_busack(1000) ) {
    Serial.println(F("succeeded."));
    sram_bus_init();
    Serial.println(F("sram_check "));
    if ( !memory_check(0, 0x10000 /* 0x80000 */) ) {
      Serial.println(F("sram_check passed."));
      sram_load(0x0000, rom_0000, sizeof(rom_0000));
      sram_load(0x1000, rom_1000, sizeof(rom_1000));
    }
    sram_bus_release();
  } else {
        Serial.println(F(" failed."));
  }

//  digitalWrite(MEGA_MEMEN_PIN, LOW);  // enable MREQ to CS
  z80_busreq(HIGH);
  z80_reset(500);

} 

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t addr = 0;
  uint8_t data = 0;

  if ( !z80_mreq_rd() ) {
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
    Serial.print(hexstr(addr,4));
    Serial.print(F(" R  "));
    Serial.print(hexstr(data,2));
    Serial.println();
    while ( !z80_mreq_rd() ) ;
  } else if ( !z80_mreq_wr() ) {
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
    Serial.print(hexstr(addr,4));
    Serial.print(F("  W "));
    Serial.print(hexstr(data,2));
    Serial.println();
    while ( !z80_mreq_wr() ) ;
  } else if ( !z80_iorq_wr() ) {
	  // IORQ LOW implies via 10k wait LOW
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
    io_write(addr, data);
    Serial.print(hexstr(addr,4));
    Serial.print(F("  O "));
    Serial.print(hexstr(data,2));
    Serial.println();
    z80_wait_disable();
    while ( !z80_iorq_wr() ) ;
    z80_wait_enable();
  } else if ( !z80_iorq_rd() ) {
	  // IORQ LOW implies via 10k wait LOW
    addr = ((uint16_t)PINC<<8) | PINL;
    data = io_read(addr);
    DDR(PORTA) = 0xff;
    PORTA = data;
    Serial.print(hexstr(addr,4));
    Serial.print(F(" I  "));
    Serial.print(hexstr(data,2));
    Serial.println();
    z80_wait_disable();
    while ( !z80_iorq_rd() ) ;
    z80_wait_enable();
    DDR(PORTA) = 0x00;
    PORTA = 0x00;
  }

}

uint32_t memory_check(uint32_t startaddr, uint32_t maxaddr) {
  uint32_t errs, errtotal = 0;
  uint32_t block = 0x2000;
  for(uint32_t addr = startaddr; addr < maxaddr; addr += block ) {
    sram_bank(addr>>16 & 0x07);
    Serial.print("[bank ");
    Serial.print(hexstr(addr>>16 & 0x07, 2));
    Serial.print("] ");
    Serial.print(hexstr(addr & 0xffff, 4));
    Serial.print(" - ");
    Serial.print(hexstr( (addr+block-1) & 0xffff, 4));
    
    if ( (errs = sram_check(addr & 0xffff, block)) > 0 ) {
      errtotal += errs;
      Serial.print(" err: ");
      Serial.println(errs);
    } else {
      Serial.println(" ok. ");      
    }
  }
  return errtotal;
}

uint8_t io_read(uint16_t port) {
  switch(port & 0xff) {
  case 0x00:
    if ( Serial1.available() )
      return 0xff;
      return 0x00;
    break;
  case 0x01:
    return (uint8_t) Serial1.read();
    break;
  case 0x04:
    return (uint8_t) Serial1.read();
    break;
  case 0x40:
    // timer/clock control
    break;
  }
  return 0;
}

void io_write(uint16_t port, uint8_t data) {
  switch(port & 0xff) {
  case 0x01:
  case 0x02:
    Serial1.write(data);
    break;
  case 0x04:
    Serial1.write(data);
    break;
  default:
	  Serial.println();
	  Serial.print("out ");
	  Serial.print(hexstr(port,4));
	  Serial.print(" ");
	  Serial.println(hexstr(data,2));
	  break;
  }
  return;
}

int sdfgets(File & file, char * buf, unsigned int lim) {
	int n = file.readByte(buf,lim);
	buf[n] = 0;
	return n;
}

BYTE eeprom_load_ihex(I2CEEPROM & rom, File & file) {
	enum {
		STARTCODE = 1, // awaiting start code ':'
		BYTECOUNT,
		ADDRESS,
		RECORDTYPE,
		DATA,
		CHECKSUM,
		ENDOFFILE = 0,
		ERRORFLAG = 0x80,
		FILEIOERROR = ERRORFLAG | 0x40,
	};
	unsigned char sta = STARTCODE;
	unsigned long baseaddress = 0;
	unsigned int address = 0;
	char buf[256];
	unsigned char bytes[256];
	unsigned int byteindex, bytecount;
	unsigned char xsum, recordtype;

	//Serial.println("ihex file read start.");
	unsigned int totalbytecount = 0;
	char * ptr;
	do {
		if (sta == STARTCODE) {
			if ( sdfgets(buf, 1) != 1 ) {
				sta = FILEIOERROR;
				break;
			} else if (buf[0] == ':') {
				xsum = 0;
				sta = BYTECOUNT;
				//printf("start code, ");
			} else if (!iscntrl(buf[0])) {
				//Serial.println("failed to find startcode.");
				sta |= ERRORFLAG;
				break;
			}
		} else if (sta == BYTECOUNT) {
			if ( sdfgets(buf, 2) != 2 ) {
				sta = FILEIOERROR;
				break;
			} else {
				bytecount = strtol(buf, &ptr, 16);
				if (*ptr != (char) 0) {
					//Serial.println("failed to read byte count.");
					Serial.println(buf);
					sta |= ERRORFLAG;
					break;
				}
				xsum += bytecount;
				sta = ADDRESS;
				//printf("byte count %d, ",bytecount);
			}
		} else if (sta == ADDRESS) {
			if ( sdfgets(buf, 4) != 4 ) {
				sta = FILEIOERROR;
				break;
			} else {
				address = strtol(buf, &ptr, 16);
				if (*ptr != (char) 0) {
					//Serial.println("failed to read address.");
					sta |= ERRORFLAG;
					break;
				}
				//Serial.print(hexstr(address, 4));
				//Serial.print(" ");
				//printf("address %04X, ",address);
				xsum += address >> 8 & 0xff;
				xsum += address & 0xff;
				sta = RECORDTYPE;
			}
		} else if (sta == RECORDTYPE) {
			if ( sdfgets(buf, 2) != 2) {
				sta = FILEIOERROR;
				break;
			} else {
				recordtype = strtol(buf, &ptr, 16);
				if (*ptr != (char) 0) {
					//Serial.println("failed to read record type.");
					sta |= ERRORFLAG;
					break;
				}
				xsum += recordtype;
				if (bytecount) {
					sta = DATA;
					byteindex = 0;
				} else {
					sta = CHECKSUM;
				}
				//printf("type %02X, ", recordtype);
			}
		} else if (sta == DATA) {
			if ( sdfgets(buf, 2) != 2 ) {
				sta = FILEIOERROR;
				break;
			} else {
				bytes[byteindex] = strtol(buf, &ptr, 16);
				if (*ptr != (char) 0) {
					//Serial.println("failed while reading data.");
					sta |= ERRORFLAG;
					break;
				}
				xsum += bytes[byteindex];
				//printf("%02X ", bytes[byteindex]);
				byteindex++;
				if (byteindex < bytecount) {
					sta = DATA;
				} else {
					sta = CHECKSUM;
				}
			}
		} else if (sta == CHECKSUM) {
			if ( sdfgets(buf, 2) != 2 ) {
				sta = FILEIOERROR;
				break;
			} else {
				xsum += strtol(buf, &ptr, 16);
				if (*ptr != (char) 0) {
					sta |= ERRORFLAG;
					break;
				}
				//printf(": %02X, ", xsum );
				if (xsum) {
					//Serial.println("Got a check sum error.");
					sta |= ERRORFLAG;
					break;
				}
				if (recordtype == 1) {
					//Serial.println("end-of-file.");
					sta = ENDOFFILE;
					break;
				} else {
					for (unsigned int i = 0; i < byteindex; i++) {
						rom.update(baseaddress+address+i, bytes[i]);
						//Serial.print(hexstr(bytes[i], 2));
						//Serial.print(" ");
					}
					//Serial.println();
					totalbytecount += bytecount;
					sta = STARTCODE;
				}
			}
		}
	} while ( file.available() > 0 );

	return sta;
}
