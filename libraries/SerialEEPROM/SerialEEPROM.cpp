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
	delayMicroseconds(5);
	digitalWrite(SDA,LOW);
	delayMicroseconds(5);
	digitalWrite(SCL,LOW);
	delayMicroseconds(5);
	digitalWrite(SDA, HIGH);
	delayMicroseconds(5);
	for(int i = 0; i < 9; ++i) {
		digitalWrite(SCL,HIGH);
		delayMicroseconds(10);
		digitalWrite(SCL,LOW);
		delayMicroseconds(10);
	}
	digitalWrite(SCL,HIGH);
	delayMicroseconds(5);
	digitalWrite(SDA,LOW);
	delayMicroseconds(5);
	digitalWrite(SCL,LOW);
	delayMicroseconds(10);

	digitalWrite(SCL,HIGH);
	delayMicroseconds(5);
	digitalWrite(SDA,HIGH);
	delayMicroseconds(5);
	digitalWrite(SCL,LOW);

	delayMicroseconds(10);
	digitalWrite(SCL,HIGH);

	pinMode(SDA, INPUT);
	pinMode(SCL, INPUT);

	wire_enable();
	_txrx_stat = 0;
}

EEPROM_I2C::EEPROM_I2C(const unsigned char addr, const unsigned char buswidth) {
	_devaddr = DEV_BASE_ADDRESS;
	_buswidth = buswidth;
	_txrx_stat = 0;
	if ( _buswidth == BUSWIDTH_1024KBIT ) {
		_devaddr |= ((addr & 0x03)<<1);
	} else if ( _buswidth == BUSWIDTH_512KBIT ) {
		_devaddr |= (addr & 0x07);
	}
}

uint8_t EEPROM_I2C::dev_addr(uint32_t addr) {
	if ( _buswidth == BUSWIDTH_1024KBIT ) {
		return (_devaddr | (addr>>16 & 1));
	}
	return _devaddr;
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

uint8_t EEPROM_I2C::write(uint32_t addr, uint8_t data) {
    Wire.beginTransmission( dev_addr(addr) );
    Wire.write( (byte) (addr >> 8) );
    Wire.write( (byte) addr );
    Wire.write(data);
    _txrx_stat = Wire.endTransmission();
    if (_txrx_stat != 0) return 0;

    for (uint8_t i=10; i; --i) {
        delay(1);                    //no point in waiting too fast
        if ( ready() )
        	break;
    }
    if (_txrx_stat != 0) return 0;
    return data;
}

uint8_t EEPROM_I2C::update(uint32_t addr, uint8_t data) {
	uint8_t readout = read(addr);
	if ( status() )
		return 0;
	if ( readout != data )
		return write(addr, data);
	return data;
}

byte * EEPROM_I2C::read(uint32_t addr, byte * dataptr, uint16_t nbytes) {
	uint16_t ntrans;

	for(uint16_t n = 0; n < nbytes; n += ntrans) {
		ntrans = (nbytes - n) > BUFFER_LENGTH ? BUFFER_LENGTH : (nbytes - n);
	    Wire.beginTransmission( dev_addr(addr) );
	    Wire.write( (byte) (addr >> 8) );
	    Wire.write( (byte) addr );
	    _txrx_stat = Wire.endTransmission();
	    if (_txrx_stat != 0)
	    	return NULL;        //read error
	    Wire.requestFrom( dev_addr(addr), (uint8_t) ntrans);
		if (_txrx_stat != 0) return NULL;
	    for(unsigned int i = 0; i < ntrans; ++i)
	    	dataptr[n+i] = Wire.read();

	    addr += ntrans;
	}
	return dataptr;
}

byte * EEPROM_I2C::write(uint32_t addr, byte * dataptr, uint16_t nbytes) {
	unsigned char ntrans;
	for(unsigned int n = 0; n < nbytes; n += ntrans) {
		ntrans = (nbytes - n) > BUFFER_LENGTH ? BUFFER_LENGTH : (nbytes - n);
		if ( (addr & 0xff00) != ((addr+ntrans) & 0xff00) ) {
			ntrans -= ((addr + ntrans) & 0x00ff);
		}
		Wire.beginTransmission( dev_addr(addr) );
		Wire.write( (byte) (addr >> 8) );
		Wire.write( (byte) addr );
		Wire.write( dataptr+n, ntrans);
		_txrx_stat = Wire.endTransmission();
		for (uint8_t i=10; i; --i) {
			delay(1);                    //no point in waiting too fast
			if ( ready() )
				break;
		}
		if (_txrx_stat != 0) return NULL;

		addr += ntrans;
	}
	return dataptr;
}
byte * EEPROM_I2C::update(uint32_t addr, byte * dataptr, uint16_t nbytes) {
	for(byte i = 0; i < nbytes; ++i) {
		update(addr+i,dataptr[i]);
		if ( status() )
			return NULL;
	}
	return dataptr;
}
