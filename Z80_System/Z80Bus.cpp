#include "Z80Bus.h"

#include <SPI.h>

void Z80Bus::clock_set(uint8_t presc, uint16_t top) {
	const uint8_t WGM_CTC_OCR1A = B0100;
	const uint8_t COM_TOGGLE = B01, COM_DISCONNECT = B00;
	presc = (presc > 5 ? 5 : presc);
	/*
	 * prescaler factors
	 *
	 * 0 -- no clk source
	 * 1 -- clk/1
	 * 2 -- clk/8
	 * 3 -- clk/64
	 * 4 -- clk/256
	 * 5 -- clk/1024
	 *
	 */
	cli();
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1C = 0;
	TCNT1 = 0;
	OCR1A = top - 1;
	TCCR1A |= (COM_TOGGLE << COM1C0) | ((WGM_CTC_OCR1A & B0011) << WGM10);
	TCCR1B |= (((WGM_CTC_OCR1A >> 2) & B11) << WGM12) | (presc << CS10);
	sei();
}

void Z80Bus::clock_start() {
	const uint8_t WGM_CTC_OCR1A = B0100;
	const uint8_t COM_TOGGLE = B01;

	cli();
	TCCR1A |= (COM_TOGGLE << COM1C0) | ((WGM_CTC_OCR1A & B0011) << WGM10);
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

void Z80Bus::clock_mode_select(const uint8_t mode) {
// stop clock
	TCCR1B &= 0xf8; // once clear bits from CS12 to CS10 to stop (prescaler = No clk source)
	switch(mode) {
	case 0 : // 4 Hz
		clock_set(5, 2000);
		clock_mode = 0;
	  break;
	case 1 : // 10 Hz
		clock_set(4, 3125); //
		clock_mode = 1;
	  break;
	case 3 : // 1 kHz
		clock_set(2, 1000);
		clock_mode = 3;
	  break;
	case 4 : // 8 kHz
		clock_set(2, 250);
		clock_mode = 4;
	  break;
	case 5 :	// 10kHz
		clock_set(1, 800);
		clock_mode = 5;
	  break;
	case 2 :
	default: // 100Hz
		clock_set(3, 1250);
		clock_mode = 2;
	  break;
	}
}

//uint8_t Z80Bus::io_rw(const uint8_t &port, const uint8_t &val, const uint8_t &inout) {

uint32_t Z80Bus::io_rw() {
	uint16_t port;
	uint8_t data;
	uint8_t dma_buff[256];

	enum IO_MODE {
		IN = 0, OUT = 1,
	};
	// from Z80 side
	uint8_t io_mode = IN;

	if (IORQ() == HIGH)
		return 0;
	port = address_bus16_get();
	//Serial.print("port: ");
	//Serial.print(port, HEX);
	if (RD() == LOW) {
		io_mode = IN;
		data_bus_mode_output();
	} else if (WR() == LOW) {
		io_mode = OUT;
		data_bus_mode_input();
	} else
		return 0;

	switch ( uint8_t(port & 0xff) ) {
	case CONSTA:  //CONSTA
		if (io_mode == IN ) {
			data = uint8_t(Serial.available()); //(Serial.available() ? 0xff : 0x00);
			data_bus_set(data);
		}
		break;
	case CONIO:  // CONDAT/CON_IN
		if (io_mode == IN ) {
			data = Serial.read();
			data_bus_set(data);
		} // else if io_mode = OUT then join together in the next case
	case CON_OUT:  // CON_OUT
		if (io_mode == OUT ) {
			data = data_bus_get();
			Serial.print((char) data);
		}
		break;
	case FDCDRIVE:  //10, fdc-port: # of drive
	case FDCTRACK:       //11, fdc-port: # of track
	case FDCSECTOR:       //fdc-port: # of sector
	case FDCOP:       //fdc-port: command
	case FDCST:       //fdc-port: status
		// not populated
		break;
	case 16: // track_sel_h
		break;
	case 17: // track_sel_l
		break;
	case 18: // sector_sel
		break;
	case 20: // dma adr_L
		if (io_mode == OUT) {
			data = data_bus_get();
			dma_address = (dma_address & 0xff00) | data;
		}
		break;
	case 21: // dma adr_H
		if (io_mode == OUT) {
			data = data_bus_get();
			dma_address = (dma_address & 0x00ff) | (data << 8);
		}
		break;
	case 22: // exec_dma
		DMA_mode();
		if (io_mode == IN) {
			dma_transfer_mode = READ_RAM;
			ram_enable();
			DMA_exec(dma_buff);

			data_bus_set(1);
		} else if (io_mode == OUT) {
			dma_transfer_mode = WRITE_RAM;
			ram_enable();
			DMA_exec(dma_buff);
		}
		MMC_mode();
		break;
	case 23: // dma_rs
		if (io_mode == IN) {
			data = dma_result;
			data_bus_set(data);
		}
		break;
	case CLKMODE: // set/change clock mode
		if ( io_mode == OUT ) {
			data = data_bus_get();
			clock_mode_select(data);
		}
		break;
	case LEDARRAY:
		if (io_mode == OUT) {
			data = data_bus_get();
			WAIT(LOW);
			data = ascii7seg(data);
			digitalWrite(53, LOW);
			SPI.begin();
			SPI.transfer(data);
			SPI.end();
			digitalWrite(53, HIGH);
			WAIT(HIGH);
		}
		break;
	default:
		data = 0;
	}
	while (IORQ() == LOW) {}
	return (uint32_t(port) << 16) | data;
}

uint32_t Z80Bus::mem_rw() {
	uint16_t addr;
	uint8_t data;
	enum RW_MODE {
		READ = 0, WRITE,
	};
	//Z80's mode
	uint8_t rw_mode = READ;
	uint8_t page = 0;

	if (MREQ() == HIGH or ram_is_enabled())
		return 0;
	addr = address_bus16_get();
	page = (addr >> 12) & 0x0f;
	if ( pages[page] == NULL ) {
		// sram
		ram_enable();
		data_bus_mode_input();
		clock_wait_rising_edge();
		data = data_bus_get();
	} else {
		// rom area
		if (RD() == LOW) {
			rw_mode = READ;
			data_bus_mode_output();
		} else if (WR() == LOW) {
			rw_mode = WRITE;
			data_bus_mode_input(); // for observation
		}
		if (rw_mode == READ) {
			data = pgm_read_byte_near(pages[page] + (addr & 0x0fff));
			data_bus_set(data);
			while (RD() == LOW)
				;
		} else if (rw_mode == WRITE) {
			data_bus_mode_input();
			while (WR() == LOW)
				;
			data = data_bus_get();
			Serial.println("write rom area error.");
		}
	}
	while (MREQ() == LOW)
		;
	ram_disable();
	return (uint32_t(addr) << 16) | data;
}


uint16_t Z80Bus::ram_check(const uint32_t & start_addr = 0x0000, const uint32_t & end_addr = 0x10000) {
	uint8_t buf;
	uint16_t errcount = 0;
	randomSeed(analogRead(0));
	for(uint32_t addr = start_addr; addr < end_addr; ++addr) {
		errcount = 0;
		uint8_t t = random(256);
		buf = ram_read(addr);
		ram_write(addr, buf^t);
		if ( ram_read(addr) != (buf^t) ) {
			// error
			Serial.println(addr, HEX);
			errcount++;
		}
		ram_write(addr, buf);
	}
	if ( errcount > 0 ) {
		Serial.print(" Errors = ");
		Serial.println(errcount, DEC);
	}
	return errcount;
}

uint8_t Z80Bus::ascii7seg(char ch) {
  const static uint8_t numeric7[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff };
  const static uint8_t alpha7[] = { 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xc2, 0x89, 0x79, 0xe1,
    0x09 /* 0x89*/, 0xc7, 0xbb /* 0xaa*/, 0xab /* 0xc8*/, 0xa3, 0x8c, 0x98, 0xaf /* 0xce */, 0x92 /*0x9b*/,
    0x87, 0xc1, 0xc1 /* 0xe3*/, 0x86 /* 0xd5 */, 0xb6, 0x91, 0x24 /* 0xb8*/,
  };  //a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z
  if ('0' <= ch and ch <= '9') {
    return numeric7[ch - '0'];
  }
  switch(ch){
    case '.':
    case ',':
      return 0x7f;
      break;
    case ' ':
      return 0xff;
      break;
    case 'i':
      return 0x7b;
      break;
    case 'o':
      return 0xa3;
      break;
    case 'u':
      return 0xe3;
      break;
  }
  ch = toupper(ch);
  if ('A' <= ch and ch <= 'Z') {
    return alpha7[ch - 'A'];
  } 
  return 0xff;
}

//PROGMEM_ROM boot_blk(0x0000, 512, mon02);
