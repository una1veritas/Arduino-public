#include "opcodeprint.h"
#include "z80_controller.h"
#include "sram.h"

#define SRAM_CS_PIN 4

void showMemory();
void showState(busmode & bus);

uint8_t mem[] = {
		0x3a, 0x0b, 0x00, 0x3d, 0x32, 0x0b, 0x00, 0xc2,
		0x03, 0x00, 0x76, 0x05,
									0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};
const uint16_t mem_size = sizeof mem;

uint8_t clkpulse = 0;
uint16_t cycle = 0;
uint8_t data = 0;
busmode bus;

void setup() {
  Serial.begin(19200);
  while(!Serial);
  Serial.println("Hi there.");
  Serial.print("mem size = ");
  Serial.println(mem_size);

  Serial.println("start z80");
  disable_z80_mreq();
  z80_bus_setup();
  z80_clock_start(400);
  digitalWrite(Z80_RESET_PIN, LOW);
  delay(1000);
  digitalWrite(Z80_RESET_PIN, HIGH);
  if ( !z80_request_bus() )
	  Serial.println("z80 BUSACK low");

  sram_bus_setup();
  sram_enable();
  for(uint16_t addr = 0; addr < mem_size; ++addr) {
	  sram_write(addr,addr & 0xff);
//	  Serial.print(mem[addr], HEX);
//	  Serial.print(" ");
  }
 // Serial.println();

  for(uint16_t addr = 0; addr < mem_size; ++addr) {
	  uint8_t data = sram_read(addr);
	  Serial.print(data >> 4 & 0x0f, HEX);
	  Serial.print(data & 0x0f, HEX);
	  Serial.print(" ");
	  if ((addr & 0xf) == 0xf)
		Serial.println();
  }
  Serial.println();
  sram_disable();
  while ( !z80_request_bus() );
}

void loop() {
  while ( clkpulse == digitalRead(Z80_CLK_PIN) ); // wait for the next edge.
  clkpulse = digitalRead(Z80_CLK_PIN);
  z80_bus_status(bus);
  if ( bus.MREQ && bus.RD ) {
      DDRF = 0xff;
      if ( bus.ADDR >= 0 && bus.ADDR < mem_size ) {
        data = mem[bus.ADDR];
        PORTF = data;
      } else {
        Serial.println("address error! ");
      }
      while ( !digitalRead(Z80_RD_PIN) );
  } else if ( bus.MREQ && bus.WR ) {
      DDRF = 0x00;
      //PORTF = 0xff; // pull-up
      if ( bus.ADDR >= 0 && bus.ADDR < mem_size ) {
        mem[bus.ADDR] = PINF;
        data = mem[bus.ADDR];
      } else {
        Serial.println("address error! ");
      }
      while ( !digitalRead(Z80_WR_PIN) );
  } else if ( bus.IORQ && bus.RD ) {
    DDRF = 0xff;
    PORTF = data;
    while ( !digitalRead(Z80_RD_PIN) );
  } else if ( bus.IORQ && bus.WR ) {
    DDRF = 0x00;
    PORTF = 0xff;
    data = PINF;
    while ( !digitalRead(Z80_WR_PIN) );
  } else if ( bus.RFSH && bus.MREQ ) {
	  while ( !digitalRead(Z80_RFSH_PIN) );
  }
  showState(bus);
  
  if ( !digitalRead(Z80_HALT_PIN) ) {
	  showMemory();
	  z80_request_bus();
  }
}

void showMemory() {
	uint16_t addr;
	for(addr = 0; addr < mem_size; ++addr) {
	  Serial.print(mem[addr]>>4&0x0f, HEX);
	  Serial.print(mem[addr]&0x0f, HEX);
	  Serial.print(" ");
	  if ((addr & 0xf) == 0xf)
		Serial.println();
	}
	Serial.println();
}

void showState(busmode & bus) {
//  Serial.print(cycle/10);
//  Serial.print(cycle%10);
/*  if ( clkpulse ) {
    Serial.print(" H ");
  } else {
    Serial.print(" L ");
  }
*/
  if ( bus.IORQ ) {
	  if ( bus.RD )
		  Serial.print(" I ");
	  else if ( bus.WR )
		  Serial.print(" O ");
  } else if ( bus.MREQ ) {
	  if ( bus.M1 ) {
		  Serial.print("MR ");
  	  } else if ( bus.RD ) {
  		  Serial.print(" R ");
  	  } else if ( bus.WR ) {
  		  Serial.print(" W ");
  	  } else if ( bus.RFSH ) {
  		  return;
  		  //Serial.print("F  ");
  	  } else {
  		  return;
  		  //Serial.print("  ");
  	  }
  } else {
	  return;
	  //Serial.print("  ");
  }
    
  Serial.print("[");
  Serial.print( bus.ADDR>>12&0x0f, HEX);
  Serial.print( bus.ADDR>>8&0x0f, HEX);
  Serial.print( bus.ADDR>>4&0x0f, HEX);
  Serial.print( bus.ADDR&0x0f, HEX);
  Serial.print("] ");
  if ( (bus.RD || bus.WR) && !bus.RFSH ) {
    Serial.print( data, HEX);
    Serial.print(" ");
  }  
  if ( bus.M1 && bus.RD ) {
    Serial.print("(op. ");
    opcode(data);
    Serial.print(")");
  }
  Serial.println();
}

