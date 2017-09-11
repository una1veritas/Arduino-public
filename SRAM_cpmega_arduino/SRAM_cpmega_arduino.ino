#include "sram.h"


void sram_init() {
  LOW4_DIR |= LOW4_MASK;
  HIGH4_DIR |= HIGH4_MASK;
  
  SRAM_CONTROL_DIR |= ( SRAM_CS | SRAM_OE | SRAM_WE); 
  SRAM_CONTROL |= ( SRAM_CS | SRAM_OE | SRAM_WE);
  LATCH_CONTROL_DIR |= (LATCH_L | LATCH_H );
  LATCH_CONTROL |= (LATCH_L | LATCH_H );
}

/*
inline void data_out(uint16_t data) {
  data >>= 8;
  LOW4_DIR |= LOW4_MASK;
  HIGH4_DIR |= HIGH4_MASK;
  LOW4_OUT &= ~LOW4_MASK;
  HIGH4_OUT &= ~HIGH4_MASK;
  LOW4_OUT |= LOW4_MASK & (uint8_t) data;
  HIGH4_OUT |= HIGH4_MASK & (uint8_t) data;
  LATCH_CONTROL &= ~LATCH_H;
  LATCH_CONTROL |= LATCH_H;
}
*/

inline void addr_out(uint32_t addr) {
  LOW4_DIR |= LOW4_MASK;
  HIGH4_DIR |= HIGH4_MASK;
  
  LOW4_OUT &= ~LOW4_MASK;
  HIGH4_OUT &= ~HIGH4_MASK;
  LOW4_OUT |= LOW4_MASK & (uint8_t) addr;
  HIGH4_OUT |= HIGH4_MASK & (uint8_t) addr;
  __asm__ __volatile__("nop");
  LATCH_CONTROL &= ~LATCH_L;
  __asm__ __volatile__("nop");
  LATCH_CONTROL |= LATCH_L;

  addr >>= 8;
  LOW4_OUT &= ~LOW4_MASK;
  HIGH4_OUT &= ~HIGH4_MASK;
  LOW4_OUT |= LOW4_MASK & (uint8_t) addr;
  HIGH4_OUT |= HIGH4_MASK & (uint8_t) addr;
  __asm__ __volatile__("nop");
  LATCH_CONTROL &= ~LATCH_H;
  __asm__ __volatile__("nop");
  LATCH_CONTROL |= LATCH_H;

  // A16 and above will always be ignored
}


unsigned char sram_read(unsigned short addr) {
  unsigned char vall4, valh4;

  addr_out(addr);  // A16 is always 0

  LOW4_DIR &= ~LOW4_MASK;
  HIGH4_DIR &= ~HIGH4_MASK;
  SRAM_CONTROL &= ~SRAM_OE;
  __asm__ __volatile__("nop");
  __asm__ __volatile__("nop");
  vall4 = LOW4_IN;
  valh4 = HIGH4_IN;
  SRAM_CONTROL |= ~SRAM_OE;
  return (vall4 & LOW4_MASK) | (valh4 & HIGH4_MASK);
}

void sram_write(unsigned short addr, unsigned char data) {
  addr_out(addr);  // A16 is always 0

  LOW4_OUT &= ~LOW4_MASK;
  HIGH4_OUT &= ~HIGH4_MASK;
  LOW4_OUT |= data & LOW4_MASK;
  HIGH4_OUT |= data & HIGH4_MASK;
  __asm__ __volatile__("nop");
  __asm__ __volatile__("nop");
  SRAM_CONTROL &= ~SRAM_WE;
  SRAM_CONTROL |= SRAM_WE;
}

uint8_t crc8(uint8_t inbyte, uint8_t & crc) {
    uint8_t j;
    uint8_t mix;

    for (j = 0; j < 8; j++) {
        mix = (crc ^ inbyte) & 0x01;
        crc >>= 1;
        if (mix) crc ^= 0x8C;
        inbyte >>= 1;
    }
    return crc;
}

uint32_t addr;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hi, there.");
  Serial.println("Let's start.");

  sram_init();
  addr = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t val,crc;
  const uint32_t segsize = (1<<7);
  const uint32_t chksize = (1<<3);
  uint32_t i;
  
  crc = 0;
  Serial.print(addr, HEX);
  Serial.print(" - ");
  Serial.println(addr+segsize, HEX);
  for(i = 0; i < segsize; i++) {
    if ((i & (chksize-1)) == (chksize - 1)) {
      val = crc;
      crc = 0;
    } else {
      val = random(0xef, 256);
      crc8(val, crc);
    }
    Serial.print(val, HEX);
    Serial.print(' ');
    sram_write(addr+i, val);
  }
  Serial.println();
  delay(20);

  crc = 0;
  for(i = 0; i < segsize; i++) {
    val = sram_read(addr+i);
    crc8(val, crc);
    if ((i & (chksize-1)) == (chksize - 1)) {
      if ( crc != 0 ) break;
    } 
    Serial.print(val, HEX);
    Serial.print(' ');
  }
  Serial.println();
  if ( crc != 0 ) {
    Serial.print("error: ");
    Serial.println(addr + i, HEX); 
    while (1);
  } else {
    Serial.println("ok.");
  }

  addr += segsize;
  if ( addr >= 0x10000 )
    while (1);
  Serial.println();
  delay(500);
}

