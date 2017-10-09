#include "Z80_pindef.h"
#include "Z80.h"
#include "sram.h"

void OC1A_start(uint8_t presc, uint16_t top);
void OC1A_enable();
void OC1A_disable();

uint32_t sram_check(uint32_t maxaddr);

char * opcode(uint32_t code, char * buf);

const uint16_t mem_size = (1<<8);
uint8_t mem[mem_size] = {
0x31, 0xff, 0x00, 0x21, 0x18, 0x00, 0x06, 0x00, 
0x78, 0x3c, 0x32, 0x27, 0x00, 0x47, 0x7e, 0xa7, 
0x28, 0x05, 0xd3, 0x01, 0x23, 0x18, 0xf1, 0x76, 
0x48, 0x69, 0x2e, 0x20, 0x49, 0x27, 0x6d, 0x20, 
0x5a, 0x38, 0x30, 0x2e, 0x0a, 0x0d, 0x00, 0x17, 
};


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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hi.");
  
  sram_bus_init(); 
  sram_select();

  //Serial.print("sram check ");
  //Serial.println(sram_check(0x20000), HEX);

  Serial.println("loading program...");
  for(uint16_t i = 0; i < mem_size; i++) {
    sram_write(i,mem[i]);
  }
  for(uint16_t i = 0; i < mem_size; i++) {
    if ( (i&0x0f) == 0 ) {
      Serial.println();
      Serial.print(hexstr(i,4));
      Serial.print(": ");
    }
    Serial.print(hexstr(sram_read(i),2));
    Serial.print(" ");
  }
  Serial.println();
  sram_deselect();

  OC1A_start(5, 1600);
  OC1A_enable();
  Z80_bus_init();
  Z80_reset();

  DDRL = 0x00; DDRK = 0x00;
}

void loop() {
  uint16_t addr;
  uint8_t data;
  char buf[16];
  static uint8_t m1;

  if ( Z80_mreq_read() ) {
    m1 = Z80_m1();
    addr = (PINL | (uint16_t) PINK<<8);
    data = mem[addr & 0xff];
    DDRF = 0xff;
    PORTF = data;
    while ( Z80_mreq_read() );
    DDRF = 0x00;
    if ( m1 ) {
      Serial.print(" M  ");
      Serial.print("[");
      Serial.print(hexstr(addr, 4));
      Serial.print("] ");
      Serial.print(hexstr(data,2));
      opcode(data,buf);
      m1 = buf[0];
      Serial.print("\t");
      Serial.print(buf+1);
      Serial.println();
    } else {
      Serial.print(" R  ");
      Serial.print("[");
      Serial.print(hexstr(addr, 4));
      Serial.print("] ");
      Serial.print(hexstr(data,2));
      Serial.println();
    }
  } else if ( Z80_mreq_write() ) {
    addr = (PINL | (uint16_t) PINK<<8);
    DDRF = 0x00; PORTF = 0;
    data = PINF;
    mem[addr & 0xff] = data;
    while ( Z80_mreq_write() );
    Serial.print("  W ");
    Serial.print("[");
    Serial.print(hexstr(addr, 4));
    Serial.print("] ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( Z80_iorq_read() ) {
    Z80_wait_request();
    DDRF = 0xff;
    addr = (PINL | (uint16_t) PINK<<8);
    PORTF = 0xaa;
    Z80_wait_cancel();
    while ( Z80_iorq_read() );
    DDRF = 0x00;
    Serial.print(hexstr(addr, 4));
    Serial.print(" I  ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( Z80_iorq_write() ) {
    DDRF = 0x00;
    Z80_wait_request();
    addr = (PINL | (uint16_t) PINK<<8);
    data = PINF;
    Z80_wait_cancel();
    while ( Z80_iorq_write() );
    Serial.print("  O ");
    Serial.print("[");
    Serial.print(hexstr(addr, 4));
    Serial.print("] ");
    Serial.print(hexstr(data,2));
    if ( isprint(data) ) {
      Serial.print(" '");
      Serial.print((char)data);
      Serial.print('\'');
    }
    Serial.println();    
  } else if ( !digitalRead(Z80_HALT_PIN) ) {
    Serial.println("halted.");
    OC1A_disable();
    while ( !digitalRead(Z80_HALT_PIN) );
    OC1A_enable();
  }
}

void OC1A_start(uint8_t presc, uint16_t top) {
  const uint8_t MODE = 4;
  cli();
  
  TCCR1A = 0; TCCR1B = 0; TCCR1C = 0; 
  
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (presc << CS10);
  TCCR1C |= (1 << FOC1A);

  sei();
}

void OC1A_disable() {
  pinMode(Z80_CLK_PIN, INPUT);
  digitalWrite(Z80_CLK_PIN, LOW);
}

void OC1A_enable() {
  pinMode(Z80_CLK_PIN, OUTPUT);
}


uint32_t sram_check(uint32_t maxaddr) {
  uint8_t randbytes[17];
  uint8_t readout;
  uint32_t errcount = 0;
  uint32_t addr;
  
  for(uint16_t i = 0; i < 17; ++i) {
    randbytes[i] = random(0, 256);
  }

  uint32_t block = 0x2000;
  for(addr = 0; addr < maxaddr; addr += block ) {
    Serial.println(addr, HEX);
    for(uint16_t i = 0; i < block; i++) {
      sram_write(addr+i,randbytes[(addr+i)%17]);
    }
    for(uint16_t i = 0; i < block; i++) {
      readout = sram_read(addr+i);
      if ( readout != randbytes[(addr+i)%17] ) {
        errcount++;
      }
    }
    if ( errcount > 0 ) {
      Serial.print("err occurred: ");
      Serial.println(errcount);
      break;
    }
  }
  return addr;
}


