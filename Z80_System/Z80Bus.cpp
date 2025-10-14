#include <SPI.h>
#include "Z80Bus.h"
#include "DFR7segarray.h"

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
		clock_set(4, 2500); //
		clock_mode = 1;
	  break;
	case 3 : // 1 kHz
		clock_set(2, 1000);
		clock_mode = 3;
	  break;
	case 4 : // 10 kHz
		clock_set(2, 200);
		clock_mode = 4;
	  break;
	case 5 :	// 20kHz
		clock_set(1, 400);
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
		switch ( uint8_t(port & 0xff) ) {
		case CONSTA:  //CONSTA
			data = uint8_t(Serial.available()); //(Serial.available() ? 0xff : 0x00);
			break;
		case CONIO:  // CONDAT/CON_IN
			data = Serial.read();
			break;
		case FDCST:       //fdc-port: status
			data = 1; //fdc.status();
			// not populated
			break;
		case DMAEXEC: // exec_dma
			dma.transfer_mode = dma.READ_RAM;
			data = 1;
			BUSREQ(LOW);
			while (BUSACK() == HIGH) ;
			break;
		case DMARES: // dma_rs
			data = dma.result;
			break;
		default:
			data = 0;
		}
		data_bus_set(data);

	} else if (WR() == LOW) {
		io_mode = OUT;
		data_bus_mode_input();
		data = data_bus_get();

		switch ( uint8_t(port & 0xff) ) {
		case CONIO:  // CONDAT/CON_IN
		case CON_OUT:  // CON_OUT
			Serial.print((char) data);
			break;
		case FDCDRIVE:  //10, fdc-port: # of drive
			fdc.sel_drive(data);
			break;
		case FDCTRACK:       //11, fdc-port: # of track
			fdc.sel_track(data);
			break;
		case FDCSECTOR:       //fdc-port: # of sector
			fdc.sel_sector(data);
			break;
		case FDCOP:       //fdc-port: command
			fdc.set_opcode(data);
			dma.transfer_mode = dma.WRITE_RAM;
			BUSREQ(LOW);
			while (BUSACK() == HIGH) ;
			break;
		case DMAL: // dma adr_L
			dma.set_address_low(data);
			break;
		case DMAH: // dma adr_H
			dma.set_address_high(data);
			break;
		case DMAEXEC: // exec_dma
			dma.transfer_mode = dma.WRITE_RAM;
			BUSREQ(LOW);
			while (BUSACK() == HIGH) ;
			break;
		case CLKMODE: // set/change clock mode
			clock_mode_select(data);
			break;
		case LED7SEG:
			WAIT(LOW);
			digitalWrite(21, LOW);
			shiftOut(19, 20, MSBFIRST, ascii7seg(data));
			digitalWrite(21, HIGH);
			WAIT(HIGH);
			break;
		}
	} else
		return 0;

	while (IORQ() == LOW) {}
	if (BUSACK() == LOW) {
		BUSREQ(HIGH);
	}
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
