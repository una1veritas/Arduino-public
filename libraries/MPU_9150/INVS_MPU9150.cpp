/*
 * INVS_MPU9150.cpp
 *
 *  Created on: 2014/06/03
 *      Author: sin
 */

#include <INVS_MPU9150.h>

//	boolean begin(int axcfg, int axhpf, int gycfg);
boolean MPU9150::begin(int axrange, int axhpf, int gyrange) {
	accessMPU();
	int c;
	Serial.println("I2C MPU address selected. Now reading register.");
	if ( (c = readRegister(0x75)) == 0x68 ) {
		Serial.println("register 0x75 matched 0x68.");
		// wake up AX, GY
		wakeUp();
		Serial.println("waken up.");
		accessAKM();
		Serial.println("AKM accessed.");
		if ( readRegister(0x00) != AKM_WIA_ID )
			return false;
		Serial.println("AKM_WIA_ID matched.");
		readRegister(AKM_REG_ASAX, magadj, 3);
		configAccel(axrange,axhpf);
		configGyro(gyrange);
		Serial.println("All done.");
		return true;
	}
	Serial.print("read register 0x75 failed: ");
	Serial.println(c, HEX);
	return false;
}

void MPU9150::wakeUp() {
	accessMPU();
	writeRegister(0x6B, 0x00);
	writeRegister(0x37, 0x02);
}

uint8_t MPU9150::readRegister(uint8_t subAddress) {
	uint8_t data;

	Wire.beginTransmission(i2c_addr);
	Wire.write(subAddress);
	Wire.endTransmission(false);
	Wire.requestFrom(i2c_addr, (uint8_t) 1);
	while (Wire.available() < 1) // Wait until data becomes available
		;
	data = Wire.read(); // Read register data into `data` variable
	Wire.endTransmission(); // End I2C transmission

	return data; // Return data from register
}

void MPU9150::readRegister(uint8_t subAddress, uint8_t * dest, uint8_t count) {
	Wire.beginTransmission(i2c_addr);
	Wire.write(subAddress);
	Wire.endTransmission(false);
	Wire.requestFrom(i2c_addr, count);
	while (Wire.available() < count)
		;
	for (int i=0; i<count ;i++)
		dest[i] = Wire.read();
	Wire.endTransmission();
}

void MPU9150::writeRegister(uint8_t subAddress, uint8_t val) {
	Wire.beginTransmission(i2c_addr);
	Wire.write(subAddress);
	Wire.write(val);
	Wire.endTransmission(); // End I2C transmission
}

void MPU9150::configAccel(const int range, const int hpf) {
	switch(range) {
	case AFS_SEL_2g:
		accel_lsb_mg = 16384;
		break;
	case AFS_SEL_4g:
		accel_lsb_mg = 8192;
		break;
	case AFS_SEL_8g:
		accel_lsb_mg = 4096;
		break;
	case AFS_SEL_16g:
	default:
		accel_lsb_mg = 2048;
		break;
	}
	accessMPU();
	writeRegister(REGISTER_ACCEL_CONFIG, range | hpf);
}

void MPU9150::configGyro(const int range) {
	switch(range) {
	case FS_SEL_250:
		gyro_lsb_deg = 250;
		break;
	case FS_SEL_500:
		gyro_lsb_deg = 500;
		break;
	case FS_SEL_1000:
		gyro_lsb_deg = 1000;
		break;
	case FS_SEL_2000:
	default:
		gyro_lsb_deg = 2000;
		break;
	}
	accessMPU();
	writeRegister(REGISTER_GYRO_CONFIG, range);
}

void MPU9150::getAccelGyro(void) {
	accessMPU();
	readRegister(0x3b, agraw, 14);
	return;
}

void MPU9150::measureCompass(void) {
	accessAKM();
	writeRegister(AKM_REG_CNTL, AKM_CNTL_SINGLEMEASUREMENT);
//	memset(magraw, 0, 6);
	return;
}

boolean MPU9150::getCompass(void) {
	accessAKM();
	readRegister(AKM_REG_HXL, magraw, 7);
	if ( magraw[6] != 0 )
		return false;
	return true;
}

boolean MPU9150::compassDRDY() {
	accessAKM();
	return readRegister(AKM_REG_ST1) == AKM_ST1_DRDY;
}
