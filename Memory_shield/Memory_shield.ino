#include "sram.h"
#include "sd_def.h"

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
        errcount++;
      }
      //if ( (i&0x0f) == 0x0f)
      //  Serial.println();
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

void setup() {
  // put your setup code here, to run once:

  Serial.begin(19200);
  while (!Serial);
  Serial1.begin(19200);
  while (!Serial1);

  Serial.println("me.");
  Serial1.println("Hi.");

  sram_bus_init();
  if ( !sram_check(0x80000) ) {
    Serial.println("sram_check passed.");
  }

} 

void loop() {
  // put your main code here, to run repeatedly:

}
