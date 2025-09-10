/* by sin, Aug, 2025 */
#include <LiquidCrystal.h>

#include "Mega100Bus.h"

//#define BUS_DEBUG

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

enum Mega_pin_assign {
  // z80
  CLK     = 13, // input on Z80
  _INT    = 3,  // in
  _NMI    = 4,  // in
  _HALT   = 5, // output on Z80
  _MREQ   = 39, // out
  _IORQ   = 6, // out 
  _RD     = 40, // out
  _WR     = 41, // out
  _BUSACK =  7,  // out
  _WAIT   =  8,  // in
  _BUSREQ =  9,  // in 
  _RESET  = 10,  // in
  _M1     = 11,  // out
  _RFSH   = 12,  // out

  // sram
  MEMEN   = 38, // E2 (positive neable)
};

Mega100Bus m100bus(
  //CLK, fixed
  _INT, _NMI, _HALT, _MREQ, _IORQ, 
  _RD, _WR, _BUSACK, _WAIT, _BUSREQ, _RESET, _M1, _RFSH,
  MEMEN);

//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(14, 15, 16, 17, 18, 19);

uint16 addr;
uint8 data;
uint8 flag;
char strbuf[32];

const uint16 MEM_MAX = 0x0080;
const uint16 MEM_ADDR_MASK = MEM_MAX - 1;
uint8 mem[MEM_MAX] = {
  /* example 2
  0x31, 0x30, 0x00, 0x21, 0x13, 0x00,
  0xcd, 0x0a, 0x00, 
  0x76, 
  0x7e, 
  0xb7, 
  0xc8, 
  0xd3, 0x02, 
  0x23, 
  0xc3, 0x0a, 0x00,
  0x0d, 0x0a, 
  0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 
  0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0d, 0x0a, 0x00,
  */
  /* example 1 */
0x21, 0x38, 0x00, 0x31, 0x7e, 0x00, 0xaf, 0x77, 0x3c, 0xfe, 0x64, 0x28, 0x08, 0xcd, 0x12, 0x00, 
0x18, 0xf5, 0xd3, 0xff, 0xc9, 0x21, 0x23, 0x00, 0x7e, 0xf6, 0x00, 0x28, 0x05, 0xd3, 0x02, 0x23, 
0x18, 0xf6, 0x76, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x6d, 0x79, 0x20, 0x66, 0x72, 0x69, 
0x65, 0x6e, 0x64, 0x73, 0x21, 0x0d, 0x0a, 0x00, 0x00,
};
uint8 pin_status = 0;
uint8 clock_pin = LOW;
 
void port_out(const uint8 & port, const uint8 & val) {
	switch(port) {
	case 2:  // putchar
		Serial.print((char) val);
		break;
	case 16:
		break;
	case 17:
		break;
	case 18: // sector
		break;
	case 20: // dma L
		break;
	case 21: // dma H
		break;
	case 22:
		break;
  case 0xff:
    dump_mem();
    break;
	default:
		Serial.print("out @ ");
		Serial.print(port, HEX);
		Serial.print(" data ");
		Serial.print(val, HEX);
		break;
	}
}

void dump_mem() {
  char buf[8];
  for(uint8 r = 0; r < MEM_MAX / 16; ++r) {
    for(uint8 c = 0; c < 16; ++c) {
      snprintf(buf, sizeof(buf), "%02X ", mem[(r<<4) + c]);
      Serial.print(buf);
    }
    Serial.println();
  }
  Serial.println();
}

void dump_control_pins(uint8 s) {
char buf[32];
snprintf(buf, sizeof(buf), "M1 %1x MREQ %1x IORQ %1x RD %x WR %1x", s>>2 & 1, s>>6 & 1, s>>3 & 1, s>>4 & 1, s>>5&1);
Serial.print(buf);
}

void setup() {
  // put your setup code here, to run once:
  lcd.begin(20, 4);
  lcd.print("Hi, there!");
  Serial.begin(38400);
  while (! Serial) {}
  Serial.println("***********************");
  Serial.println("    system starting.    ");

  // nop test
  m100bus.mem_disable();
  m100bus.clock_start(5, 8000);
  m100bus.address_bus16_mode_input();
  m100bus.data_bus_mode_input();

  Serial.println("Issue BUSREQ");
  z80bus.BUSREQ(LOW);
  for(uint8 t = 0; t < 8; ++t) {
    z80bus.clock_wait_rising_edge();
    if ( !z80bus.BUSACK() ) break;
  }
  if ( !z80bus.BUSACK() ) {
    Serial.println("Start initial DMA.");
    z80bus.mem_disable();
    for(uint16 addr = 0; addr < MEM_MAX; ++addr) {
      z80bus.mem_write(addr, mem[addr]);
      Serial.print(mem[addr], HEX);
      if ((addr & 0xf) == 0xf) {
        Serial.println();
      } else {
        Serial.print(" ");
      }
    }
    Serial.println("Write into SRAM done.\n");
    for(uint16 addr = 0; addr < MEM_MAX; ++addr) {
      uint8 val;
      val = z80bus.mem_read(addr);
      Serial.print(val, HEX);
      if ((addr & 0xf) == 0xf) {
        Serial.println();
      } else {
        Serial.print(" ");
      }
    }
    z80bus.address_bus16_mode_input();
    z80bus.data_bus_mode_input();
    z80bus.BUSREQ(HIGH);
  }
  z80bus.Z80_RESET(LOW);
  z80bus.clock_wait_rising_edge(5);
  z80bus.Z80_RESET(HIGH);

  /*
  z80.DATA_BUS_mode(OUTPUT);
  z80.set_DATA_BUS(0x00);
  // start z80 clock
  z80.clock_start(5, 1600);
  
  int count = 0;
  // perform z80 rest, assert pin during at least 3 clocks passed
  z80.do_reset();
  Serial.println("Resetting Z80.");
  */
}

void loop() {
  z80bus.clock_wait_rising_edge();
  if ( !z80bus.MREQ() && !z80bus.RD() ) {
    addr = z80bus.address_bus16_get();
    flag = z80bus.M1();
    z80bus.clock_wait_rising_edge();
    data = z80bus.data_bus_get();
    snprintf(strbuf, sizeof(strbuf), " READ %04X %01X %02X", addr, flag, data);
    Serial.println(strbuf);
  } else if ( !z80bus.MREQ() && !z80bus.WR() ) {
    addr = z80bus.address_bus16_get();
    z80bus.clock_wait_rising_edge();
    data = z80bus.data_bus_get();
    snprintf(strbuf, sizeof(strbuf), "WRITE %04X   %02X", addr, data);
    Serial.println(strbuf);
  } 
  /*
  // put your main code here, to run repeatedly:

  if (clock_pin == LOW and z80.CLK() == HIGH) {
    // rising edge
    clock_pin = HIGH;
    
    //pin_status = z80.control_pin_status() | (1<<1);
    //if (pin_status != 0xff) {
    //  dump_control_pins(pin_status);
    //  Serial.println();
    //}

    if ( (z80.HALT() == LOW) ) {
      Serial.println("Z80 halted.");
      while (z80.HALT() == LOW) ;
    }

    if ( !z80.MREQ() ) {
      if ( !z80.RD() ) {
        addr = z80.get_ADDR_BUS();
#ifdef BUS_DEBUG
        if (! z80.M1()) {
          Serial.print("OP FETCH ADDR = ");
        } else {
          Serial.print("RD ADDR = ");
        }
        Serial.print(addr, HEX);
#endif
        z80.DATA_BUS_mode(OUTPUT);
        data = mem[addr & MEM_ADDR_MASK];
        z80.set_DATA_BUS(data);
#ifdef BUS_DEBUG
        Serial.print("  DATA = ");
        Serial.println(data, HEX);
#endif
        while ( !z80.MREQ() and !z80.RD() ) ;
      } else if ( !z80.WR() ) {
        addr = z80.get_ADDR_BUS();
#ifdef BUS_DEBUG
        Serial.print("WR ADDR = ");
        Serial.println(addr, HEX);
#endif
        z80.DATA_BUS_mode(INPUT);
        data = z80.get_DATA_BUS();
        mem[addr & MEM_ADDR_MASK]= data;
        //dump_mem();
        while ( !z80.MREQ() and !z80.WR() ) ;
      }
    } else if ( !z80.IORQ() ) {
      if ( !z80.RD() ) {
        addr = z80.get_ADDR_BUS();
#ifdef BUS_DEBUG
        Serial.print("IN ADDR = ");
        Serial.print(addr & 0xff, HEX);
        z80.DATA_BUS_mode(OUTPUT);
        Serial.print("  DATA = ");
        Serial.println(z80.get_DATA_BUS(), HEX);
#endif
        // port_in
        while ( !z80.IORQ() and !z80.RD() ) ;
      } else if ( !z80.WR() ) {
        addr = z80.get_ADDR_BUS();
#ifdef BUS_DEBUG
        Serial.print("OUT ADDR = ");
        Serial.print(addr & 0xff, HEX);
#endif
        z80.DATA_BUS_mode(INPUT);
        data = z80.get_DATA_BUS();
#ifdef BUS_DEBUG
        Serial.print(". DATA = ");
        Serial.println(z80.get_DATA_BUS(), HEX);
#endif
        z80.WAIT_LO();
        port_out(addr, data);
        z80.WAIT_HI();
        while ( !z80.IORQ() and !z80.WR() ) ;
      }
    }
  } else if ( clock_pin == HIGH and z80.CLK() == LOW) {
    // falling edge
    clock_pin = LOW;
  }
  */
}
