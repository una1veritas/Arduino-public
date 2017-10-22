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

OC1AClock z80clock(4, 6400);

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
  DDR(Z80_DATA_BUS) = 0xff;
  Z80_DATA_BUS = 0x00;
  DDR(Z80_DATA_BUS) = 0x00;
  
  Serial.println("starting Z80..");
  z80_bus_init();
  z80clock.start();
  z80_reset(500);

  z80clock.restart(3,400);
  uint16_t oldaddr = 0xffff, addr = 0;
  uint8_t data = 0;
  while (1) {
    if ( !digitalRead(Z80_RD_PIN) ) {
      addr = ((uint16_t)PINC<<8) | PINL;
      data = PINA;
      Serial.print(hexstr(addr,4));
      Serial.print(" ");
      Serial.print(hexstr(data,2));
      Serial.println();
      while ( !digitalRead(Z80_RD_PIN) ) ;
      if ( oldaddr + 1 != addr ) {
        Serial.println("leap!!!");
        while (1);
      } else {
        oldaddr = addr;
      }
    }
  }

  pinMode(Z80_BUSACK_PIN, INPUT);
  if ( !digitalRead(Z80_BUSACK_PIN) ) {
    sram_bus_init();
    Serial.println("sram_check ");
    if ( !sram_check(0x80000) ) {
      Serial.println("sram_check passed.");
    }
    sram_bus_release();
  }
} 

void loop() {
  // put your main code here, to run repeatedly:

}

