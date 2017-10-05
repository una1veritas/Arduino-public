#include "Z80_pindef.h"
#include "Z80.h"
//#include "sram.h"

const uint16_t mem_size = 256;
uint8_t mem[mem_size] = {
0x31, 0xff, 0x00, 0x21, 0x1c, 0x00, 0x3e, 0x00, 
0x32, 0x29, 0x00, 0x3a, 0x29, 0x00, 0x3c, 0x32, 
0x29, 0x00, 0x7e, 0xa7, 0x28, 0x05, 0xd3, 0x01, 
0x23, 0x18, 0xf0, 0x76, 0x48, 0x69, 0x2c, 0x20, 
0x74, 0x68, 0x65, 0x72, 0x65, 0x2e, 0x0a, 0x0d, 
0x00, 0x17,
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

  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  
  Serial.println("init Z80 and inactivate.");
  clock_start(5, 400);
  clock_enable();
  Z80_init();
  Z80_busactivate();
//  Z80_businactivate();
/*
  Serial.println("Z80 inactivated.");
  sram_bus_activate();
  sram_select();
  for(int i = 0; i < 256; i++)
    sram_write(i,0);
  sram_deselect();
  sram_bus_inactivate();
*/
  Serial.println("Z80 reset.");
//  Z80_busactivate();
  Z80_reset();

}

void loop() {
  uint16_t addr;
  uint8_t data;
  static uint8_t m1;
  
  if ( Z80_mreq_read() ) {
    m1 = Z80_m1();
    databus_outputmode();
    addr = addrbus_read();
    data = mem[addr & 0xff];
    databus_write(data);
    while ( Z80_mreq_read() );
    databus_inputmode();
    Serial.print(hexstr(addr, 4));
    if ( m1 ) {
      Serial.print(" M  ");
    } else {
      Serial.print(" R  ");
    }
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( Z80_mreq_write() ) {
    databus_inputmode();
    addr = addrbus_read();
    data = databus_read();
    mem[addr & 0xff] = data;
    while ( Z80_mreq_write() );
    Serial.print(hexstr(addr, 4));
    Serial.print("  W ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( Z80_iorq_read() ) {
    Z80_wait_request();
    databus_outputmode();
    addr = addrbus_read();
    data = databus_write(0xaa);
    Z80_wait_cancel();
    while ( Z80_iorq_read() );
    databus_inputmode();
    Serial.print(hexstr(addr, 4));
    Serial.print(" I  ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( Z80_iorq_write() ) {
    Z80_wait_request();
    databus_inputmode();
    addr = addrbus_read();
    data = databus_read();
    Z80_wait_cancel();
    while ( Z80_iorq_write() );
    Serial.print(hexstr(addr, 4));
    Serial.print("  O ");
    Serial.print(hexstr(data,2));
    if ( isprint(data) ) {
      Serial.print(" '");
      Serial.print((char)data);
      Serial.print('\'');
    }
    Serial.println();    
  } else if ( !digitalRead(Z80_HALT_PIN) ) {
    Serial.println("halted.");
    clock_disable();
    while ( !digitalRead(Z80_HALT_PIN) );
    clock_enable();
  }
}
