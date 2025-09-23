#include "Z80Bus.h"
#include "progmem_rom.h"

void Z80Bus::clock_start(uint8_t presc, uint16_t top) {
	const uint8_t WGM_CTC_OCR1A = B0100;
	const uint8_t COM_TOGGLE = B01;
	presc = (presc > 5 ? 5 : presc);
	cli();
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1C = 0;
	TCNT1 = 0;
	OCR1A = top - 1;
	TCCR1A |= (COM_TOGGLE << COM1C0) | ((WGM_CTC_OCR1A & B0011) << WGM10);
	TCCR1B |= (((WGM_CTC_OCR1A >> 2) & B11) << WGM12) | (presc << CS10);
	sei();

	pinMode(CLK_OUT, OUTPUT);
}

void Z80Bus::clock_stop() {
	const uint8_t WGM_CTC_OCR1A = B0100;
	const uint8_t COM_DISCONNECT = B00;

	cli();
	TCCR1A |= (COM_DISCONNECT << COM1C0) | ((WGM_CTC_OCR1A & B0011) << WGM10);
	sei();

	pinMode(CLK_OUT, INPUT_PULLUP);
}

uint8_t Z80Bus::io_rw(const uint8_t &port, const uint8_t &val, const uint8_t &inout) {
	switch (port) {
	case 0:  //CON_STS
		if (inout == INPUT) {
			return (Serial.available() ? 0xff : 0x00);
		}
		break;
	case 1:  // CON_IN
		if (inout == INPUT) {
			return Serial.read();
		}
		break;
	case 2:  // CON_OUT
		if (inout == OUTPUT)
			Serial.print((char) val);
		return 0;
		break;
	case 16: // track_sel_h
		break;
	case 17: // track_sel_l
		break;
	case 18: // sector_sel
		break;
	case 20: // dma adr_L
		if (inout == OUTPUT) {
			dma_address = (dma_address & 0xff00) | val;
		}
		break;
	case 21: // dma adr_H
		if (inout == OUTPUT) {
			dma_address = (dma_address & 0x00ff) | (val << 8);
		}
		break;
	case 22: // exec_dma
		if (inout == INPUT) {
			dma_mode = READ_RAM;
			return 1; // the number of blocks
		} else if (inout == OUTPUT) {
			dma_mode = WRITE_RAM;
		}
		break;
	default: // dma_rs
		if (inout == INPUT) {
			return dma_result;
		}
		break;
	}
	return 0;
}

uint32_t Z80Bus::mem_rw() {
	uint16_t addr;
	uint8_t data;
	enum RW_MODE { READ = 0, WRITE, };  //Z80's mode
	uint8_t rw_mode = READ;
	uint8_t page = 0;

	if ( MREQ() == HIGH or ram_is_enabled() )
		return;
  addr = address_bus16_get();
  //Serial.print("addr: ");
  //Serial.print(addr, HEX);
	if ( RD() == LOW ) {
		rw_mode = READ;
		data_bus_mode_output();
    } else if ( WR() == LOW ) {
    	rw_mode = WRITE;
    	data_bus_mode_input(); // for observation
    } else
    	return;

	page = (addr >> 10) & 0x3f;
	switch(page) {
	case 0x00:
		if (rw_mode == READ) {
		      data = pgm_read_byte_near(mon02 + (addr & 0x3ff));
		      data_bus_set(data);
		      while (RD() == LOW);
          //Serial.print(" put data ");
          //Serial.println(data, HEX);
		} else if (rw_mode == WRITE ) {
			data_bus_mode_input();
			while (WR() == LOW);
			data = data_bus_get();
      Serial.println("write rom area error.");
		}
		break;
	default:
		ram_enable();
		data_bus_mode_input();
		data = data_bus_get();
    //Serial.print("addr ");
    //Serial.print(addr, HEX);
    //Serial.print(" data ");
    //Serial.println(data, HEX);
	}
	while( MREQ() == LOW );
	ram_disable();
	return (uint32_t(addr) << 16) | data;
}


uint16_t Z80Bus::ram_check(const uint32_t & start_addr = 0x0000, const uint32_t & end_addr = 0x10000) {
  uint8_t buf[256];
  uint16_t errcount = 0;
  uint16_t errtotal = 0;
  randomSeed(analogRead(0));
  uint16_t skip = 0x100;
  for(uint32_t addr = start_addr; addr < end_addr; addr += skip) {
    errcount = 0;
    uint8_t t = random(256);
    for(uint32_t ix = 0; ix < 0x100; ++ix) {
      buf[ix] = ram_read(addr + ix);
      buf[ix] ^= t;
      ram_write(addr+ix, buf[ix]);
    }
    for(uint32_t ix = 0; ix < 0x100; ++ix) {
      if (buf[ix] != ram_read(addr+ix)) {
        Serial.println(addr+ix, HEX);
        errcount++;
      }
    }
    if ( errcount > 0 ) {
      Serial.print(addr, HEX);
      Serial.print(" -- ");
      Serial.print(addr + 0x100, HEX);
      Serial.print(" Errors = ");
      Serial.println(errcount, DEC);
    }
    errtotal += errcount;
  }
  return errtotal;
}


//PROGMEM_ROM boot_blk(0x0000, 512, mon02);
