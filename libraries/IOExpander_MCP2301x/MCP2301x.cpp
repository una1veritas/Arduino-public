/*
 */
#if ARDUINO >= 100
  #include "Arduino.h"
#else
   #include "WPrograms.h"
#endif
#include <Wire.h>

#include "MCP2301x.h"

#ifdef ARDUINO >= 100
	inline byte Wsend(byte v) { return Wire.write(v); }
	inline byte Wreceive() { return Wire.read(); }
#else
	inline byte Wsend(byte v) { return Wire.send(v); }
	inline byte Wreceive() { return Wire.receive(); }
#endif


MCP2301x::MCP2301x(const int addr) {
	address = FIXEDADDR | (addr & 0x07);
	cache = 0;
}

void MCP2301x::init() {
	// assume and reset to the Power-On/Reset state
	write(IOCON, 0x00);
	// BANK = 0, MIRROR = 0, SEQOP = 0, DISSLW = 0, (HAEN = 0,) ODR = 0, INTPOL = 0.
	write16(IODIRA, 0x0000); // set all to be output
	write16(IPOLA, 0x0000); // set all to be normal.
	write16(GPPUA, 0x0000); // set all pull-ups disabled.
}

uint8_t MCP2301x::send(const uint8_t reg, const uint8_t val) {
  Wire.beginTransmission(address);
  Wsend(reg); //Wire.send(reg);
  Wsend(val); //Wire.send(val);
  return commstate = Wire.endTransmission();
}

// returns received value, set result in this->comm_result.
uint8_t MCP2301x::receive(const uint8_t reg) {
  Wire.beginTransmission(address);
  Wsend(reg); //Wire.send(reg);
  commstate = Wire.endTransmission();

  commstate = Wire.requestFrom(address,(uint8_t) 1);
  while (Wire.available() < 1) ;
  return Wreceive(); //Wire.receive();
}

uint8_t MCP2301x::send(const uint8_t reg, const uint8_t array[], const size_t n) {
  Wire.beginTransmission(address);
  Wsend(reg); //Wire.send(reg);
  for(int i = 0; i < n; i++) {
	  Wsend(array[i]); //Wire.send(val);
  }
  return commstate = Wire.endTransmission();
}

// returns received value, set result in this->comm_result.
uint8_t MCP2301x::receive(const uint8_t reg, uint8_t array[], const size_t n) {
  Wire.beginTransmission(address);
  Wsend(reg); //Wire.send(reg);
  commstate = Wire.endTransmission();

  commstate = Wire.requestFrom(address,(uint8_t) n);
  while (Wire.available() < 1) ;
  for (int i = 0; i < n; i++) {
  	  array[i] = Wreceive(); //Wire.receive();
  }
  return commstate;
}

void MCP2301x::mode(const byte port, const byte io) {
	if ( port & 1 ) {
		write(IODIRB, (io == INPUT ? ALL_INPUT : ALL_OUTPUT));
	} else {
		write(IODIRA, (io == INPUT ? ALL_INPUT : ALL_OUTPUT));
	}
}
