/* by sin, Aug, 2025 */

#include "CPU_Z80.h"
#include "SRAM1Mbit.h"

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

void start_OC1C_clock(uint8_t presc, uint16_t top) {
  const uint8_t WGM_CTC_OCR1A = B0100;
  const uint8_t COM_TOGGLE = B01;

  cli();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (COM_TOGGLE << COM1C0) | ((WGM_CTC_OCR1A & B0011) << WGM10);
  TCCR1B |= (((WGM_CTC_OCR1A>>2) & B11)<< WGM12)  | (presc << CS10);

  sei();
}

void stop_OC1C_clock(uint8_t presc, uint16_t top) {
  const uint8_t WGM_CTC_OCR1A = B0100;
  const uint8_t COM_DISCONNECT = B00;

  cli();
  
  TCCR1A |= (COM_DISCONNECT << COM1C0) | ((WGM_CTC_OCR1A & B0011) << WGM10);

  sei();
}

enum Z80_Mega_pin {
  CLK     = 13, // input on Z80
  _INT    = 8,  // in
  _NMI    = 9,  // in
  _HALT   = 10, // output on Z80
  _MREQ   = 11, // out
  _IORQ  = 12, // out 
  _RD     = 40, // out
  _WR     = 41, // out
  _BUSACK = 2,  // out
  _WAIT   = 3,  // in
  _BUSREQ = 4,  // in 
  _RESET  = 5,  // in
  _M1     = 6,  // out
  _RFSH   = 7,  // out
};

const uint8 Z80_ADDR_BUS_WIDTH = 16;
const uint8 MEGA_ADDR_BUS[Z80_ADDR_BUS_WIDTH+1] = {
  // PORTA (A0 -- A7)
  22, 23, 24, 25, 26, 27, 28, 29, 
  // PORTC (A8 -- A15)
  37, 36, 35, 34, 33, 32, 31, 30,
  //
  15,
};
const uint8 DATA_BUS_WIDTH = 8;
const uint8 MEGA_DATA_BUS[DATA_BUS_WIDTH] = {
  // PORTK
  62, 63, 64, 65, 66, 67, 68, 69,
};

const uint8 SRAM_WE = _WR; // PG0/WR. (_W)
const uint8 SRAM_OE = _RD; // PG1/RD. (_G)
const uint8 SRAM_E1 = _MREQ; // PG2/ALE (_E1)
const uint8 SRAM_E2 = 14; // PG2/ALE (_E1)
const uint8 SRAM_ADDR_BUS_WIDTH = 17;

SRAM sram(SRAM_E2, SRAM_E2, SRAM_OE, SRAM_WE, SRAM_ADDR_BUS_WIDTH, MEGA_ADDR_BUS, DATA_BUS_WIDTH, MEGA_DATA_BUS);

CPU_Z80 z80(CLK, _INT, _NMI, _HALT, _MREQ, _IORQ, 
  _RD, _WR, _BUSACK, _WAIT, _BUSREQ, _RESET, _M1, _RFSH, 
  MEGA_ADDR_BUS, MEGA_DATA_BUS);

uint16 addr;
uint8 data;
const uint16 MEM_MAX = 0x0080;
const uint16 MEM_ADDR_MASK = MEM_MAX - 1;
uint8 mem[MEM_MAX] = {
  0x21, 0x30, 0x00, 
  0xdd, 0x21, 0x28, 0x00,
  0xdd, 0x36, 0x00, 0x65, 
  0x3e, 0x01,
  0xbe, 
  0x28, 0x09,
  0x36, 0x01,
  0xdd, 0x35, 0x00,
  0x28, 0x10,
  0x18, 0x03,
  0x23, 
  0x18, 0xf1, 
  0x2b, 0xbe,
  0x20, 0x04,
  0x36, 0x00,
  0x18, 0xf8,
  0x23, 
  0x18, 0xe6, 
  0x76,
};
uint8 pin_status = 0;
uint8 clock_pin = LOW;
 
void port_out(uint8 port, uint8 val) {
  Serial.print("out @ ");
  Serial.print(port, HEX);
  Serial.print(" data ");
  Serial.println(val, HEX);
  if (port = 0x01) {
    dump_mem();
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
}

  uint8 control_pin_status(){
    uint8 s = 0;
    s |= (digitalRead(_HALT) == HIGH ? 1 : 0) << 0;
    s |= (digitalRead(_MREQ) == HIGH ? 1 : 0) << 6;
    s |= (digitalRead(_IORQ) == HIGH ? 1 : 0) << 3;
    s |= (digitalRead(_RD) == HIGH ? 1 : 0) << 4;
    s |= (digitalRead(_WR) == HIGH ? 1 : 0) << 5;
    s |= (digitalRead(_BUSACK) == HIGH ? 1 : 0) << 7;
    s |= (digitalRead(_M1) == HIGH ? 1 : 0) << 2;
    s |= (digitalRead(_RFSH) == HIGH ? 1 : 0) << 1;
    return s;
  }
void dump_control_pins(uint8 s) {
  char buf[32];
  snprintf(buf, sizeof(buf), "M1 %1x MREQ %1x IORQ %1x RD %x WR %1x", s>>2 & 1, s>>6 & 1, s>>3 & 1, s>>4 & 1, s>>5&1);
  Serial.print(buf);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  while (! Serial) {}
  Serial.println("***********************");
  Serial.println("    system started.    ");
  sram.disable();
  z80.DATA_BUS_mode(OUTPUT);
  z80.set_DATA_BUS(0x00);
  z80.BUSREQ(LOW);
  Serial.println("BUREQ LOW.");
  delay(5000);
  z80.BUSREQ(HIGH);

  // start z80 clock
  start_OC1C_clock(5, 400);
  
  int count = 0;
  // perform z80 rest, assert pin during at least 3 clocks passed
  z80.RESET(LOW);
  Serial.println("Resetting Z80.");
  while (count < 5) {
    while (z80.CLK() == HIGH) ;
    while (z80.CLK() == LOW) ;
    ++count;
  }
  z80.RESET(HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (clock_pin == LOW and z80.CLK() == HIGH) {
    // rising edge
    clock_pin = HIGH;
    /*
    pin_status = z80.control_pin_status() | (1<<1);
    if (pin_status != 0xff) {
      dump_control_pins(pin_status);
      Serial.println();
    }
    */
    if ( (z80.HALT() == LOW) ) {
      Serial.println("Z80 halted.");
      while (z80.HALT() == LOW) ;
    }

    if ( !z80.MREQ() ) {
      if ( !z80.RD() ) {
        addr = z80.get_ADDR_BUS();
        if (! z80.M1()) {
          Serial.print("OP FETCH ADDR = ");
        } else {
          Serial.print("RD ADDR = ");
        }
        Serial.print(addr, HEX);
        z80.DATA_BUS_mode(OUTPUT);
        data = mem[addr & MEM_ADDR_MASK];
        z80.set_DATA_BUS(data);
        Serial.print("  DATA = ");
        Serial.println(data, HEX);
      } else if ( !z80.WR() ) {
        addr = z80.get_ADDR_BUS();
        Serial.print("WR ADDR = ");
        Serial.println(addr, HEX);
        z80.DATA_BUS_mode(INPUT);
        data = z80.get_DATA_BUS();
        mem[addr & MEM_ADDR_MASK]= data;
        dump_mem();
      }
    } else if ( !z80.IOREQ() ) {
      if ( !z80.RD() ) {
        addr = z80.get_ADDR_BUS();
        Serial.print("IN ADDR = ");
        Serial.print(addr & 0xff, HEX);
        z80.DATA_BUS_mode(OUTPUT);
        Serial.print("  DATA = ");
        Serial.println(z80.get_DATA_BUS(), HEX);
        // port_in
      } else if ( !z80.WR() ) {
        addr = z80.get_ADDR_BUS();
        Serial.print("OUT ADDR = ");
        Serial.print(addr & 0xff, HEX);
        z80.DATA_BUS_mode(INPUT);
        data = z80.get_DATA_BUS();
        //z80.WAIT(LOW);
        port_out(addr, data);
        //z80.WAIT(HIGH);
        Serial.print(". DATA = ");
        Serial.println(z80.get_DATA_BUS(), HEX);
      }
    }
  } else if ( clock_pin == HIGH and z80.CLK() == LOW) {
    // falling edge
    clock_pin = LOW;
  }
}
