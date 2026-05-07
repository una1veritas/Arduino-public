/*
 * uni_programmer.cpp
 *
 *  Created on: 2026/05/07
 *      Author: sin
 */


#include "uni_programmer.h"

void IOEX_EEPROM::init() {
	pinMode(cs_pin, OUTPUT);
	digitalWrite(cs_pin, HIGH);
	if ( cs2_pin != 0xff ) {
		pinMode(cs2_pin, OUTPUT);
		digitalWrite(cs2_pin, HIGH);
	}
	pinMode(we_pin, OUTPUT);
	digitalWrite(we_pin, HIGH);
	pinMode(oe_pin, OUTPUT);
	digitalWrite(oe_pin, HIGH);
	addrbus.begin();
	databus.begin();
}

bool IOEX_EEPROM::write_byte(const uint32_t & address, const uint8_t value) {
	bool status = false;

	output_disable();
	write_disable();

	write_addressbus(address);
	set_databus_output();
	write_databus(value);

	chip_select();
	delayMicroseconds(1); // 	wait_62ns();
	write_enable();
	delayMicroseconds(1);
	write_disable();

	status = wait_for_write_cycle_end(value);

	chip_deselect();

	return status;
}

bool IOEX_EEPROM::wait_for_write_cycle_end(byte lastValue) {
    // if (mSupportsDataPoll)
	// Verify programming complete by reading the last value back until it matches the
	// value written twice in a row.  The D7 bit will read the inverse of last written
	// data and the D6 bit will toggle on each read while in programming mode.
	//
	// This loop code takes about 18uSec to execute.  The max readcount is set to the
	// device's maxReadTime (in uSecs) divided by ten rather than eighteen to ensure
	// that it runs at least as long as the chip's timeout value, even if some code
	// optimizations are made later. In actual practice, the loop will terminate much
	// earlier because it will detect the end of the write well before the max time.
	const unsigned int mMaxWriteTime = 15; // ms
	byte b1=0, b2=0;
	set_databus_input();
	delayMicroseconds(1);
	for (unsigned int readCount = 1; (readCount < (mMaxWriteTime * 100)); readCount++)
	{
		chip_select();
		output_enable();
		delayMicroseconds(1);
		b1 = read_databus();
		output_disable();
		chip_deselect();

		chip_select();
		output_enable();
		delayMicroseconds(1);
		b2 = read_databus();
		output_disable();
		chip_deselect();
		if ((b1 == b2) && (b1 == lastValue))
		{
			return true;
		}
	}

	//debugLastExpected = lastValue;
	//debugLastReadback = b2;
	return false;
}
