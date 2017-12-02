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

#define MONITOR_MEM_RD
#define MONITOR_MEM_WR
#define MONITOR_IO_IN
#define MONITOR_IO_OUT

const uint8_t rom_0000_ipl[0x160] PROGMEM = {
		/* 0000 */ 0x31, 0x00, 0x01, 0xc3, 0x00, 0x01, 0x00, 0x00,
		/* 0008 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0010 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0018 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0020 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0028 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0030 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0038 */ 0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0040 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0048 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0050 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0058 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0060 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0068 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0070 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0078 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0080 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0088 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0090 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0098 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00a0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00a8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00b0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00b8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00c0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00c8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00d0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00d8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00e0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00e8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00f0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 00f8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* 0100 */ 0x21, 0x42, 0x01, 0xcd, 0x39, 0x01, 0x06, 0x31,
		/* 0108 */ 0x11, 0x01, 0x00, 0x21, 0x00, 0xdc, 0x7a, 0xd3,
		/* 0110 */ 0x10, 0x7b, 0xd3, 0x12, 0x7d, 0xd3, 0x14, 0x7c,
		/* 0118 */ 0xd3, 0x15, 0x3e, 0x80, 0xd3, 0x16, 0xc5, 0x01,
		/* 0120 */ 0x80, 0x00, 0x09, 0xc1, 0x1c, 0x7b, 0xfe, 0x1a,
		/* 0128 */ 0x20, 0x03, 0x14, 0x1e, 0x00, 0x05, 0x20, 0xde,
		/* 0130 */ 0x21, 0x55, 0x01, 0xcd, 0x39, 0x01, 0xc3, 0x00,
		/* 0138 */ 0xf2, 0x7e, 0xa7, 0xc8, 0xd3, 0x02, 0x23, 0x18,
		/* 0140 */ 0xf8, 0x76, 0x0d, 0x0a, 0x49, 0x50, 0x4c, 0x20,
		/* 0148 */ 0x6c, 0x6f, 0x61, 0x64, 0x69, 0x6e, 0x67, 0x2e,
		/* 0150 */ 0x2e, 0x2e, 0x0d, 0x0a, 0x00, 0x64, 0x6f, 0x6e,
		/* 0158 */ 0x65, 0x2e, 0x0d, 0x0a, 0x00,
};

void sram_load_rom(const uint16_t addr, const uint8_t * mem, uint16_t msize) {
  for(uint16_t i = 0; i < msize; ++i) {
    uint8_t code = pgm_read_byte(mem+i);
    sram_write(addr+i,code);
  }
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

void sram_dump(uint16_t addr, uint16_t length);

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

struct SDVirtualDisk {
	static const uint16_t 	TRACKS = 77;
	static const uint16_t 	SECTORS_PER_TRACK = 26;
	static const uint8_t 	BYTES_PER_SECTOR = 128;
	static const uint16_t 	BLOCK_SIZE = 1024;

	uint8_t drive;
	uint16_t track;
	uint8_t sector;
	uint16_t dstaddr;

	String filename;
	File sdfile;
} vdisk;

struct DiskDMA {
	uint8_t result;
	uint8_t request;
} diskdma;

void diskdma_exec(void);
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
  OC1AClock_setup(3, 32);
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
		  sram_dump(0x0000, 0x0100);
		  sdfile.close();
	  } else {
		  Serial.println(F("failed."));
		  Serial.println(F("loading IPL from program flash."));
	      sram_load_rom(0, rom_0000_ipl, sizeof(rom_0000_ipl));
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
#ifdef MONITOR_MEM_RD
    Serial.print(hexstr(strbuff,addr,4));
    if ( z80_m1() == LOW ) {
    	Serial.print(F(" M  "));
    } else {
    	Serial.print(F(" R  "));
    }
    Serial.print(hexstr(strbuff,data,2));
    Serial.println();
#endif
    while ( !z80_mreq_rd() ) ;
  } else if ( !z80_mreq_wr() ) {
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
#ifdef MONITOR_MEM_WR
    Serial.print(hexstr(strbuff,addr,4));
    Serial.print(F("  W "));
    Serial.print(hexstr(strbuff,data,2));
    Serial.println();
#endif
    while ( !z80_mreq_wr() ) ;
  } else if ( !z80_iorq_wr() ) {
	  // IORQ LOW implies via 10k wait LOW
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
#ifdef MONITOR_IO_OUT
    Serial.print(hexstr(strbuff,addr,4));
    Serial.print(F("  O "));
    Serial.print(hexstr(strbuff,data,2));
    Serial.println();
#endif
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
    DDR(PORTA) = 0x00;
    PORTA = 0x00;
#ifdef MONITOR_IO_IN
    Serial.print(hexstr(strbuff,addr,4));
    Serial.print(F(" I  "));
    Serial.print(hexstr(strbuff,data,2));
    Serial.println();
#endif
    z80_wait_disable();
    while ( !z80_iorq_rd() ) ;
    z80_wait_enable();
  }
  if ( diskdma.request != DMA_MODE::DMA_NONE ) {
	  while ( z80_busack() );
	  diskdma_exec();
	  z80_busreq(HIGH);
	  //while ( !z80_busack() );
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
  case CON_STAT:
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
	  return diskdma.result;
	  break;
  case RTC_CONT:
    // timer/clock control
    break;
  default:
	  Serial.println();
	  Serial.print("in ");
	  Serial.println(hexstr(strbuff,port,4));
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
//  case 0x04:
 //   Serial1.write(data);
 //   break;

  case DISK_SELECT: // 0x0f 15
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
	  diskdma.request = data;
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

void diskdma_exec(void) {
	Serial.println(diskdma.request,HEX);
	if ( diskdma.request == DMA_READ ) {
		Serial.print(F("DMA_READ "));
		Serial.print(vdisk.track, HEX);
		Serial.print('/');
		Serial.print(vdisk.sector, HEX);
		Serial.print(F(" -> "));
		Serial.println(hexstr(strbuff,vdisk.dstaddr, 4));
		diskdma.request = DMA_NONE;
		if ( vdisk.track > vdisk.TRACKS || vdisk.sector > vdisk.SECTORS_PER_TRACK ) {
			diskdma.result = DMA_NG;
			Serial.println(F("failed!"));
			return;
		}
		//Serial.println("seek.");
		vdisk.sdfile = SD.open(vdisk.filename);
		if ( !vdisk.sdfile ) {
			Serial.println(F("vdisk SD file open failed!"));
			return;
		}
		vdisk.sdfile.seek( ((unsigned long)vdisk.track * vdisk.SECTORS_PER_TRACK + vdisk.sector) << 7 );
		uint16_t i;
		//Serial.println("load.");
		sram_bus_setup();
		for(i = 0; i < 128; ++i) {
			int dbyte = vdisk.sdfile.read();
			if ( dbyte == -1 )
				break;
			sram_write(vdisk.dstaddr+i,(uint8_t)dbyte);
			/*
			if ( ((vdisk.dstaddr+i) & 0x0f) == 0 ) {
				Serial.println();
				Serial.print(hexstr(strbuff,vdisk.dstaddr+i, 4));
				Serial.print(" : ");
			}
			Serial.print(hexstr(strbuff,dbyte,2));
			Serial.print(" ");*/
		}
		//Serial.println();
		//sram_dump(vdisk.dstaddr, 128);
		//delay(2);
		//sram_dump(0, 0x160);
		//delay(2);
		sram_bus_release();
		vdisk.sdfile.close();
		if ( i != 128 ) {
			Serial.println(F("vdisk SD file read error!"));
			diskdma.result = DMA_NG;
		} else {
			diskdma.result = DMA_OK;
		}
	} else if ( (diskdma.request & ~0x03) == DMA_WRITE ) {
		//Write the currently set track and sector. C contains a deblocking code:
		// C=0 - Write can be deferred
		// C=1 - Write must be immediate
		// C=2 - Write can be deferred, no pre-read is necessary.
		Serial.print("DMA_WRITE, ");
		switch (diskdma.request & 0x03) {
		case 0:
			Serial.println("can be deferred.");
			break;
		case 1:
			Serial.println("must be immediate.");
			break;
		case 2:
			Serial.println("can be deferred, no pre-read is necessary.");
			break;
		default:
			Serial.println("error.");
			break;
		}
		diskdma.request = DMA_NONE;
		diskdma.result = DMA_NG;
	} else if ( diskdma.request == DMA_HOME ) {
		Serial.println("DMA_HOME");
		diskdma.request = DMA_NONE;
		/*
		vdisk.sdfile = SD.open(vdisk.filename);
		if ( !vdisk.sdfile ) {
			diskdma.result = DMA_NG;
			Serial.println(F("vdisk SD file open failed!"));
			return;
		}
		vdisk.sdfile.seek( 0 );
		vdisk.sdfile.close();
		*/
		diskdma.result = DMA_OK;
		return;
	} else if ( diskdma.request == DMA_BOOT ) {
		Serial.println("DMA_BOOT");
		diskdma.request = DMA_NONE;
		diskdma.result = DMA_NG;
	} else {
		Serial.println("undefined DMA function");
		diskdma.result = DMA_NG;
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
				}
				totalbytecount += bytecount;
				sta = STARTCODE;
			}
		}
	} while ( file.available() > 0 );

	return sta;
}

void sram_dump(uint16_t addr, uint16_t length) {
	for(uint16_t i = 0; i < length; ++i) {
		if ( (i& 0x0f) == 0x00 ) {
		  Serial.print(hexstr(strbuff, addr+i, 4));
		  Serial.print(": ");
		}
		sram_select();
		Serial.print(hexstr(strbuff, sram_read(addr+i), 2));
		sram_deselect();
		Serial.print(" ");
		if ( (i& 0x0f) == 0x0f ) {
		  Serial.println();
		}
	}
	Serial.println();
}
