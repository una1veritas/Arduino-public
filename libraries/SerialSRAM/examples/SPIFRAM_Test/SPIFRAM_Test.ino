/*
 * SerialRAM_Test.cpp
 *
 *  Created on: 2013/05/14
 *      Author: sin
 */

#include <string.h>
#include <SPI.h>
#include <SPIFRAM.h>

SPIFRAM fram(10, SPIFRAM::BUSWIDTH_FM25V10);

static const char text[][64] = {
		"The mailman passes by and I just wonder why",
		"He never stops to ring my front door bell",
		"There's not a single line from that dear old love of mine",
		"No, not a word since I last heard \"farewell\"."
};

char buff[128];

void setup() {
	Serial.begin(9600);
	Serial.println("Hello.");
	delay(200);

	randomSeed(millis());
	Serial.println("random seed by millis().");

	SPI.begin();
	fram.begin();
	Serial.print("read status: ");
	Serial.println((int)fram.status(), HEX);
	Serial.println();

	uint32_t addr;
	int length;
	for(int i = 0; i < 3; i++) {
		addr = random(0, 1L<<SPIFRAM::BUSWIDTH_FM25V10);
		Serial.print(addr, HEX);
		Serial.print(": ");

		memset(buff,0,64);
		strcpy(buff, text[millis()%4]);
		length = strlen(buff);
		fram.write(addr, (byte*) buff, length+1);
		Serial.println((int)fram.status(), HEX);

		memset(buff,0,64);
		fram.read(addr, (byte*) buff, length+1);
		Serial.println(buff);
		Serial.println();
		delay(100);
	}

}

void loop() {
}
