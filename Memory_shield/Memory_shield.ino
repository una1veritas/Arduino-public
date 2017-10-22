#include <SPI.h>
#include <Wire.h>

#include "common.h"
#include "sram.h"
#include "mega_def.h"
#include "Z80.h"

uint32_t sram_check(uint32_t maxaddr) {
  uint8_t randbytes[17];
  uint8_t readout;
  uint32_t errcount = 0;
  uint32_t addr;
  
  for(uint16_t i = 0; i < 17; ++i) {
    randbytes[i] = random(0, 256);
  }

  sram_select();

  uint32_t block = 0x2000;
  for(addr = 0; addr < maxaddr; addr += block ) {
    sram_bank(addr>>16 & 0x07);
    Serial.print("addr ");
    Serial.print(hexstr(addr, 6));
    Serial.print(" (bank ");
    Serial.print(hexstr(addr>>16 & 0x07, 2));
    Serial.print(")");
    for(uint16_t i = 0; i < block; i++) {
      sram_write((uint16_t)(addr+i),randbytes[(addr+i)%17]);
    }
    for(uint16_t i = 0; i < block; i++) {
      readout = sram_read((uint16_t)(addr+i));
      //Serial.print(hexstr(readout, 2));
      //Serial.print(" ");
      if ( readout != randbytes[(addr+i)%17] ) {
        Serial.println();
        Serial.print(hexstr(addr+i, 6));
        Serial.print(" ");
        Serial.print(randbytes[(addr+i)%17], HEX);
        Serial.print("/");
        Serial.print(readout, HEX);
        Serial.print(", ");
        errcount++;
        if ( errcount > 100 )
          break;
      }
      //if ( (i&0x0f) == 0x0f)
      //Serial.println();
    }
    Serial.print(" -- ");
    Serial.println(hexstr(addr+block-1, 6));
    if ( errcount > 0 ) {
      Serial.print("err occurred: ");
      Serial.println(errcount);
      break;
    }
  }
  sram_deselect();
  return errcount;
}

const uint16_t rom_0000_size = (1<<8);
const uint8_t rom_0000[rom_0000_size] PROGMEM = {
0x21, 0x5a, 0x00, 0xcd, 0x51, 0x00, 0xdb, 0x00, 
0xfe, 0xff, 0x20, 0xfa, 0xdb, 0x01, 0x47, 0xfe, 
0x1a, 0x28, 0x10, 0xd3, 0x02, 0x3e, 0x28, 0xd3, 
0x02, 0x78, 0xcd, 0x25, 0x00, 0x3e, 0x29, 0xd3, 
0x02, 0x18, 0xe3, 0x76, 0x47, 0xcb, 0x3f, 0xcb, 
0x3f, 0xcb, 0x3f, 0xcb, 0x3f, 0x21, 0x41, 0x00, 
0x85, 0x6f, 0x7e, 0xd3, 0x02, 0x78, 0xe6, 0x0f, 
0x21, 0x41, 0x00, 0x85, 0x6f, 0x7e, 0xd3, 0x02, 
0xc9, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 
0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 
0x46, 0x0e, 0x02, 0x7e, 0xa7, 0xc8, 0xed, 0xa3, 
0x18, 0xf9, 0x48, 0x69, 0x2c, 0x20, 0x49, 0x27, 
0x6d, 0x20, 0x5a, 0x38, 0x30, 0x21, 0x0a, 0x0d, 
0x00, 0x17, 
};

void sram_load(const uint8_t * mem, uint16_t msize) {
  for(uint16_t i = 0; i < msize; ++i) {
    uint8_t code = pgm_read_byte(mem+i);
    sram_write(i,code);
  }
  for(uint16_t i = 0; i < msize; ++i) {
    if ( (i& 0x0f) == 0x00 ) {
      Serial.print(hexstr(i, 4));
      Serial.print(": ");
    }
    Serial.print(hexstr(sram_read(i), 2));
    Serial.print(" ");
    if ( (i& 0x0f) == 0x0f ) {
      Serial.println();
    }
  }
  Serial.println();  
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

class OC1AClock {
  private:
    uint8_t prescaler;
    uint16_t topvalue;
  public:
  
  OC1AClock(uint8_t presc, uint16_t top) : prescaler(presc), topvalue(top) {}

  void start(void) {
    const uint8_t MODE = 4;
    cli();
    
    TCCR1A = 0; TCCR1B = 0; TCCR1C = 0; 
    
    TCNT1 = 0;
    OCR1A = topvalue - 1;
  
    TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
    TCCR1B |= ((MODE >> 2 & 0x03) << 3) |  (prescaler<<CS10);
    TCCR1C |= (1 << FOC1A);
  
    sei();
  }
  
  void stop() {
    TCCR1B &= ~(0x07<<CS10);
  }
  
  void restart(void) {
    TCCR1B |= (prescaler<<CS10);
  }

  void restart(uint8_t presc, uint16_t top) {
    stop();
    topvalue = top;
    prescaler = presc;
    cli();
    TCNT1 = 0;
    OCR1A = topvalue - 1;
    sei();
    restart();
  }
};

OC1AClock z80clock(4, 1600);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(57600);
  while (!Serial);
  Serial1.begin(57600);
  while (!Serial1);

  Serial.println("This is Serial.");
  Serial1.println("This is Serial1.");

  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  SPI.begin();

  Wire.begin();

  pinMode(MEGA_MEMEN_PIN, OUTPUT);
  digitalWrite(MEGA_MEMEN_PIN, HIGH);  // disable MREQ to CS
  pinMode(MEGA_IOWAITEN_PIN, OUTPUT);
  digitalWrite(MEGA_IOWAITEN_PIN, HIGH);  // disable IOREQ to WAIT
  
  Serial.println("starting Z80..");
  z80_bus_init();
  z80clock.start();
  z80_reset(500);

  z80_busreq(LOW);
  while ( z80_busack() );
  if ( !z80_busack() ) {
    Serial.println("BUSREQ succeeded.");
    sram_bus_init();
    Serial.println("sram_check ");
    if ( !sram_check(0x80000) ) {
      Serial.println("sram_check passed.");
    }
    sram_bus_release();
    z80_busreq(HIGH);
  }

  digitalWrite(MEGA_MEMEN_PIN, LOW);  // enable MREQ to CS

} 

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t addr = 0;
  uint8_t data = 0;

  if ( !(digitalRead(Z80_MREQ_PIN) || digitalRead(Z80_RD_PIN)) ) {
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
    Serial.print(hexstr(addr,4));
    Serial.print(" ");
    Serial.print(hexstr(data,2));
    Serial.println();
    while ( !(digitalRead(Z80_MREQ_PIN) || digitalRead(Z80_RD_PIN)) ) ;
  }
}

