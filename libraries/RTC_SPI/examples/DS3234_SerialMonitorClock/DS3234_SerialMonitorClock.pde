#include <SPI.h>
#include "DS3234.h"

#include <string.h>

unsigned long asBCD(unsigned long lval) {
  unsigned long tmp;
  byte * p = (byte*)(&tmp);
  int i;
  for ( i = 0; i < 4; i++) {
    p[i] = lval%10;
    lval /= 10;
    p[i] |= (lval%10)<<4;
    lval /= 10;
  }
  return tmp;
}

byte decToBcd(byte d) { 
  return ((d/10)<<4) + (d%10); 
}	

DS3234 rtc(9);
unsigned long clockval;

void setup() {

  Serial.begin(9600);
  SPI.begin();
  rtc.init();

  int t;
  rtc.getTemperature(t);
  Serial.print(t/10.0);
  Serial.print(" C.  ");

  delay(500);
}

void loop() {
  long temp;
  byte buf[16], * p;
  byte c, serbuf[32];
  long idlemillis;
  int bp;

  if (Serial.available()) {
    bp = 0;
    while (Serial.available()) {
      idlemillis = millis();
      c = Serial.read();
      serbuf[bp] = c;
      bp++;
      bp %= 32;
      serbuf[bp] = 0;
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
      switch(serbuf[0]) {
      case 'T':
      case 't':
        temp = atol((char*)&serbuf[1]);
        temp = asBCD(temp);
        rtc.setTime(temp);
        break;
      case 'C':
      case 'c':
        temp = atol((char*)&serbuf[1]);
        temp = asBCD(temp);
        rtc.setCalender(temp);
        break;
      }
    }
  }

  delay(100);
  rtc.getTime(temp);
  if ( temp != clockval ) {
    clockval = temp;
    rtc.getCalender(temp);
    Serial.print(DAYS[temp&0xff]);
    Serial.print(" ");
    sprintf((char*)buf, "20%02lx-%02lx-%02lx ", temp>>24&0xff, temp>>16&0xff, temp>>8&0xff);
    Serial.print((char*)buf);
    sprintf((char*)buf, "%02lx:%02lx:%02lx ", clockval>>16&0xff, clockval>>8&0xff, clockval&0xff);
    Serial.print((char*)buf);

    /*
    int t;
     rtc.getTemperature(t);
     Serial.print(t/10.0);
     Serial.print(" C.  ");
     */
    Serial.println();
    delay(100);
  }
}


