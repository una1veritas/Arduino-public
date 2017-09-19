#include "z80io.h"
#include "z80io_pindef.h"
#include "sram.h"
#include "sram_pindef.h"

uint8_t testprog[] = {
  0x3e,  0x00,  0xd3,  0x01,  0x3c, 0xc3,  0x02,  0x00,
};

uint8_t ioreq = 1;
void setup() {
  Serial.begin(19200);
  Serial.println("Hello.");

  sram_init();
  for(int i = 0; i < sizeof testprog; i++) {
    sram_write(i,testprog[i]);
  }
  sram_bus_release();
  DATA_DIR = 0;
  
  init_z80io();
  // OCA1 as Z80 clock source
  init_Timer1(5,1600);

  z80io_reset();

  if ( z80_busreq() ) {
    Serial.println("BUSREQ succeeded.");
  } else {
    Serial.println("BUSREQ failed?");
  }
  delay(1000);
  z80_busactivate();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("DATA_IN = ");
  Serial.println(DATA_IN, HEX);
    delay (100);
}
