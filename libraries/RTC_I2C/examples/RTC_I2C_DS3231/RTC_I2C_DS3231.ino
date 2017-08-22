#include <Wire.h>
#include "RTC_I2C.h"

byte data[8];

RTC_I2C rtc;

byte hms[4] = { 0, 0, 0, 0, };
long lastmillis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);

  Wire.begin();

  Serial.println("Start up");
  rtc.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
	long adjustval;

	if ( millis() > lastmillis + 100 ) {
		rtc.readTime(hms);
		lastmillis = millis();

		if ( hms[0] != hms[3] ) {
			  Serial.print(hms[2]>>4, HEX);
			  Serial.print(hms[2]&0x0f, HEX);
			  Serial.print(':');
			  Serial.print(hms[1]>>4, HEX);
			  Serial.print(hms[1]&0x0f, HEX);
			  Serial.print(':');
			  Serial.print(hms[0]>>4, HEX);
			  Serial.print(hms[0]&0x0f, HEX);
			  Serial.println();

			  hms[3] = hms[0];

			  if ( hms[0] == 0 ) {
				Serial.println(rtc.readCalDate(), HEX);
			}
		}
	}

	if ( Serial.available() > 0 ) {
		char buf[16];
		int i = 0;
		while ( Serial.available() > 0 ) {
			buf[i++] = (char) Serial.read();
			delay(5);
		}
		if ( tolower(buf[0]) == 'a' ) {
			Serial.println(buf);
			//advance
			adjustval = strtol(buf+1, NULL, 16);
			Serial.println(adjustval, HEX);

			rtc.readTime(hms);
			hms[0] += adjustval & 0xff;
			hms[1] += adjustval>>8 & 0xff;
			hms[2] += adjustval>>16 & 0xff;
			rtc.setTime(hms);

		}
	}
}
