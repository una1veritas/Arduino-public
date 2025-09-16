
#include <Arduino.h>

#include "SRAMBuffer.h"

SRAMBuffer::SRAMBuffer(byte * a, const size_t n) :
		length(n), _writept(0), _readpt(0) {
	array = a;
}

void SRAMBuffer::init() {
	_writept = 0;
	_readpt = 0;
}

byte SRAMBuffer::read(const long & address) {
	byte data;
	_readpt = address;
	data = array[_readpt];
	_readpt++;
	return data;
}

void SRAMBuffer::read(const long & address, byte *buffer, const long & size) {
	_readpt = address;
	for (unsigned int i = 0; i < size; i++)
		*buffer++ = array[_readpt++];
}

void SRAMBuffer::write(const long & address, byte data) {
	_writept = address;
	array[_writept++] = data;
}

void SRAMBuffer::write(const long & address, byte *buffer, const long & size) {
	_writept = address;
	//
	for (unsigned int i = 0; i < size; i++)
		array[_writept++] = *buffer++;
}

int SRAMBuffer::available(void) {
	return _writept - _readpt;
}
