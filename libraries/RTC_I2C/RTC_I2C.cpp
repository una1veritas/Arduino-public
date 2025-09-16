#include "RTC_I2C.h"

bool RTC_I2C::readRegisters(const byte addr, byte * data, const byte num) {
	Wire.beginTransmission(DEV_ADDRESS);
	Wire.write(addr);
	Wire.endTransmission();

	// request num bytes of data
	if ( Wire.requestFrom(DEV_ADDRESS, num) != num )
		return false;
	for (int i = 0; i < num; i++) {
		// store data in raw bcd format
		*data++ = Wire.read(); //Wire.receive();
	}
	return true;
}

bool RTC_I2C::writeRegisters(const byte addr, byte *data, const byte num) {
	Wire.beginTransmission(DEV_ADDRESS);
	Wire.write(addr); // reset register pointer
	for (int i = 0; i < num; i++) {
		Wire.write(*data++);
	}
	if ( Wire.endTransmission() == 0 )
		return true;
	return false;
}
