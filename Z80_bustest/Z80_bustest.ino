
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

void showState();

uint8_t mem[] = {
  0x3e, 0x06, 0x32, 0x18, 0x00, 0x3a, 0x18, 0x00, 
  0x3c, 0xd3, 0x02, 0x32, 0x18, 0x00, 0xdb, 0x01, 
  0x32, 0x19, 0x00, 0xa7, 0xc2, 0x05, 0x00, 0x76, 
  0x00, 0x00,                  
  0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};
const uint16_t mem_size = sizeof mem;

uint8_t clkpulse = 0;
uint8_t state_M1 = 0;
uint8_t state_RFSH = 0;
uint8_t state_IORD = 0;
uint8_t state_IOWR = 0;
uint8_t state_READ = 0; 
uint8_t state_WRITE = 0; 
uint16_t cycle = 0;
uint16_t addr = 0;
uint8_t data = 0;

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
  z80_clock_start(3200);
  digitalWrite(Z80_RESET_PIN, LOW);
  delay(2000);
  digitalWrite(Z80_RESET_PIN, HIGH);
}

void loop() {
  while ( clkpulse == digitalRead(Z80_CLK_PIN) );
  if ( clkpulse != digitalRead(Z80_CLK_PIN) ) {
    clkpulse = digitalRead(Z80_CLK_PIN);
    if ( clkpulse ) {
      if ( !digitalRead(Z80_M1_PIN) && state_M1 ) {
        cycle = 0;
      } else {
        ++cycle;    
      }
    }
    addr = ((uint16_t)PINA | (PINC<<8));
    state_M1 = digitalRead(Z80_M1_PIN);
    state_IORD = digitalRead(Z80_RD_PIN) || digitalRead(Z80_IORQ_PIN);
    state_IOWR = digitalRead(Z80_WR_PIN) || digitalRead(Z80_IORQ_PIN);
    state_READ = digitalRead(Z80_RD_PIN) || digitalRead(Z80_MREQ_PIN) ;
    state_WRITE = digitalRead(Z80_WR_PIN) || digitalRead(Z80_MREQ_PIN) ;
    state_RFSH = digitalRead(Z80_RFSH_PIN) || digitalRead(Z80_MREQ_PIN);
    if ( !state_READ ) {
        DDRF = 0xff;
        if ( addr >= 0 && addr < mem_size ) {
          data = mem[addr];
          PORTF = data;
        } else {
          Serial.print("address error! ");
        }
    } else if ( !state_WRITE ) {
        for(int i = 0; i < 8; i++)
        DDRF = 0x00;
        //PORTF = 0xff; // pull-up
        if ( addr >= 0 && addr < mem_size ) {
          mem[addr] = PINF;
          data = mem[addr];
        } else {
          Serial.print("address error! ");
        }
        Serial.println(addr&0xfff0, HEX);
        for(int i = 0; i < 16; i++) {
          Serial.print(mem[(addr & 0xfff0) + i], HEX);
          Serial.print(" ");
        }
        Serial.println();
    } else if ( !state_IORD ) {
      DDRF = 0xff;
      PORTF = data;
    } else if ( !state_IOWR ) {
      DDRF = 0x00;
      PORTF = 0xff;
      data = PINF;
    } else if ( !state_RFSH ) {
      
    }
    showState();
  }
  if ( !digitalRead(Z80_HALT_PIN) ) {
    for(addr = 0; addr < mem_size; ++addr) {
      Serial.print(mem[addr]>>4&0x0f, HEX);
      Serial.print(mem[addr]&0x0f, HEX);
      Serial.print(" ");
      if ((addr & 0xf) == 0xf)
        Serial.println();
    }
    Serial.println();
    digitalWrite(Z80_BUSREQ_PIN, LOW);
    while ( !digitalRead(Z80_HALT_PIN) );
    digitalWrite(Z80_BUSREQ_PIN, HIGH);
  }
}

void showState() {
  Serial.print(cycle/10);
  Serial.print(cycle%10);
  if ( clkpulse ) {
    Serial.print(" H ");
  } else {
    Serial.print(" L ");
  }
  if ( digitalRead(Z80_M1_PIN) ) {
    Serial.print("  ");
  } else {
    Serial.print("M ");
  }

  if ( !state_IORD ) {
    Serial.print("I ");
  } else if ( !state_IOWR ) {
    Serial.print("O ");
  } else if ( !state_READ ) {
    Serial.print("R ");
  } else if ( !state_WRITE ) {
    Serial.print("W ");
  } else {
    Serial.print("  ");
  }
    
  Serial.print("[");
  Serial.print( addr>>24&0x0f, HEX);
  Serial.print( addr>>16&0x0f, HEX);
  Serial.print( addr>>8&0x0f, HEX);
  Serial.print( addr&0x0f, HEX);
  Serial.print("] ");
  if ( !state_READ || !state_WRITE || state_IORD || state_IOWR ) {
    Serial.print( data, HEX);
    Serial.print(" ");
  }  
  if ( !state_M1 && !state_READ ) {
    Serial.print("(op. ");
    opcode(data);
    Serial.print(")");
  } else if ( state_M1 && !state_READ ) {
    Serial.print("(data)");
  } else if ( !state_WRITE ) {
    Serial.print("(write)");
  } else if ( !state_RFSH ) {
    Serial.print("(refresh row)");
  } else {    
    //Serial.print("");
  }
  Serial.println();
}

