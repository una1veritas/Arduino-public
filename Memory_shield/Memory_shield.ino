#include <SPI.h>
#include <Wire.h>

#include "common.h"
#include "sram.h"
#include "mega_def.h"
#include "Z80.h"


const uint8_t rom_0000[0x100] PROGMEM = {
  0x31, 0x00, 0x10, 0xc3, 0x00, 0x10, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xe1, 0x18, 0xc5, 
};

const uint8_t rom_1000[0x100] PROGMEM = {
  0x21, 0x11, 0x10, 0x7e, 0x32, 0x10, 0x10, 0xa7, 
  0x28, 0x05, 0xd3, 0x01, 0x23, 0x20, 0xf4, 0x76, 
  0x00, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 
  0x66, 0x72, 0x69, 0x65, 0x6e, 0x64, 0x73, 0x21, 
  0x0d, 0x0a, 0x00, 
};

void sram_load(const uint16_t addr, const uint8_t * mem, uint16_t msize) {
  for(uint16_t i = 0; i < msize; ++i) {
    uint8_t code = pgm_read_byte(mem+i);
    sram_select();
    sram_write(addr+i,code);
    sram_deselect();
  }
  for(uint16_t i = 0; i < msize; ++i) {
    if ( (i& 0x0f) == 0x00 ) {
      Serial.print(hexstr(addr+i, 4));
      Serial.print(": ");
    }
    sram_select();
    Serial.print(hexstr(sram_read(addr+i), 2));
    sram_deselect();
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

OC1AClock z80clock(4, 200);

uint8_t io_read(uint16_t port);
void io_write(uint16_t port, uint8_t data);

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

//  pinMode(MEGA_MEMEN_PIN, OUTPUT);
//  digitalWrite(MEGA_MEMEN_PIN, HIGH);  // disable MREQ to CS
//  pinMode(MEGA_IOWAITEN_PIN, OUTPUT);
//  digitalWrite(MEGA_IOWAITEN_PIN, HIGH);  // disable IOREQ to WAIT
  
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
    if ( !memory_check(0, 0x10000 /* 0x80000 */) ) {
      Serial.println("sram_check passed.");
      sram_load(0x0000, rom_0000, sizeof(rom_0000));
      sram_load(0x1000, rom_1000, sizeof(rom_1000));
    }
    sram_bus_release();
  }

//  digitalWrite(MEGA_MEMEN_PIN, LOW);  // enable MREQ to CS
  z80_busreq(HIGH);
  z80_reset(500);

} 

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t addr = 0;
  uint8_t data = 0;

  if ( !(digitalRead(Z80_MREQ_PIN) || digitalRead(Z80_RD_PIN)) ) {
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
    Serial.print(hexstr(addr,4));
    Serial.print(" R  ");
    Serial.print(hexstr(data,2));
    Serial.println();
    while ( !(digitalRead(Z80_MREQ_PIN) || digitalRead(Z80_RD_PIN)) ) ;
  } else if ( !(digitalRead(Z80_MREQ_PIN) || digitalRead(Z80_WR_PIN)) ) {
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
    Serial.print(hexstr(addr,4));
    Serial.print("  W ");
    Serial.print(hexstr(data,2));
    Serial.println();
    while ( !(digitalRead(Z80_MREQ_PIN) || digitalRead(Z80_WR_PIN)) ) ;
  } else if ( !(digitalRead(Z80_IORQ_PIN) || digitalRead(Z80_WR_PIN)) ) {
    addr = ((uint16_t)PINC<<8) | PINL;
    data = PINA;
    digitalWrite(Z80_WAIT_PIN, LOW);
    io_write(addr, data);
    digitalWrite(Z80_WAIT_PIN, HIGH);
    Serial.print(hexstr(addr,4));
    Serial.print("  O ");
    Serial.print(hexstr(data,2));
    Serial.println();    
    while ( !(digitalRead(Z80_IORQ_PIN) || digitalRead(Z80_WR_PIN)) ) ;
  } else if ( !(digitalRead(Z80_IORQ_PIN) || digitalRead(Z80_RD_PIN)) ) {
    addr = ((uint16_t)PINC<<8) | PINL;
    digitalWrite(Z80_WAIT_PIN, LOW);
    data = io_read(addr);
    digitalWrite(Z80_WAIT_PIN, HIGH);
    DDR(PORTA) = 0xff;
    PORTA = data;
    Serial.print(hexstr(addr,4));
    Serial.print(" I  ");
    Serial.print(hexstr(data,2));
    Serial.println();    
    while ( !(digitalRead(Z80_IORQ_PIN) || digitalRead(Z80_RD_PIN)) ) ;
    DDR(PORTA) = 0x00;
    PORTA = 0x00;
  }

}

uint32_t memory_check(uint32_t startaddr, uint32_t maxaddr) {
  uint32_t errs, errtotal = 0;
  uint32_t block = 0x2000;
  for(uint32_t addr = startaddr; addr < maxaddr; addr += block ) {
    sram_bank(addr>>16 & 0x07);
    Serial.print("[bank ");
    Serial.print(hexstr(addr>>16 & 0x07, 2));
    Serial.print("] ");
    Serial.print(hexstr(addr & 0xffff, 4));
    Serial.print(" - ");
    Serial.print(hexstr( (addr+block-1) & 0xffff, 4));
    
    if ( (errs = sram_check(addr & 0xffff, block)) > 0 ) {
      errtotal += errs;
      Serial.print(" err: ");
      Serial.println(errs);
    } else {
      Serial.println(" ok. ");      
    }
  }
  return errtotal;
}

uint8_t io_read(uint16_t port) {
  switch(port & 0xff) {
  case 0x00:
    if ( Serial1.available() )
      return 0xff;
      return 0x00;
    break;
  case 0x01:
    return (uint8_t) Serial1.read();
    break;
  case 0x04:
    return (uint8_t) Serial1.read();
    break;
  case 0x40:
    // timer/clock control
    break;
  }
  return 0;
}

void io_write(uint16_t port, uint8_t data) {
  switch(port & 0xff) {
  case 0x01:
    Serial1.write(data);
    break;
  case 0x04:
    Serial1.write(data);
    break;
  case 0x40:
    // timer/clock control
    break;
  }
  return;
}

