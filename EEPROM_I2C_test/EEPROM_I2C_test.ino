#include <Wire.h>
#include "SerialEEPROM.h"

EEPROM_I2C rom;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(19200);
  Serial.println("Hi.");

  Wire.begin();
  rom.begin();
  Serial.println(rom.read(0x100), HEX);
  Serial.print("write: ");
  Serial.println(rom.write(0x100, 0x55), HEX);
  Serial.print("read again: ");
  Serial.println(rom.read(0x100), HEX);
}

void loop() {
  // put your main code here, to run repeatedly:

}
