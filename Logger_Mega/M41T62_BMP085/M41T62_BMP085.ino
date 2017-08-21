#include <Wire.h>
#include <RTC.h>
#include "BMP085.h"

RTC rtc(RTC::CHIP_ST_M41T62);
BMP085 bmp;

void setup() {
  Wire.begin();
  rtc.begin();
  bmp.begin();
  Serial.begin(9600);
  Serial.println("Hi, there!");
}

long clockval;
void loop() {
  long temp = 0;
  long cal;
  byte c, buf[32];
  int bp;
  
  long idlemillis;
  
  if (Serial.available()) {
    bp = 0;
    while (Serial.available()) {
      idlemillis = millis();
      c = Serial.read();
      buf[bp] = c;
      bp++;
      bp %= 32;
      buf[bp] = 0;
      while (!Serial.available()) {
        if (millis() - idlemillis > 500)
          break;
      }
      if ( millis() - idlemillis > 500 
        || c == '.' ) {
          break;
      }
    }
    
    if ( c == '.' ) {
      switch(buf[0]) {
      case 'T':
      case 't':
        temp = atol((char*)&buf[1]);
        temp = RTC::uint2BCD(temp);
        Serial.println(temp,HEX);
        rtc.setTime(temp);
        break;
      case 'C':
      case 'c':
        temp = atol((char*)&buf[1]);
        temp = RTC::uint2BCD(temp);
        Serial.println(temp,HEX);
        rtc.setCalendar(temp);
        break;
      }
    }
  }
  
  if ( rtc.getCentiSeconds() == 0 ) {
    rtc.update();
    if ( rtc.time != clockval ) {
      clockval = rtc.time;
  //    Serial.println(rtc.getCentiSeconds(), HEX);
      Serial.print( rtc.time>>20&0x0f, HEX );
      Serial.print( rtc.time>>16&0x0f, HEX );
      Serial.print(':');
      Serial.print( rtc.time>>12&0x0f, HEX );
      Serial.print( rtc.time>>8&0x0f, HEX );
      Serial.print(':');
      Serial.print( rtc.time>>4&0x0f, HEX );
      Serial.print( rtc.time&0x0f, HEX );
      Serial.print(" 20");
      Serial.print( rtc.date>>20&0x0f, HEX);
      Serial.print( rtc.date>>16&0x0f, HEX);
      Serial.print('/');
      Serial.print( rtc.date>>12&0x0f, HEX);
      Serial.print( rtc.date>>8&0x0f, HEX);
      Serial.print('/');
      Serial.print( rtc.date>>4&0x0f, HEX);
      Serial.print( rtc.date&0x0f, HEX);
      Serial.print(" ");
      Serial.print( rtc.copyNameOfDay((char*) buf, rtc.dayOfWeek()) );
      //
      bmp.requestTemperature();
      delay(bmp.waitOnRequest);
      bmp.getTemperature();
      Serial.print(", ");
      Serial.print(bmp.temperature/10.0,1);
      Serial.print(" C., ");
      bmp.requestPressure(1);
      delay(bmp.waitOnRequest);
      bmp.getPressure();
      Serial.print( float((bmp.pressure+5)/10) / 10.0, 1);
      Serial.print(" hP\n");
    } 
  }
}

