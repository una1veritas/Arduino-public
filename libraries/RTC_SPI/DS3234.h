#ifndef DS3234_H
#define DS3234_H

#include <avr/pgmspace.h>
#if ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif
#include <SPI.h>

class DS3234 {
   byte cs_pin;
public:
  PROGMEM const static prog_char nameOfDay[36];
  PROGMEM const static prog_char nameOfMonth[60];
  
  DS3234(byte pin) {
      cs_pin = pin;
      deselect();
   }
  
   void init();
   void begin() { init(); }
   void select() { 
      SPI.setClockDivider(SPI_CLOCK_DIV4);
      SPI.setDataMode(SPI_MODE1);
      SPI.setBitOrder(MSBFIRST);
      if ( cs_pin != 0xff ) 
         digitalWrite(cs_pin, LOW); 
   }
   void deselect() { 
      if ( cs_pin != 0xff ) 
         digitalWrite(cs_pin, HIGH); 
   }

   float JD2000(byte year, byte month, byte day);
  
   byte * transfer(byte reg, byte * buf, int num);
   
   void setCalender(long c);
   void setTime(long t);
   void getTime(long & t);
   void getCalender(long & c);
   void getTemperature(int & t);
  
  static const byte seconds(long t) {
	return (t>>4&0x0f)*10 + (t&0x0f);
  }
  static const byte minutes(long t) {
	return (t>>12&0x0f)*10 + (t>>8&0x0f);
  }
  static const byte hours(long t) {
	return (t>>20&0x0f)*10 + (t>>16&0x0f);
  }
  static const byte day(long d) {
	return (d>>12&0x0f)*10 + (d>>8&0x0f);
  }
  static const byte date(long d) {
	return (d>>12&0x0f)*10 + (d>>8&0x0f);
  }
  static const byte month(long d) {
	return (d>>20&0x0f)*10 + (d>>16&0x0f) ;
  }
  static const byte year(long d) {
	return (d>>28&0x0f)*10 + (d>>24&0x0f) ;
  }
  
  static const byte bcd_seconds(long t) {
	return t & 0x7f;
  }
  static const byte bcd_minutes(long t) {
	return t>>8 & 0x7f;
  }
  static const byte bcd_hours(long t) {
	return t>>16 & 0x3f;
  }
  static const byte bcd_day(long d) {
	return d&0x07;
  }
  static const byte bcd_date(long d) {
	return d>>8&0x3f;
  }
  static const byte bcd_month(long d) {
	return d>>16&0x1f;
  }
  static const byte bcd_year(long d) {
	return d>>24&0xff ;
  }
  
  static char * copyNameOfDay(char * buf, byte d) {
	strcpy_P(buf, nameOfDay + (d%7)*4);
	return buf;
  }
  
  static char * copyNameOfMonth(char * buf, byte m) {
	strcpy_P(buf, nameOfMonth + (m%12)*4);
	return buf;
  }
  
};

#endif //DS3234_H

