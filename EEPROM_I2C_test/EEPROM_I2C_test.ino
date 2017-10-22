#include <Wire.h>
#include "SerialEEPROM.h"

EEPROM_I2C rom;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(19200);
  Serial.println("Hi.");

  //rom.reset();
  Wire.begin();

  if ( rom.begin() )
    Serial.println("rom begin succeeded.");
  else
    Serial.println("rom begin failed.");

  Serial.println("send any char to start.");
  while ( !Serial.available() );

  randomSeed(millis());
  uint32_t addr = random(0, 0x20000);
  addr |= 0xf0;
  Serial.print(addr, HEX);
  Serial.print(" --- ");
  Serial.println(addr+31, HEX);
  for(uint32_t i = 0; i < 32; i++) {
    byte val = rom.read(addr + i);
    Serial.print(val>>4, HEX);
    Serial.print(val & 0x0f, HEX);
    Serial.print(' ');  
  }
  Serial.println();
  
  char buf[32];
  char text[] = "This royal throne of kings, this scepter'd isle, \nThis earth of majesty, this seat of Mars,";
  Serial.println("block read: ");
  rom.read(addr, (byte*) buf, 32);
  for(uint32_t i = 0; i < 32; i++) {
    byte val = buf[i];
    Serial.print(val>>4, HEX);
    Serial.print(val & 0x0f, HEX);
    Serial.print(' ');  
  }
  Serial.println();
  
  Serial.println("block write: ");
  rom.write(addr, (byte*) text, 32);
  for(uint32_t i = 0; i < 32; i++) {
    byte val = text[i];
    Serial.print(val>>4, HEX);
    Serial.print(val & 0x0f, HEX);
    Serial.print(' ');  
  }
  Serial.println();

  Serial.println("read out: ");
  rom.read(addr, (byte*) buf, 32);
  for(uint32_t i = 0; i < 32; i++) {
    byte val = buf[i];
    Serial.print(val>>4, HEX);
    Serial.print(val & 0x0f, HEX);
    Serial.print(' ');  
  }
  Serial.println();

}

void loop() {
  // put your main code here, to run repeatedly:

}
