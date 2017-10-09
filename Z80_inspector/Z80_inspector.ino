/*
 * Z80 CPU inspector
 * To run small programs on Z80 CPU
 * w/ OC1A clock source and Serial SRAM
 * 
 */
#include <avr/pgmspace.h>

#include <SPI.h>
#include "SPISRAM.h"

#include "Z80.h"

const int SRAM_CS = 53;
const int SRAM_HOLD = 14; // / SIO3
const int SRAM_SIO2 = 15;

SPISRAM spisram(SRAM_CS, SPISRAM::BUS_MBits); // CS pin

char * opcode(uint32_t code, char * buf);


const uint16_t mem0000_size = (1<<8);
const uint8_t mem0000[mem0000_size] PROGMEM = {
  0xc3, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xc3, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x31, 0x00, 0x02, 0x21, 0x99, 0x00, 0xcd, 0x8a, 
  0x00, 0x76, 0x06, 0x00, 0x0e, 0x02, 0x7e, 0xa7, 
  0xc8, 0xed, 0xa3, 0x78, 0x32, 0xab, 0x00, 0x18, 
  0xf5, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 
  0x49, 0x27, 0x6d, 0x20, 0x5a, 0x38, 0x30, 0x21, 
  0x0a, 0x0d, 0x00, 0x17, 
};

const uint8_t MEM_SKIPCHECK = 1;
uint32_t longopcode = 0;

void setup() {
  Serial.begin(57600);
  while (!Serial) {}
  Serial1.begin(57600);
  
  // All SPI devices must be inactivated in advance
  // by pulling CSs high.
  pinMode(SRAM_CS, OUTPUT);
  digitalWrite(SRAM_CS, HIGH);
  
  SPI.begin();
  spisram.begin();

  if ( !MEM_SKIPCHECK ) {
    Serial.println("\nSPI Sram check");
    spisram_check(0x20000);
  }
  Serial.println("\nLoad program to SPI Sram");
  spisram_load(mem0000, mem0000_size);
  
  Z80_bus_init();
  Z80_status_init();
  OC1A_clock_start(4,1600);
  Serial.println("Z80 is going to reset.");
  Z80_reset(LOW);
  delay(500);
  Z80_reset(HIGH);
  Serial.println("Z80 is started.");
  
}

void loop() {
  uint16_t address;
  uint8_t data;
  uint8_t m1;
  char buf[16];
  
  if ( !(Z80_mreq() || Z80_read()) ) {
    m1 = Z80_M1();
    address = Z80_addressbus();
    data = spisram.read(address);
    Z80_databus_write(data);
    while ( !(Z80_mreq() || Z80_read()) );
    Z80_databus_mode(INPUT);
    if ( !m1 ) {
      Serial.print("M1 ");
    } else {
      Serial.print("R  ");
    }
    Serial.print("[");
    Serial.print(hexstr(address,4));
    Serial.print("] ");
    Serial.print(hexstr(data,2));
    if ( !m1 ) {
      opcode(data,buf);
      if ( longopcode ) {
        longopcode <<= 8;
        longopcode |= data;
        opcode(longopcode,buf);
        Serial.print("\t");
        Serial.print(buf);
        longopcode = 0;
      } else {
        opcode(data,buf);
        Serial.print("\t");
        Serial.print(buf);
        if ( buf[0] == '*' )
          longopcode = data;
        else
          longopcode = 0;
      }
    }
    Serial.println();
  } else if ( !(Z80_mreq() || Z80_write()) ) {
    address = Z80_addressbus();
    data = Z80_databus();
    while ( !(Z80_mreq() || Z80_write()) );
    spisram.write(address,data);
    Serial.print(" W ");
    Serial.print("[");
    Serial.print(hexstr(address,4));
    Serial.print("] ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( !(Z80_iorq() || Z80_read()) ) {
    // IN instruction
    address = Z80_addressbus();
    data = io(address & 0xff);
    Z80_databus_write(data);
    while ( !(Z80_iorq() || Z80_read()) );
    Z80_databus_mode(INPUT);
    Serial.print("I  ");
    Serial.print("[");
    Serial.print(hexstr(address,4));
    Serial.print("] ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( !(Z80_iorq() || Z80_write()) ) {
    // OUT instruction
    address = Z80_addressbus();
    data = Z80_databus();
    io(address & 0xff, data);
    while ( !(Z80_iorq() || Z80_write()) );
    Serial.print(" O ");
    Serial.print("[");
    Serial.print(hexstr(address,4));
    Serial.print("] ");
    Serial.print(hexstr(data,2));
    if ( isprint(data) ) {
      Serial.print(" ");
      Serial.print((char)data);
    }
    Serial.println();
  } else if ( !Z80_halt() ) {
    pinMode(Z80_CLK_PIN, INPUT);
    Serial.println("Halted.");
    while (1);
  }
}

uint8_t io(uint8_t port) {
  uint8_t data;
  switch(port) {
    case 0x00:
      data = (Serial1.available() ? 0xff : 0);
    break;
    case 0x01:
      data = (uint8_t) Serial1.read();
    break;
    case 0x15:
      // select disk result
    break;
    case 0x22:
      // drive func result
    break;
  }
  return data;    
}

uint8_t io(uint8_t port, uint8_t data) {
  switch(port) {
    case 0x01:
    case 0x02:
      Serial1.write(data);
    break;
    case 0x15:
      // select disk
    break;
    case 0x16:
      // set track low
    break;
    case 0x17:
      // set track high
    break;
    case 0x18:
      // set sector?
    break;
    case 0x20:
    break;
      // set dma low
    case 0x21:
      // set dma high
    break;
    case 0x22:
      // drive function
    break;
  }
  return data;
}

static uint8_t prescaler;
void OC1A_clock_start(uint8_t presc, uint16_t top) {
  const uint8_t MODE = 4;
  prescaler = presc;
  cli();
  
  TCCR1A = 0; TCCR1B = 0; TCCR1C = 0; 
  
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (prescaler << CS10);
  TCCR1C |= (1 << FOC1A);

  sei();
}

void OC1A_clock_stop() {
  TCCR1B &= ~(0x07<<CS10);
}

void OC1A_clock_restart() {
  TCCR1B |= (prescaler<<CS10);
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

void spisram_check(uint32_t addrmax) {
  uint32_t block = 1<<13;
  uint32_t seed = 1257;
  addrmax &= (1<<spisram.buswidth())-1;
  for(uint32_t addr = 0; addr < addrmax; addr += block) {
    uint8_t data;
    uint16_t errcount = 0;
    randomSeed(seed);
    for(uint32_t i = 0; i < block; ++i) {
      data = random(0,256);
      spisram.write(addr+i, data);
    }
    randomSeed(seed);
    for(uint32_t i = 0; i < block; ++i) {
      data = random(0,256);
      uint8_t rval = spisram.read(addr+i);
      if ( rval != data ) 
        errcount++;
    }
    Serial.print(hexstr(addr, 6));
    Serial.print(" -- ");
    Serial.print(hexstr(addr+(block-1), 6));
    if ( errcount == 0 ) {
      Serial.print(" ok");
    } else {
      Serial.print(" errors ");      
      Serial.print(errcount);
    }
    Serial.println();
  }
}

void spisram_test() {
    char text[128] = "Awake, arise, or be forever fallen!";

  long count = 0, err = 0;
  Serial.println("\nRandom read/write...");
  while ( count < 256 ) {
    long addr;
    if ( spisram.buswidth()== 16 ) {
      addr = random() & 0xffff;
    } else {
      addr = random() & 0xffffff;
    }
    Serial.print("0x");
    Serial.print( hexstr(addr, 6));
    Serial.print(": ");
    for(int i = 0; i < 8; i++) {
      text[i] = random() & 0xFF;
      Serial.print( " " );
      Serial.print( hexstr((uint8_t)text[i],2) );
      spisram.write(addr+i, text[i]);
    }
    Serial.print( "/" );
    for(int i = 0; i < 8; i++) {
      uint8_t t = (uint8_t) spisram.read(addr+i);
      Serial.print( " " );
      Serial.print( hexstr(t, 2) );
      count++;
      if ((byte)text[i] != t )
        err++;
    }
    Serial.println();
  }
  Serial.print("error count = ");
  Serial.print(err);
  Serial.print(" of ");
  Serial.println(count);
}

void spisram_load(const uint8_t * mem, uint16_t msize) {

  for(uint16_t i = 0; i < msize; ++i) {
    uint8_t code = pgm_read_byte(mem+i);
    spisram.write(i,code);
  }
  for(uint16_t i = 0; i < msize; ++i) {
    if ( (i& 0x0f) == 0x00 ) {
      Serial.print(hexstr(i, 4));
      Serial.print(": ");
    }
    Serial.print(hexstr(spisram.read(i), 2));
    Serial.print(" ");
    if ( (i& 0x0f) == 0x0f ) {
      Serial.println();
    }
  }
  Serial.println();  
}

