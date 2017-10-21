#include <Wire.h>
#include "SerialEEPROM.h"

EEPROM_I2C rom;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(19200);
  Serial.println("Hi.");

  Wire.begin();
  rom.reset();
  Serial.println(rom.begin());
  Serial.println(rom.read(0x10005), HEX);
  Serial.print("write: ");
  Serial.println(rom.write(0x10005, 0xaa), HEX);
  Serial.print("read again: ");
  Serial.println(rom.read(0x10005), HEX);
}

void loop() {
  // put your main code here, to run repeatedly:

}
