#include <Wire.h>
#include <RTC.h>

RTC rtc(RTC::CHIP_ST_M41T62);


void setup() {
  Wire.begin();
  rtc.begin();
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
      Serial.print( rtc.time, HEX );
      Serial.print(" ");
      Serial.print( rtc.date, HEX);
      Serial.print(" ");
      Serial.print( rtc.copyNameOfDay((char*) buf, rtc.dayOfWeek()) );
      Serial.println();
    } 
  }
}

