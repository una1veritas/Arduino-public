#include <Wire.h>
#include "SRAM_mega.h"

SRAM sram;

void setup() {
	sram.begin();

	sram.enable();
	sram.write(0x3245, 'H');
	byte val = sram.read(0x3245);
	Serial.println((char)val);
	sram.disable();
}

void loop() {
}

