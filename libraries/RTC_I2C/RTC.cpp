 /*
  *
  */
  
#ifdef __AVR__
#include <Wire.h>
#include "RTC.h"

PROGMEM const char RTC::NameOfDay[]=
  "Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat\0";
PROGMEM const char RTC::NameOfMonth[]=
  "Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec\0";
#else
#include "i2c.h"
#include "RTC.h"

const char RTC::NameOfDay[]=
  "Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat\0";
const char RTC::NameOfMonth[]=
  "Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec\0";
#endif


uint32_t RTC::BCD2uint(uint32_t d) {
	uint32_t t;
	t = d>>28 & 0x0f;
	t *= 10;
	t += d>>24 & 0x0f;
	t *= 10;
	t += d>>20 & 0x0f;
	t *= 10;
	t += d>>16 & 0x0f;
	t *= 10;
	t += d>>12 & 0x0f;
	t *= 10;
	t += d>>8 & 0x0f;
	t *= 10;
	t += d>>4 & 0x0f;
	t *= 10;
	t += d & 0x0f;
	return t;
}

uint32_t RTC::uint2BCD(uint32_t b) {
	uint32_t t;
	t = b % 10;
	b /= 10;
	t |= (b%10)<<4;
	b /= 10;
	t |= (b%10)<<8;
	b /= 10;
	t |= (b%10)<<12;
	b /= 10;
	t |= (b%10)<<16;
	b /= 10;
	t |= (b%10)<<20;
	b /= 10;
	t |= (b%10)<<24;
	b /= 10;
	t |= (b%10)<<28;
	return t;
}

// PRIVATE FUNCTIONS

// Aquire data from the RTC chip in BCD format
// refresh the buffer
void RTC::readRegisters(uint8_t addr, uint8_t * regvals, uint8_t num) {
	// use the Wire lib to connect to tho rtc
	// reset the resgiter pointer to zero

	wire.beginTransmission(I2C_CTRL_ID);
 	wire.write(addr);
#if defined ARMCMX
	wire.endRequest();
#elif defined (ARDUINO)
	wire.endTransmission();
#endif
	// request num bytes of data
#if defined (ARDUINO)
	wire.requestFrom((uint8_t)I2C_CTRL_ID, num);
#elif defined (ARMCMX)
	wire.receiveFrom((uint8_t)I2C_CTRL_ID, num);
#endif
	for(int i = 0; i < num; i++) {
		// store data in raw bcd format
		*regvals++ = wire.read(); //Wire.receive();
	}
}

// update the data on the IC from the bcd formatted data in the buffer
void RTC::writeRegisters(uint8_t addr, uint8_t *regvals, uint8_t num)
{
	wire.beginTransmission(I2C_CTRL_ID);
	wire.write(addr); // reset register pointer
	for(int i=0; i<num; i++) {
		wire.write(*regvals++);
	}
	wire.endTransmission();
}


boolean RTC::updateTime() {
	uint32_t tmp = time;
	readRegisters( rSec, (byte *) &tmp, 3);
	tmp &= ((unsigned long)BITS_YR<<16 | (unsigned long)BITS_MTH<<8 | BITS_DATE);
	if (tmp != time) {
		time = tmp;
		return true;
	}
	return false;
}

boolean RTC::update() {
	uint8_t tmp[8];
	uint32_t t_date;
	uint32_t t_time;
	readRegisters((byte) rSec, (byte *) tmp, 7);
	t_time = (tmp[0] & BITS_SEC) | uint32_t(tmp[1] & BITS_MIN)<<8 | uint32_t(tmp[2] & BITS_HR)<<16;
	t_date = (tmp[4] & BITS_DATE) | uint32_t(tmp[5] & BITS_MTH)<<8 | uint32_t(tmp[6] & BITS_YR)<<16;
	if ( t_date != date || t_time != time ) {
		time = t_time;
		date = t_date;
		return true;
	}
  return false;
}

uint8_t RTC::getSeconds() {
	uint8_t sec;
	readRegisters((byte) rSec, (byte *) &sec, 1);
	return sec & BITS_SEC;
}

uint8_t RTC::getCentiSeconds() {
	uint8_t sec;
	readRegisters((byte) rCentiSec, (byte *) &sec, 1);
	return sec;
}

void RTC::setTime(const long & p) {
//	writeRegisters((byte *) &(p ((unsigned long)BITS_HR<<16 | BITS_MIN<<8 | BITS_SEC)),
//			(byte) DS1307_SEC, 3);
	writeRegisters((byte) rSec, (byte *) &p, 3);
}

void RTC::setCalendar(const long & p) {
	// YYMMDD
//	writeRegisters((byte*) &(p & ((unsigned long)BITS_YR<<16 | (unsigned long)BITS_MTH<<8 | BITS_DATE)), (uint8_t) DS1307_DOW, 4);
	writeRegisters((uint8_t) rDate, (byte*) &p, 3);
}

byte RTC::dayOfWeek() {
	return  (JD2000(date) + 1) % 7;
}

long RTC::JD2000(const long & yymmdd) {
	byte y = yymmdd>>16 & 0xff;
	byte m = yymmdd>>8 & 0xff;
	byte d = yymmdd & 0xff;
	y = 10* (y>>4&0x0f) + (y&0x0f);
	m = 10 * (m>>4&0x0f) + (m&0x0f);
	d = 10 * (d>>4&0x0f) + (d & 0x0f);
	return JD2000(y,m,d);
}

long RTC::JD2000(byte year, byte month, byte day) {
	/* year must be after the millenium 2000 */
	/*
	Serial.println(year, DEC);
	Serial.println(month, DEC);
	Serial.println(day, DEC);
	*/
	if (month <= 2) {
		year--;
		month += 12;
	}
	int a = int(year / 100);
	int b = 2 - a + int(a / 4);
	return long(long(365.25 * long(year)) + int(30.6001 * (month + 1)) + day
			+ 2451479.5 + 0.5 + b); // + 0.5 for getting the day of that day's noon
//	return long(long(365.25 * long(year)) + int(30.6001 * (month + 1)) + day
//			+ 1720994.5 + b);
}

long integerPart(float df) {
	return (long int) df;
}

float fractionalPart(float df) {
	return df - ((long int) df);
}

long sign(float d) {
	if ( d < 0 )
		return -1;
	else
		return 1;
}

float RTC::CalendarDate(float jd) {
	jd += 0.5f;
	long z = integerPart(jd);
	long a = z;
	float f = fractionalPart(jd);
	if ( z >= 2299161 ) {
		long alpha = integerPart( (z-1867216.25f)/36524.25f );
		a += 1 + alpha - integerPart(alpha/4);
	}
	long b = a + 1524;
	long c = integerPart( (b-122.1f)/365.25f );
	long d = integerPart(365.25f * c);
	long e = integerPart( (b-d)/30.6001f );
	long day = b - d - integerPart(30.6001f * e) + f;
	int month;
	if ( e < 13.5f ) {
		month = e - 1;
	} else {
		month = e-13;
	}
	long year;
	if ( month > 2.5f) {
		year = c - 4716;
	} else {
		year = c - 4715;
	}
  if ( year > 0 )
    return year*10000 + month*100 + day + f;
  else {
    year = -year;
    return -1*(year*10000 + month*100 + day + f);
  }
}

/*
void RTC::writeRegister(byte rg, byte val) {
	writeRegisters(rg % 0x40, (uint8_t *) &val, 1);
}

byte RTC::readRegister(byte rg) {
	byte val;
	readRegisters(rg % 0x40, (uint8_t *) &val, 1);
	return val;
}
*/

void RTC::stop(void)
{
	// set the ClockHalt bit high to stop the rtc
	// this bit is part of the seconds byte
  uint8_t r;
	readRegisters((uint8_t) rSec, &r, 1);
	r |= BIT_CLOCKHALT;
  writeRegisters(rSec, &r, 1);
}

void RTC::start(void)
{
	// unset the ClockHalt bit to start the rtc
	// TODO : preserve existing seconds
  uint8_t r;
	readRegisters((uint8_t) rSec, &r, 1);
	r &= ~BIT_CLOCKHALT;
	writeRegisters(rSec, &r, 1);
}

boolean RTC::isrunning(void)
{
  uint8_t r;
  readRegisters((uint8_t) rSec, &r, 1);
  return !(r & BIT_CLOCKHALT);
}

