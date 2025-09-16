/*
 * RTC_I2C.h
 *
 *  Created on: 2017/08/22
 *      Author: sin
 */

#ifndef RTC_I2C_H_
#define RTC_I2C_H_

//#include <avr/pgmspace.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <Wire.h>

// library interface description
class RTC_I2C {
	// library-accessible "private" interface
private:

	bool readRegisters(const byte reg, byte * data_array, const byte n = 1);
	bool writeRegisters(const byte reg, byte * data_array, const byte n = 1);

	enum {
		REG_SECONDS = 0,
		REG_MINUTES,
		REG_HOURS,
		REG_DAY,
		REG_DATE,
		REG_MONTHCENTURY,
		REG_YEAR,

		REG_CONTROL = 0x0e,
		REG_STATUS = 0x0f,
	};

	static const byte DEV_ADDRESS = 0x68; //MAXIM DS1307, DS3231 (, ST M41T62)

	// Define register bit masks
	enum {
		BIT_CLOCKHALT = 0x80,
		BITS_SECONDS = 0b01111111,
		BITS_MINUTES = 0b01111111,
		BITS_HOURS =  0b00111111,
		BITS_DAY = 0b00000111,
		BITS_DATE =0b00111111,
		BITS_MONTH = 0b00111111,
		BITS_YEAR =  0b11111111,
	};

	static const uint16_t BASE_YEAR_HEXDEC = 0x2000;

//		uint8_t bcdToDec(uint8_t b) { return (b>>4)*10 + (b&0x0f); }
//		uint8_t decToBcd(uint8_t d) { return ((d/10)<<4) + (d%10); }

	// user-accessible "public" interface

public:

	enum DAYINDEX {
		SUN = 0, MON, TUE, WED, THU, FRI, SAT,
	};

	RTC_I2C(void) {	}

	bool begin(void) {
		// unset the ClockHalt bit to start the rtc
		// TODO : preserve existing seconds
		byte r;
		if ( readRegisters(REG_STATUS, &r) )
			return true;
		return false;
	}

	void halt(void) {
		// set the ClockHalt bit high to stop the rtc
		// this bit is part of the seconds byte
		byte r;
		readRegisters(REG_CONTROL, &r);
		r |= BIT_CLOCKHALT;
		writeRegisters(REG_CONTROL, &r);
	}

	void start(void) {
		byte r;
		readRegisters(REG_CONTROL, &r);
		r &= ~BIT_CLOCKHALT;
		writeRegisters(REG_CONTROL, &r);
	}

	void setTime(const byte * data) {
		writeRegisters(REG_SECONDS, data, 3);
	}

	void setTime(const long & hhmmss) {
		byte data[3];
		data[0] = ((hhmmss / 10) % 10)<<4 | (hhmmss % 10);
		data[1] = ((hhmmss / 100) / 10)<<4 | ((hhmmss / 1000) % 10);
		data[2] = ((hhmmss / 1000) / 10)<<4 | ((hhmmss / 10000) % 10);
		writeRegisters(REG_SECONDS, data, 3);
	}

	void readTime(byte * data) {
		readRegisters(REG_SECONDS, data, 3);
	}

	void readTime(uint32_t & hexdec) {
		byte data[3];
		readRegisters(REG_SECONDS, data, 3);
		hexdec = data[2];
		hexdec <<= 8;
		hexdec |= data[1];
		hexdec <<= 8;
		hexdec |= data[0];
	}

	void readTime(long & hhmmss) {
		byte data[3];
		readRegisters(REG_SECONDS, data, 3);
		hhmmss = (data[0]>>4)*10 + (data[0]& 0x0f);
		hhmmss += (data[1]>>4)*1000 + (data[1]& 0x0f)*100;
		hhmmss += (data[2]>>4)*100000 + (data[2]& 0x0f)*10000;
	}

	byte readDay() {
		byte data;
		readRegisters(REG_DAY, &data);
		return data;
	}

	uint32_t readCalDate() {
		byte data[4];
		uint32_t cal;
		readRegisters(REG_DATE, data, 3);
		cal = BASE_YEAR_HEXDEC;
		cal |= data[2];
		cal <<= 8;
		cal |= data[1];
		cal <<= 8;
		cal |= data[0];
		return cal;
	}
};

#endif /* RTC_I2C_H_ */
