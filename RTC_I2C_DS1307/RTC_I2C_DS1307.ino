#include <Wire.h>
#include <RTC.h>

RTC rtc(RTC::CHIP_MAXIM_DS1307);
unsigned long millis_offset;

void setup() {
  Wire.begin();
  rtc.begin();
  Serial.begin(9600);
  Serial.println("Hi, there!");

  Serial.println("Syncing timer...");
    rtc.update();
    do {
      long tmp = rtc.time;
      millis_offset = millis();
      rtc.update();
      if ( tmp != rtc.time ) {
        millis_offset %= 1000;
        break;
      }
    } while (true);
  Serial.println("succeeded.");
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
  
  if ( (millis() - millis_offset)% 1000 <= 3 ) {
    rtc.update();
    if ( rtc.time != clockval ) {
//      Serial.println(millis() - millis_offset);
      clockval = rtc.time;
      Serial.print( rtc.time, HEX );
      Serial.print(" ");
      Serial.print( rtc.date, HEX);
      Serial.print(" ");
      Serial.print( rtc.copyNameOfDay((char*) buf, rtc.dayOfWeek()) );
      Serial.println();
    }
  }
}

