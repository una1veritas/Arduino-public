/*
 RTC.h - library for DS1307/DS3231 i2c rtc
 */

// ensure this library description is only included once
#ifndef RTC_h
#define RTC_h

#if defined (ARMCMX)
#include "armcmx.h"
#include "binary.h"
#include "I2CBus.h"

#elif defined (ARDUINO)
//#include <avr/pgmspace.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <Wire.h>
#endif

// library interface description
class RTC {
	// library-accessible "private" interface
private:
#if defined(ARDUINO)
	TwoWire & wire;
#elif defined (ARMCMX)
	I2CBus & wire;
#endif
	uint8_t rSec, rDate, rCentiSec;

public:
	// cache
	uint32_t time, date;

private:
	void readRegisters(byte reg, uint8_t *, byte);
	void writeRegisters(byte reg, uint8_t *, byte);

	enum {
		DS1307_REG_SEC = 0,
		DS1307_REG_MIN,
		DS1307_REG_HR,
		DS1307_REG_DOW,
		DS1307_REG_DATE,
		DS1307_REG_MTH,
		DS1307_REG_YR,

		M41T62_REG_CENTISEC = 0,
		M41T62_REG_SEC = 1,
		M41T62_REG_MIN,
		M41T62_REG_HR,
		M41T62_REG_DOW,
		M41T62_REG_DATE,
		M41T62_REG_MTH,
		M41T62_REG_YR,
	};

	static const uint8_t I2C_CTRL_ID = B1101000; //DS1307, M41T62

	// Define register bit masks
	static const uint8_t BIT_CLOCKHALT = B10000000;

	static const uint8_t BITS_SEC = B01111111;
	static const uint8_t BITS_MIN = B01111111;
	static const uint8_t BITS_HR =  B00111111;
	static const uint8_t BITS_DOW = B00000111;
	static const uint8_t BITS_DATE =B00111111;
	static const uint8_t BITS_MTH = B00111111;
	static const uint8_t BITS_YR =  B11111111;

	static const uint16_t BASE_YEAR = 2000;

//		uint8_t bcdToDec(uint8_t b) { return (b>>4)*10 + (b&0x0f); }
//		uint8_t decToBcd(uint8_t d) { return ((d/10)<<4) + (d%10); }

	// user-accessible "public" interface

public:

#ifdef ARDUINO
  PROGMEM const static char NameOfDay[36];
  PROGMEM const static char NameOfMonth[60];
#else
  const static char NameOfDay[36];
  const static char NameOfMonth[60];
#endif

	enum DAYINDEX {
		SUN = 0, MON, TUE, WED, THU, FRI, SAT,
	};

	enum CHIP {
		CHIP_MAXIM_DS1307 = 0,
		CHIP_ST_M41T62 = 1,
	};

#if defined(ARDUINO)
	RTC(TwoWire & w, const uint8_t chip = CHIP_MAXIM_DS1307) : wire(w), time(0), date(0) {
		if ( chip == CHIP_ST_M41T62 ) {
			rSec = M41T62_REG_SEC;
			rDate = M41T62_REG_DATE;
			rCentiSec = M41T62_REG_CENTISEC;
		} else {
			rSec = DS1307_REG_SEC;
			rDate = DS1307_REG_DATE;
			rCentiSec = 0xff;
		}
	}
#elif defined (ARMCMX)
	RTC(I2CBus & w, const uint8_t chip = CHIP_MAXIM_DS1307) : wire(w), time(0), date(0) {
		if ( chip == CHIP_ST_M41T62 ) {
			rSec = M41T62_REG_SEC;
			rDate = M41T62_REG_DATE;
			rCentiSec = M41T62_REG_CENTISEC;
		} else {
			rSec = DS1307_REG_SEC;
			rDate = DS1307_REG_DATE;
			rCentiSec = 0xff;
		}
	}
#endif

	RTC(const uint8_t chip = CHIP_MAXIM_DS1307) : wire(Wire), time(0), date(0) {
		if ( chip == CHIP_ST_M41T62 ) {
			rSec = M41T62_REG_SEC;
			rDate = M41T62_REG_DATE;
			rCentiSec = M41T62_REG_CENTISEC;
		} else {
			rSec = DS1307_REG_SEC;
			rDate = DS1307_REG_DATE;
			rCentiSec = 0xff;
		}
	}

	void init() {
		start();
	}
	bool begin() {
		if ( isrunning() )
			return true;
		start();
		return true;
	}
//		static int base_year() { return DS1307_BASE_YR; }

  // returns true if the value is changed from time/cal.
	boolean updateTime();
	boolean update(); // both time and calendar date.

	byte getSeconds();
	byte getCentiSeconds();
//		byte* getTimestamp(byte* );
	void setTime(const long &);
	void setCalendar(const long &);
//		long time();
//		long calendar();
//		void tweakMinutes(int);

	void start(void);
	void stop(void);
	boolean isrunning(void);

  // utility functions
	byte dayOfWeek();
  
  inline long JD2000(void) {
    return this->JD2000(date);
  }
	static long JD2000(byte y, byte m, byte d);
	static long JD2000(const long & yymmdd);
  static float CalendarDate(float jd);


  static char * copyNameOfDay(char * buf, byte d) {
#if defined(ARDUINO)
	  strcpy_P(buf, NameOfDay + d * 4);
#elif defined (ARMCMX)
	  strcpy(buf, NameOfDay + d * 4);
#endif
	  return buf;
  }

  static char * copyNameOfMonth(char * buf, byte m) {
#if defined(ARDUINO)
	  strcpy_P(buf, NameOfMonth + ((m+11) % 12) * 4);
#elif defined (ARMCMX)
	  strcpy(buf, NameOfMonth + ((m+11) % 12) * 4);
#endif
	  return buf;
  }

  static uint32_t BCD2uint(uint32_t);
  static uint32_t uint2BCD(uint32_t);

  word year() { return 2000+BCD2uint(date>>16 & 0xff); }
  byte month() { return BCD2uint(date>>8 & 0xff); }
  byte day() { return BCD2uint(date & 0xff); }
  byte hour() { return BCD2uint(time>>16 & 0xff); }
  byte minute() { return BCD2uint(time>>8 & 0xff); }
  byte second() { return BCD2uint(time & 0xff); }

};


#endif

