/*
 * EEPROM_I2C.cpp
 *
 *  Created on: 2017/10/20
 *      Author: sin
 */

#include <Wire.h>
#include "SerialEEPROM.h"

byte EEPROM_I2C::ready(void) {
    Wire.beginTransmission( dev_addr(0) );
    Wire.write((byte)0);
    return Wire.endTransmission() == 0;
}

void EEPROM_I2C::wire_enable(void) {
    TWCR |= (_BV(TWEN) | _BV(TWIE) | _BV(TWEA));
    digitalWrite(SDA, HIGH);
    digitalWrite(SCL, HIGH);
}

void EEPROM_I2C::wire_disable(void) {
    TWCR &= ~(_BV(TWEN) | _BV(TWIE) | _BV(TWEA));
    digitalWrite(SDA, LOW);
    digitalWrite(SCL, LOW);
}

void EEPROM_I2C::reset(void) {
	wire_disable();

	pinMode(SDA, OUTPUT);
	pinMode(SCL, OUTPUT);

	digitalWrite(SCL, HIGH);
	digitalWrite(SDA, HIGH);
	delay(10);

	// start bit
	digitalWrite(SCL,HIGH);
	delayMicroseconds(10);
	digitalWrite(SDA,LOW);
	delayMicroseconds(10);
	digitalWrite(SCL,LOW);
	delayMicroseconds(10);
	digitalWrite(SDA, HIGH);
	delayMicroseconds(10);
	for(int i = 0; i < 9; ++i) {
		digitalWrite(SCL,HIGH);
		delayMicroseconds(20);
		digitalWrite(SCL,LOW);
		delayMicroseconds(20);
	}
	digitalWrite(SCL,HIGH);
	delayMicroseconds(10);
	digitalWrite(SDA,LOW);
	delayMicroseconds(10);
	digitalWrite(SCL,LOW);
	delayMicroseconds(20);

	digitalWrite(SCL,HIGH);
	delayMicroseconds(10);
	digitalWrite(SDA,HIGH);
	delayMicroseconds(10);
	digitalWrite(SCL,LOW);

	delayMicroseconds(20);
	digitalWrite(SCL,HIGH);

	pinMode(SDA, INPUT);
	pinMode(SCL, INPUT);

	wire_enable();
	_txrx_stat = 0;
}

uint8_t EEPROM_I2C::dev_addr(uint32_t addr) {
	uint8_t devaddr = DEV_BASE_ADDRESS;
	if ( _buswidth == BUSWIDTH_1024KBIT ) {
		devaddr |= (_addr & 0x06) | (addr>>16 & 0x01);
	}
	return DEV_BASE_ADDRESS;
}
uint8_t EEPROM_I2C::read(uint32_t addr) {
	uint8_t val = 0;
    Wire.beginTransmission( dev_addr(addr) );
    Wire.write( (byte) (addr >> 8) );
    Wire.write( (byte) addr );
    _txrx_stat = Wire.endTransmission();
    if (_txrx_stat != 0)
    	return val;        //read error

    Wire.requestFrom( dev_addr(addr), (uint8_t) 1);
    val = Wire.read();
    return val;
}

uint8_t EEPROM_I2C::raw_write(uint32_t addr, uint8_t data) {
    Wire.beginTransmission( dev_addr(addr) );
    Wire.write( (byte) (addr >> 8) );
    Wire.write( (byte) addr );
    Wire.write(data);
    _txrx_stat = Wire.endTransmission();
    if (_txrx_stat != 0) return data;

    //wait up to 50ms for the write to complete
    for (uint8_t i=100; i; --i) {
        delayMicroseconds(500);                    //no point in waiting too fast
        Wire.beginTransmission( dev_addr(addr) );
        Wire.write((byte)0);        //high addr byte
        Wire.write((byte)0);                              //low addr byte
        _txrx_stat = Wire.endTransmission();
        if (_txrx_stat == 0) break;
    }
    return data;
}

uint8_t EEPROM_I2C::write(uint32_t addr, uint8_t data) {
	uint8_t readout = read(addr);
	if ( status() )
		return 0;
	if ( readout != data )
		return raw_write(addr, data);
	return data;
}
