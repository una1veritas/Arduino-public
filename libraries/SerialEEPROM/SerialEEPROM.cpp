/*
 * EEPROM_I2C.cpp
 *
 *  Created on: 2017/10/20
 *      Author: sin
 */

#include <Wire.h>
#include "SerialEEPROM.h"

uint8_t EEPROM_I2C::read(uint16_t addr) {
	uint8_t val = 0;
    Wire.beginTransmission( (DEV_ADDRESS | (_addr<<1)) );
    Wire.write( (byte) (addr >> 8) );
    Wire.write( (byte) addr );
    _rx_stat = Wire.endTransmission();
    if (_rx_stat != 0) return _rx_stat;        //read error

    Wire.requestFrom( (DEV_ADDRESS | (_addr<<1)), 1);
    val = Wire.read();
    return val;
}

uint8_t EEPROM_I2C::write(uint16_t addr, uint8_t data) {
    Wire.beginTransmission( (DEV_ADDRESS | (_addr<<1)) );
    Wire.write( (byte) (addr >> 8) );
    Wire.write( (byte) addr );
    Wire.write(data);
    _tx_stat = Wire.endTransmission();
    if (_tx_stat != 0) return _tx_stat;

    //wait up to 50ms for the write to complete
    for (uint8_t i=100; i; --i) {
        delayMicroseconds(500);                    //no point in waiting too fast
        Wire.beginTransmission( (DEV_ADDRESS | (_addr<<1)) );
        Wire.write((byte)0);        //high addr byte
        Wire.write((byte)0);                              //low addr byte
        _tx_stat = Wire.endTransmission();
        if (_tx_stat == 0) break;
    }
    return data;
}
