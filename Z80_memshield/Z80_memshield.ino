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
#include "OC1AClock.h"
#include "iodef.h"


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
    sram_write(addr+i,code);
  }
  /*
  for(uint16_t i = 0; i < msize; ++i) {
    if ( (i& 0x0f) == 0x00 ) {
      Serial.print(hexstr(strbuff, addr+i, 4));
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
  */
}

char * hexstr(char * buf, uint32_t val, uint8_t digits) {
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

int str2byte(char * str) {
	unsigned char c;
	int val = 0;
	for (int i = 0; i < 2; i++) {
		c = toupper(str[i]);
		if ( c > '9' )
			c -= 7;
		c -= '0';
		if ( c & 0xf0 )
			return -1;
		val <<= 4;
		val |= c;
	}
	return val;
}

I2CEEPROM eeprom(0);
SPISRAM spisram(SPISRAM_CS_PIN);

enum LOAD_DEST {
	LOAD_DST_SRAM,
	LOAD_DST_SPISRAM,
	LOAD_DST_I2CEEPROM,
};
uint8_t load_ihex(File & file, const uint8_t dst);

uint8_t io_read(uint16_t port);
void io_write(uint16_t port, uint8_t data);
char strbuff[9];

struct VirtualDisk {
	static const uint16_t 	TRACKS = 77;
	static const uint16_t 	SECTORS_PER_TRACK = 26;
	static const uint8_t 	SECTOR_BYTESIZE = 128;
	static const uint16_t 	BLOCK_SIZE = 1024;

	uint16_t track;
	uint8_t sector;
	uint16_t dstaddr;

	String filename;
	File sdfile;
} vdisk;

struct DMA {
	uint8_t result;
	uint8_t request;
} dma;

void dma_exec(void);
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
  } else {
	  Serial.println(F("start failed."));
	  while (1);
  }
  
  Serial.println(F("starting Z80.."));
  OC1AClock_setup(4, 200);
  z80_bus_setup();
  OC1AClock_start();
  z80_reset();

  Serial.print(F("BUSREQ "));
  z80_busreq(LOW);
  if ( !z80_busack(1000) ) {
    Serial.println(F("succeeded."));
    sram_bus_setup();
    Serial.println(F("sram_check "));
    if ( !memory_check(0, 0x10000 /* 0x80000 */) ) {
      Serial.println(F("sram_check passed."));
	  Serial.print(F("open BOOTROM.HEX "));
	  File sdfile = SD.open(F("BOOTROM.HEX"));
	  if ( sdfile ) {
		  Serial.println(F("succeeded."));
		  BYTE errcode;
		  if ( (errcode = load_ihex(sdfile, LOAD_DST_SRAM)) ) {
			  Serial.print(F("ihex read error "));
			  Serial.println(errcode, HEX);
			  while (1);
		  }
		  sdfile.close();
	  } else {
		  Serial.println(F("failed."));
	  }
    }
    sram_bus_release();
  } else {
        Serial.println(F(" failed."));
  }

  if ( SD.exists("CPMBOOT.DSK") ) {
	  vdisk.filename = "CPMBOOT.DSK";
  }

//  digitalWrite(MEGA_MEMEN_PIN, LOW);  // enable MREQ to CS
  z80_busreq(HIGH);
  z80_reset();

} 

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t addr = 0;
  uint8_t data = 0;

  if ( !z80_mreq_rd() ) {
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
    Serial.print(hexstr(strbuff,addr,4));
    if ( z80_m1() == LOW ) {
    	Serial.print(F(" M  "));
    } else {
    	Serial.print(F(" R  "));
    }
    Serial.print(hexstr(strbuff,data,2));
    Serial.println();
    while ( !z80_mreq_rd() ) ;
  } else if ( !z80_mreq_wr() ) {
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
    Serial.print(hexstr(strbuff,addr,4));
    Serial.print(F("  W "));
    Serial.print(hexstr(strbuff,data,2));
    Serial.println();
    while ( !z80_mreq_wr() ) ;
  } else if ( !z80_iorq_wr() ) {
	  // IORQ LOW implies via 10k wait LOW
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
    Serial.print(hexstr(strbuff,addr,4));
    Serial.print(F("  O "));
    Serial.print(hexstr(strbuff,data,2));
    Serial.println();
    io_write(addr, data);
    z80_wait_disable(); // force WAIT high
    while ( !z80_iorq_wr() ) ;
    z80_wait_enable();
  } else if ( !z80_iorq_rd() ) {
	  // IORQ LOW implies via 10k wait LOW
    addr = ((uint16_t)PINC<<8) | PINL;
    data = io_read(addr);
    DDR(PORTA) = 0xff;
    PORTA = data;
    Serial.print(hexstr(strbuff,addr,4));
    Serial.print(F(" I  "));
    Serial.print(hexstr(strbuff,data,2));
    Serial.println();
    z80_wait_disable();
    while ( !z80_iorq_rd() ) ;
    z80_wait_enable();
    DDR(PORTA) = 0x00;
    PORTA = 0x00;
  }
  if ( dma.request != DMA_MODE::DMA_NONE ) {
	  while ( z80_busack() );
	  dma_exec();
	  z80_busreq(HIGH);
	  while ( !z80_busack() );
	  //Serial.println("now BUSACK high.");
  }

}

uint32_t memory_check(uint32_t startaddr, uint32_t maxaddr) {
  uint32_t errs, errtotal = 0;
  uint32_t block = 0x2000;
  for(uint32_t addr = startaddr; addr < maxaddr; addr += block ) {
    Serial.print(hexstr(strbuff,addr & 0xffff, 6));
    Serial.print(" - ");
    Serial.print(hexstr(strbuff, (addr+block-1) & 0xffff, 6));
    sram_bank(addr>>16 & 0x07);
    errs = sram_check(addr & 0xffff, block);
    if ( errs > 0 ) {
      errtotal += errs;
      Serial.print(" errors ");
      Serial.println(errs);
    } else {
      Serial.println(" ok. ");
    }
  }
  return errtotal;
}

uint8_t io_read(uint16_t port) {
  switch(port & 0xff) {
  case CON_STATUS:
    if ( Serial1.available() )
      return 0xff;
      return 0x00;
    break;
  case CON_IN:
    return (uint8_t) Serial1.read();
    break;
  case CON_READ_NOWAIT:
    return (uint8_t) Serial1.read();
    break;

  case DISK_DMA_RES: 	// 23
	  return dma.result;
	  break;
  case RTC_CONT:
    // timer/clock control
    break;
  }
  return 0;
}

void io_write(uint16_t port, uint8_t data) {
  switch(port & 0xff) {
  case CON_IN:
  case CON_OUT:
    Serial1.write(data);
    break;
  case 0x04:
    Serial1.write(data);
    break;

  case DISK_TRACK_L: 	// 16
	  vdisk.track = data; // here TRACK_L clears high 8 bits
	  break;
  case DISK_TRACK_H: 	// 17
	  vdisk.track = (((uint16_t)data)<<8) | (vdisk.track & 0xff);
	  break;
  case DISK_SECTOR: 	// 18
	  vdisk.sector = data;
	  break;
  case DISK_DMA_ADDRL: 	// 20
	  vdisk.dstaddr &= 0xff00;
	  vdisk.dstaddr |= data;
	  break;
  case DISK_DMA_ADDRH: // 21
	  vdisk.dstaddr &= 0x00ff;
	  vdisk.dstaddr |= (((uint16_t)data)<<8);
	  break;
  case DISK_DMA_EXEC: 	// 22
	  dma.request = data;
	  z80_busreq(LOW);
	  // dma_exec will be executed in busack-state after finished the OUT inst.
	  break;
  default:
	  Serial.println();
	  Serial.print("out ");
	  Serial.print(hexstr(strbuff,port,4));
	  Serial.print(" ");
	  Serial.println(hexstr(strbuff,data,2));
	  break;
  }
  return;
}

void dma_exec(void) {
	if ( dma.request == DMA_READ ) {
		Serial.print("track ");
		Serial.print(vdisk.track, HEX);
		Serial.print(", sector ");
		Serial.print(vdisk.sector, HEX);
		Serial.print(", DMA to address ");
		Serial.println(hexstr(strbuff,vdisk.dstaddr, 4));
		if ( vdisk.track > vdisk.TRACKS || vdisk.sector > vdisk.SECTORS_PER_TRACK ) {
			dma.result = DMA_NG;
			dma.request = DMA_OK;
			Serial.println("failed!");
			return;
		}
		//Serial.println("seek.");
		vdisk.sdfile = SD.open(vdisk.filename);
		if ( !vdisk.sdfile ) {
			Serial.println("vdisk file open failed!");
			return;
		}
		vdisk.sdfile.seek(vdisk.track * vdisk.SECTORS_PER_TRACK + vdisk.sector );
		uint16_t i;
		//Serial.println("load.");
		sram_bus_setup();
		for(i = 0; i < 128; ++i) {
			int dbyte = vdisk.sdfile.read();
			if ( dbyte == -1 )
				break;
			sram_write(vdisk.dstaddr+i,(uint8_t)dbyte);
		}
		vdisk.sdfile.close();
		sram_bus_release();
		if ( i != 128 ) {
			Serial.println("vdisk read error!");
			dma.result = DMA_NG;
			dma.request = DMA_OK;
			return;
		} else {
			dma.result = DMA_OK;
			dma.request = DMA_OK;
			return;
		}
	} else if ( dma.request == DMA_WRITE ) {
		Serial.println("DMA_WRITE");
	} else {
		Serial.println("undefined DMA function");
	}
}


int sdfgets(File & file, char * buf, unsigned int lim) {
	int n = file.read(buf,lim);
	buf[n] = 0;
	return n;
}

uint8_t load_ihex(File & file, const uint8_t dst) {
	enum {
		STARTCODE = 1, // awaiting start code ':'
		BYTECOUNT,
		ADDRESS,
		RECORDTYPE,
		HEADER,
		DATA,
		CHECKSUM,
		ENDOFFILE = 0,
		ERRORFLAG = 0x80,
		SYNTAXERROR = ERRORFLAG | 0x20,
		FILEIOERROR = ERRORFLAG | 0x40,
	};
	unsigned char sta = STARTCODE;
	unsigned long baseaddress = 0;
	unsigned int address = 0;
	char buf[8];
	unsigned char bytes[256];
	unsigned int byteindex, bytecount;
	unsigned char xsum, recordtype;

	int t;
	//Serial.println("ihex file read start.");
	unsigned int totalbytecount = 0;
	do {
		if (sta == STARTCODE) {
			if ( sdfgets(file, buf, 1) != 1 ) {
				sta = FILEIOERROR;
				break;
			}
			if (buf[0] == ':') {
				xsum = 0;
				sta = HEADER;
				//printf("start code, ");
			} else if (!iscntrl(buf[0])) {
				//Serial.println("failed to find startcode.");
				sta |= ERRORFLAG;
				break;
			}
		} else if (sta == HEADER) {
			if ( sdfgets(file, buf, 8) != 8 ) {
				sta = FILEIOERROR;
				break;
			}
			t = str2byte(buf);
			if ( t < 0 ) {
				sta = BYTECOUNT | SYNTAXERROR;
				break;
			}
			bytecount = (uint8_t) t;
			xsum += bytecount;
			t = str2byte(buf+2);
			if ( t < 0 ) {
				sta = ADDRESS | SYNTAXERROR;
				break;
			}
			address = t & 0xff;
			xsum += (uint8_t) t;
			address <<= 8;
			t = str2byte(buf+4);
			if ( t < 0 ) {
				sta = ADDRESS | SYNTAXERROR;
				break;
			}
			address |= (t & 0xff);
			xsum += (uint8_t) t;
			t = str2byte(buf+6);
			if ( t < 0 ) {
				sta = RECORDTYPE | SYNTAXERROR;
				break;
			}
			recordtype = t & 0xff;
			xsum += (uint8_t) t;
			if ( bytecount ) {
				sta = DATA;
				byteindex = 0;
			} else {
				sta = CHECKSUM;
			}
			Serial.print(hexstr(strbuff,address,4));
			Serial.print(": ");
		} else if (sta == DATA) {
			if ( sdfgets(file, buf, 2) != 2 ) {
				sta = FILEIOERROR;
				break;
			}
			t = str2byte(buf);
			if ( t < -1 ) {
				sta = DATA | SYNTAXERROR;
				break;
			}
			bytes[byteindex] = (uint8_t) t;
			xsum += bytes[byteindex];
			//printf("%02X ", bytes[byteindex]);
			byteindex++;
			if (byteindex < bytecount) {
				sta = DATA;
			} else {
				sta = CHECKSUM;
			}
		} else if (sta == CHECKSUM) {
			if ( sdfgets(file, buf, 2) != 2 ) {
				sta = FILEIOERROR;
				break;
			}
			t = str2byte(buf);
			if ( t < 0 ) {
				sta = DATA | SYNTAXERROR;
				break;
			}
			xsum += (uint8_t) t;
			if ( xsum ) {
				//Serial.println("Got a check sum error.");
				sta = CHECKSUM | ERRORFLAG;
				break;
			}
			if (recordtype == 1) {
				//Serial.println("end-of-file.");
				sta = ENDOFFILE;
				break;
			} else {
				for (unsigned int i = 0; i < byteindex; i++) {
					if ( dst == LOAD_DST_I2CEEPROM ) {
						eeprom.update(baseaddress+address+i, bytes[i]);
					} else if ( dst == LOAD_DST_SRAM ) {
						sram_write(baseaddress+address+i, bytes[i]);
					}
					Serial.print(hexstr(strbuff,bytes[i], 2));
					Serial.print(" ");
				}
				Serial.println();
				totalbytecount += bytecount;
				sta = STARTCODE;
			}
		}
	} while ( file.available() > 0 );

	return sta;
}
