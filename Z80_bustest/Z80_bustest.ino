
#include "opcodeprint.h"
#include "z80_pindef.h"

#define ALLOW_Z80_MREQ_PIN 4
#define SRAM_CS_PIN 5

void z80_clock_start(uint16_t top) {
  const uint8_t MODE = 4;
  const uint8_t PRESCALER = 5;

  TIMSK1 = 0;
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (PRESCALER << CS10);
  TCCR1C |= (1 << FOC1A);

  DDRB |= (1<<PB5);
}

void z80_bus_setup() {
  DDRF = 0xff;
  PORTF = 0x00;
  DDRA = 0x00;
  DDRC = 0x00;

  pinMode(Z80_MREQ_PIN, INPUT);
  pinMode(Z80_IORQ_PIN, INPUT);
  pinMode(Z80_RD_PIN, INPUT);
  pinMode(Z80_WR_PIN, INPUT);

  pinMode(Z80_M1_PIN, INPUT);
  pinMode(Z80_RFSH_PIN, INPUT);
//  pinMode(Z80_BUSACK_PIN, INPUT);
  pinMode(Z80_HALT_PIN, INPUT);
  digitalWrite(Z80_HALT_PIN, HIGH);

  pinMode(Z80_RESET_PIN, OUTPUT);
  digitalWrite(Z80_RESET_PIN, HIGH);
  pinMode(Z80_BUSREQ_PIN, OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, HIGH);
  pinMode(Z80_WAIT_PIN, OUTPUT);
  digitalWrite(Z80_WAIT_PIN, HIGH);
//  pinMode(Z80_NMI_PIN, OUTPUT);
//  digitalWrite(Z80_NMI_PIN, HIGH);
//  pinMode(Z80_INT_PIN, OUTPUT);
//  digitalWrite(Z80_INT_PIN, HIGH);
}

void z80_bus_request() {
    digitalWrite(Z80_BUSREQ_PIN, LOW);
    while ( !digitalRead(Z80_HALT_PIN) );
    digitalWrite(Z80_BUSREQ_PIN, HIGH);
}

void showMemory();
void showState();

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
uint16_t addr = 0;
uint8_t data = 0;

struct busmode {
	bool M1, RFSH, IORQ, MREQ, WR, RD;
} bus;

void setup() {
  Serial.begin(19200);
  while(!Serial);
  Serial.println("Hi there.");
  Serial.print("mem size = ");
  Serial.println(mem_size);

  pinMode(ALLOW_Z80_MREQ_PIN, OUTPUT);
  digitalWrite(ALLOW_Z80_MREQ_PIN, HIGH); // mreq disabled.
  pinMode(SRAM_CS_PIN, OUTPUT);
  digitalWrite(SRAM_CS_PIN,HIGH);
  
  z80_bus_setup();
  z80_clock_start(400);
  digitalWrite(Z80_RESET_PIN, LOW);
  delay(2000);
  digitalWrite(Z80_RESET_PIN, HIGH);
}

void loop() {
  while ( clkpulse == digitalRead(Z80_CLK_PIN) ); // wait for the next edge.
  clkpulse = digitalRead(Z80_CLK_PIN);
  /*
  if ( clkpulse ) {
    if ( !digitalRead(Z80_M1_PIN) && !bus.M1 ) {
      cycle = 0;
    } else {
      ++cycle;    
    }
  }
  */
  bus.M1 = !digitalRead(Z80_M1_PIN);
  bus.RFSH = !digitalRead(Z80_RFSH_PIN);
  bus.IORQ = !digitalRead(Z80_IORQ_PIN);
  bus.MREQ = !digitalRead(Z80_MREQ_PIN);
  bus.WR = !digitalRead(Z80_WR_PIN);
  bus.RD = !digitalRead(Z80_RD_PIN);
  addr = ((uint16_t)PINA | (PINC<<8));
  if ( bus.MREQ && bus.RD ) {
      DDRF = 0xff;
      if ( addr >= 0 && addr < mem_size ) {
        data = mem[addr];
        PORTF = data;
      } else {
        Serial.println("address error! ");
      }
      while ( !digitalRead(Z80_RD_PIN) );
  } else if ( bus.MREQ && bus.WR ) {
      DDRF = 0x00;
      //PORTF = 0xff; // pull-up
      if ( addr >= 0 && addr < mem_size ) {
        mem[addr] = PINF;
        data = mem[addr];
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
	  addr = ((uint16_t)PINA | (PINC<<8));
	  while ( !digitalRead(Z80_RFSH_PIN) );
  }
  showState();
  
  if ( !digitalRead(Z80_HALT_PIN) ) {
	  showMemory();
	  z80_bus_request();
  }
}

void showMemory() {
	for(addr = 0; addr < mem_size; ++addr) {
	  Serial.print(mem[addr]>>4&0x0f, HEX);
	  Serial.print(mem[addr]&0x0f, HEX);
	  Serial.print(" ");
	  if ((addr & 0xf) == 0xf)
		Serial.println();
	}
	Serial.println();
}

void showState() {
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
  Serial.print( addr>>12&0x0f, HEX);
  Serial.print( addr>>8&0x0f, HEX);
  Serial.print( addr>>4&0x0f, HEX);
  Serial.print( addr&0x0f, HEX);
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

