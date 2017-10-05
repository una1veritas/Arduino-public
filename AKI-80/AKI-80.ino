/*
 * for Arduino Mega (atmega 1280/2560)
 * 
 */

#include "z80opcode.h"

#define CLK_PIN 11

#define RESET_PIN 5
#define MREQ_PIN 4
#define IORQ_PIN 3
#define BUSREQ_PIN 6
#define WAIT_PIN 7
#define NMI_PIN 17
#define INT_PIN 16
#define RD_PIN 40
#define WR_PIN 41

#define M1_PIN 8
#define RFSH_PIN 9
#define BUSACK_PIN 14
#define HALT_PIN 15

void interrupt_setup() {
  pinMode(RESET_PIN, OUTPUT);
  pinMode(WAIT_PIN, OUTPUT);
  pinMode(NMI_PIN, OUTPUT);
  pinMode(INT_PIN, OUTPUT);
  pinMode(BUSREQ_PIN, OUTPUT);

  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(WAIT_PIN, HIGH);
  digitalWrite(NMI_PIN, HIGH);
  digitalWrite(INT_PIN, HIGH);
  digitalWrite(BUSREQ_PIN, HIGH);

}

void bus_setup() {
  DDRA = 0x00;
  DDRC = 0x00;
  DDRF = 0x00;

  pinMode(MREQ_PIN, INPUT);
  pinMode(IORQ_PIN, INPUT);
  pinMode(RD_PIN, INPUT);
  pinMode(WR_PIN, INPUT);
}

void status_setup() {
  pinMode(M1_PIN, INPUT);
  pinMode(RFSH_PIN, INPUT);
  pinMode(BUSACK_PIN, INPUT);
  pinMode(HALT_PIN, INPUT);
}

uint16_t addrbus() {
  return (uint16_t) PINA | ((uint16_t)PINC<<8);
}

void databus_readmode() {
  DDRF = 0x00;  
}

void databus_writemode() {
  DDRF = 0xff;
}

uint8_t databus(void) {
  return PINF;
}

uint8_t databus(uint8_t data) {
  PORTF = data;
  return data;
}

uint8_t io(uint8_t port) {
  uint8_t data;
  switch(port) {
    case 0x01:
      data = (uint8_t) Serial1.read();
    break;
  }
  return data;
}

uint8_t io(uint8_t port, uint8_t data) {
  switch(port) {
    case 0x01: 
      Serial1.write(data);
    break;
  }
  return data;
}

void start_OC1A(uint8_t presc, uint16_t top) {
  const uint8_t MODE = 4;

  cli();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (presc << CS10);
  TCCR1C |= (1 << FOC1A);

  sei();
}
  
char * hexstr(uint32_t val, uint8_t digits) {
  static char buf[8+1];
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

const uint16_t mem_size = 1<<8;
uint8_t mem[mem_size] = {
  0x21, 0x15, 0x00, 0x06, 0x00, 0x7e, 0xa7, 0x28, 
  0x06, 0xd3, 0x01, 0x04, 0x23, 0x18, 0xf6, 0x78, 
  0x32, 0x14, 0x00, 0x76, 0x00, 0x48, 0x65, 0x6c, 
  0x6c, 0x6f, 0x2e, 0x20, 0x49, 0x27, 0x6d, 0x20, 
  0x5a, 0x38, 0x30, 0x2e, 0x20, 0x0d, 0x0a, 0x50, 
  0x6c, 0x65, 0x61, 0x73, 0x65, 0x20, 0x63, 0x6f, 
  0x6e, 0x74, 0x69, 0x6e, 0x75, 0x65, 0x20, 0x6d, 
  0x79, 0x20, 0x74, 0x65, 0x73, 0x74, 0x2e, 0x0d, 
  0x0a, 0x00, 
};

void halt() {
  for(int addr = 0; addr < mem_size; addr++) {
    if ( !(addr & 0x0f) ) {
      Serial.print("\n");
      Serial.print(hexstr(addr, 4));
      Serial.print(": ");
    }
    Serial.print(hexstr(mem[addr], 2));
    Serial.print(" ");
  }
  while (1);
}

uint8_t clkpulse;
uint8_t m1byte;
const bool show_refresh = false;
void setup() {
  // put your setup code here, to run once:

  Serial.begin(19200);
  Serial.println("Hey!");
  Serial1.begin(19200);
  Serial.println("Serial1 started.");
  interrupt_setup();
  bus_setup();
  status_setup();
  start_OC1A(5,800);
  pinMode(CLK_PIN, OUTPUT);
  clkpulse = (PINB & (1<<PB5));

  digitalWrite(RESET_PIN, LOW);
  delay(1000);
  digitalWrite(RESET_PIN, HIGH);
  Serial.println("go.");
}

void loop() {
  uint16_t addr;
  uint8_t data;
  char buf[8];
  
  // put your main code here, to run repeatedly:
  if ( !(digitalRead(MREQ_PIN) || digitalRead(RD_PIN)) ) {
    clkpulse = (PINB & (1<<PB5));
    ( clkpulse ? Serial.print("H") : Serial.print("L") );
    if ( !digitalRead(M1_PIN) ) {
      m1byte++;
      Serial.print(" M  ");
    } else if ( !digitalRead(HALT_PIN) ) {
      Serial.print(" H  ");
    }else {
      m1byte = 0;
      Serial.print("  R ");
    }
    databus_writemode();
    addr = addrbus();
    data = mem[addr & (mem_size-1)];
    databus(data);
    while ( !(digitalRead(MREQ_PIN) || digitalRead(RD_PIN)) ) ;
    databus_readmode();
    Serial.print("[");
    Serial.print(hexstr(addr, 4)); //Serial.print(addr, HEX);
    Serial.print("] ");
    Serial.print(hexstr(data,2));
    if ( m1byte ) {
      Serial.print("  ");
      Serial.print(opcode(data,buf));
    }
    Serial.println();
  } else if ( !(digitalRead(MREQ_PIN) || digitalRead(WR_PIN)) ) {
    clkpulse = (PINB & (1<<PB5));
    ( clkpulse ? Serial.print("H") : Serial.print("L") );
    Serial.print("  W ");
    databus_readmode();
    addr = addrbus();
    data = databus();
    mem[addr] = data;
    while ( !(digitalRead(MREQ_PIN) || digitalRead(WR_PIN)) ) ;
    Serial.print("[");
    Serial.print(hexstr(addr, 4));
    Serial.print("] ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( !(digitalRead(IORQ_PIN) || digitalRead(RD_PIN)) ) {
    clkpulse = (PINB & (1<<PB5));
    ( clkpulse ? Serial.print("H") : Serial.print("L") );
    Serial.print("  I ");
    databus_writemode();
    addr = addrbus();
    data = io(addr);
    databus(data); 
    while ( !(digitalRead(IORQ_PIN) || digitalRead(RD_PIN)) ); 
    databus_readmode();
    Serial.print("[");
    Serial.print(hexstr(addr & 0x00ff, 4));
    Serial.print("] ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( !(digitalRead(IORQ_PIN) || digitalRead(WR_PIN)) ) {
    clkpulse = (PINB & (1<<PB5));
    ( clkpulse ? Serial.print("H") : Serial.print("L") );
    Serial.print("  O ");
    databus_readmode();
    addr = addrbus();
    data = databus();
    io(addr,databus());
    while ( !(digitalRead(IORQ_PIN) || digitalRead(WR_PIN)) ); 
    Serial.print("[");
    Serial.print(hexstr(addr & 0x00ff, 4));
    Serial.print("] ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( show_refresh && !(digitalRead(MREQ_PIN) || digitalRead(RFSH_PIN)) ) {
    Serial.print("(");
    Serial.print("  S ");
    addr = addrbus();
    while ( !(digitalRead(MREQ_PIN) || digitalRead(RFSH_PIN)) ); 
    Serial.print(hexstr(addr, 4));
    Serial.println(")");
  } 

  if ( !digitalRead(HALT_PIN) ) {
    halt();
  }
}
