#include <SPI.h>

#include "DS3234.h"

PROGMEM const prog_char DS3234::nameOfDay[36]= 
	"Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat\0";
PROGMEM const prog_char DS3234::nameOfMonth[60]= 
	"Dec\0Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0";

float DS3234::JD2000(byte year, byte month, byte day) {
  /* year must be the years since the millenium 2000 */
  if ( month <= 2 ) {
	year--;
	month += 12;
  }
  int a = int(year/100);
  int b = 2 - a + int(a/4);
  return (float)(int(365.25*year) + int(30.6001*(month + 1)) + day + 1720994.5 + 0.5 + b);
}

void DS3234::init() {
  if ( cs_pin != 0xff ) {
	pinMode(cs_pin, OUTPUT);
	digitalWrite(cs_pin, HIGH);
  }
}

byte * DS3234::transfer(byte reg, byte * buf, int num) {
    byte * p = buf;
    select();
    SPI.transfer(reg);
    while ( num-- > 0 ) {
      *p = SPI.transfer(*p);
      p++;
    }
    deselect();
    return buf;
}

void DS3234::setTime(long t) {
   transfer((byte)0x80, (byte*) &t, 3);
}

void DS3234::setCalender(long c){
  byte dow = (byte)((long)(JD2000(year(c),month(c),date(c)) + 1) % 7) + 7;
  c = (c & 0xffffff00) + dow;
   transfer((byte)0x83, (byte*) &c, 4);
}

void DS3234::getTime(long & t) {
  transfer((byte)0, (byte*) &t, 3);
   t &= 0x003f7f7f;
}

void DS3234::getCalender(long & cal){
  transfer((byte)3, (byte*) &cal, 4);
  cal &= 0xff1f3f07;
}

void DS3234::getTemperature(int & t) {
    transfer((byte)0x11, (byte *) &t, 2);
    t = (t&0xff)<<8 | (t>>8&0xff);
    /*
    temp = (buf[0]<<8) | buf[1];
    temp >>= 6;
*/
    t >>= 5;
    t *= 5;
    t >>= 2;
    
}


