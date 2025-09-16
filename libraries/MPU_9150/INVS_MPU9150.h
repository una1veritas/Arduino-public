/*
 * INVS_MPU9150.h
 *
 *  Created on: 2014/06/03
 *      Author: sin
 */

#ifndef INVS_MPU9150_H_
#define INVS_MPU9150_H_

#include <Arduino.h>
#include <Wire.h>

class MPU9150 {
	const uint8_t I2C_MPU_ADDR; // selectable by AD0 pin.
	static const uint8_t I2C_AK8975_ADDR = 0x0C;
	uint8_t i2c_addr;

	void accessMPU() { i2c_addr = I2C_MPU_ADDR; }
	uint8_t accessAKM() { i2c_addr = I2C_AK8975_ADDR; }

	uint8_t readRegister(uint8_t);
	void readRegister(uint8_t, uint8_t *, uint8_t);
	void writeRegister(uint8_t, uint8_t);

public:
	uint8_t agraw[14];
	uint8_t magraw[7];
	uint8_t magadj[3];
	uint16_t accel_lsb_mg;
	uint16_t gyro_lsb_deg;

public:

	enum ACCEL_CONFIG {
		REGISTER_ACCEL_CONFIG = 0x1C,
		AFS_SEL_2g = 0<<3,
		AFS_SEL_4g = 1<<3,
		AFS_SEL_8g = 2<<3,
		AFS_SEL_16g = 3<<3,
		//
		ACCEL_HPF_Reset = 0,
		ACCEL_HPF_On_5Hz = 1,
		ACCEL_HPF_On_2Hz5 = 2,
		ACCEL_HPF_On_1Hz25 = 3,
		ACCEL_HPF_On_0Hz63 = 4,
		ACCEL_HPF_Hold = 7,
	};

	enum GYRO_CONFIG {
		REGISTER_GYRO_CONFIG = 0x1B,
		FS_SEL_250 = 0<<3,
		FS_SEL_500 = 1<<3,
		FS_SEL_1000 = 2<<3,
		FS_SEL_2000 = 3<<3,
	};

	enum AK8975_CONST {
		AKM_REG_WIA = 0x00,
		AKM_REG_ST1 = 0x02,
		AKM_REG_HXL = 0x03,
		AKM_REG_ST2 = 0x09,
		AKM_REG_CNTL = 0x0A,
		AKM_REG_ASAX = 0x10,

		AKM_WIA_ID = 0x48,
		AKM_ST1_DRDY = 1,
		AKM_ST2_DERR = 4,
		AKM_ST2_OVFR = 8,
		AKM_CNTL_POWERDOWN = 0,
		AKM_CNTL_SINGLEMEASUREMENT = 1,
	};

	MPU9150(const int ad0 = 1) : I2C_MPU_ADDR( 0x68 | (ad0 & 1) )  {
		i2c_addr = I2C_MPU_ADDR;
	}

	boolean begin(int axcfg, int axhpf, int gycfg);
	void wakeUp(void);

	void configAccel(const int range, const int hpf);
	void configGyro(const int range);
	//
	void getAccelGyro();
	boolean getCompass();
	void measureCompass();
	boolean compassDRDY();

	float accX() { return float((static_cast<int16_t>(agraw[0])<<8) + agraw[1])/accel_lsb_mg; }
	float accY() { return float((static_cast<int16_t>(agraw[2])<<8) + agraw[3])/accel_lsb_mg; }
	float accZ() { return float((static_cast<int16_t>(agraw[4])<<8) + agraw[5])/accel_lsb_mg; }
	float gyroX() { return float((static_cast<int16_t>(agraw[8])<<8) + agraw[9])/gyro_lsb_deg; }
	float gyroY() { return float((static_cast<int16_t>(agraw[10])<<8) + agraw[11])/gyro_lsb_deg; }
	float gyroZ() { return float((static_cast<int16_t>(agraw[12])<<8) + agraw[13])/gyro_lsb_deg; }

	// in mu Tesra
	float compassX() { return ((int16_t(magraw[3])<<8) | magraw[2])*0.3; }
	float compassY() { return ((int16_t(magraw[1])<<8) | magraw[0])*0.3; }
	float compassZ() { return -((int16_t(magraw[5])<<8) | magraw[4])*0.3; }
};

#endif /* INVS_MPU9150_H_ */
