/*
 *  BMP085.h
 *  for Bosch BMP085 barometic pressure sensor
 *
 *  Created by Sin on 10/04/25.
 *  Copyright 2010 Kyutech. All rights reserved.
 *
 */

#include <Arduino.h>

class BMP085 {
//	static int pressure_wait_ms[4] = { 5, 8, 14, 26 };

	static const uint8_t BMP085_ADDRESS = 0x77;  // I2C address of BMP085

	//just taken from the BMP085 datasheet
	int ac1;
	int ac2; 
	int ac3; 
	unsigned int ac4;
	unsigned int ac5;
	unsigned int ac6;
	int b1; 
	int b2;
	int mb;
	int mc;
	int md;
	long b5;

	uint16_t value_UT;
	uint32_t value_UP;
	uint8_t OSS; // Over sampling setting { 0, 1, 2, 3 }
	enum {
		IDLE = 0,
		MEASURE_TEMP,
		MEASURE_PRESS,
	} status;
private:
	char read(byte r);
	uint16_t readInt(byte r);
//	byte write(byte r, byte v);
	void calibrate();

	int16_t calc_temperature();
	int32_t calc_pressure();

public:
	int32_t pressure;
	int16_t temperature;
	uint16_t waitOnRequest;
public:
	BMP085() {	}

	boolean begin() {
		OSS = 0;
		calibrate();
		status = IDLE;
		return true;
	}

	void requestTemperature();
	void requestPressure(uint8_t ss = 0);
	int16_t readTemperature();
	int32_t readPressure();
	uint16_t waitMillis() { return waitOnRequest; }
	void getTemperature();
	void getPressure();
};

