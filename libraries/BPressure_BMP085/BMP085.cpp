/*
 *  BMP085.cpp
 *  for Bosch BMP085 barometic pressure sensor
 *
 *  Created by Sin on 10/04/25.
 *  Copyright 2010 Kyutech. All rights reserved.
 *
 */

#include <Ardunino.h>
#include <Wire.h>
#include "BMP085.h"


// Read 1 byte from the BMP085 at 'address'
char BMP085::read(unsigned char address)
{
  unsigned char data;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, (uint8_t) 1);
  while(!Wire.available())
    ;

  return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
uint16_t BMP085::readInt(unsigned char address)
{
  uint16_t result;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, (uint8_t) 2);
  while(Wire.available()<2)
    ;
  result = Wire.read();
  result <<= 8;
  result |= (uint8_t)Wire.read();

  return result;
}

// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
void BMP085::calibrate()
{
  ac1 = readInt(0xAA);
  ac2 = readInt(0xAC);
  ac3 = readInt(0xAE);
  ac4 = readInt(0xB0);
  ac5 = readInt(0xB2);
  ac6 = readInt(0xB4);
  b1 = readInt(0xB6);
  b2 = readInt(0xB8);
  mb = readInt(0xBA);
  mc = readInt(0xBC);
  md = readInt(0xBE);
}

// Calculate temperature given ut.
// Value returned will be in units of 0.1 deg C
int16_t BMP085::calc_temperature()
{
  long x1, x2;

  x1 = (((long)value_UT - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8)>>4);
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
int32_t BMP085::calc_pressure()
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

  b7 = ((unsigned long)(value_UP - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;

  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;

  return p;
}


// Read the uncompensated temperature value
void BMP085::requestTemperature()
{
  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();

  waitOnRequest = 5; // required wait in millisec
  status = MEASURE_TEMP;
}

// Read the uncompensated pressure value
void BMP085::requestPressure(uint8_t ss) {
	OSS = ss;

  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();

  // Wait for conversion, delay time dependent on OSS
  waitOnRequest = (2 + (3<<OSS));
  status = MEASURE_PRESS;
}

int16_t BMP085::readTemperature() {
  // Wait at least 4.5ms
//  delay(5);

  // Read two bytes from registers 0xF6 and 0xF7
  value_UT = readInt(0xF6);
  temperature = calc_temperature();
  status = IDLE;
  return temperature;
}

int32_t BMP085::readPressure() {
  unsigned char msb, lsb, xlsb;
  //unsigned long up = 0;
  value_UP = 0;

  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, (uint8_t) 3);

  // Wait for data to become available
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();

  value_UP = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);

  pressure = calc_pressure();
  status = IDLE;
  return pressure;
}

void BMP085::getPressure() {
	requestPressure();
	delay(waitOnRequest);
	readPressure();
}

void BMP085::getTemperature() {
	requestTemperature();
	delay(waitOnRequest);
	readTemperature();
}
