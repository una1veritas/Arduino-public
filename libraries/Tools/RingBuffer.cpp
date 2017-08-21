/*
 * RingBuffer.cpp
 *
 *  Created on: 2014/04/01
 *      Author: sin
 */

#include "RingBuffer.h"

size_t RingBuffer::write(uint8_t c) {

	if ( ((_tail + 1) % _capa) == _head )
		return 0; // error
	_buffer[_tail++] = c;
	_tail %= _capa;

	return 1;
}

size_t RingBuffer::printTo(Print& p) const {
	size_t tmphead = _head;
	size_t cnt = 0;
	while ( tmphead != _tail ) {
		p.print(_buffer[tmphead++]);
		tmphead %= _capa;
		cnt++;
	}
	return cnt;
}

size_t RingBuffer::copyTo(char * ptr) {
	size_t tmphead = _head;
	size_t cnt = 0;
	while ( tmphead != _tail ) {
		*ptr++ = _buffer[tmphead++];
		tmphead %= _capa;
		cnt++;
	}
	return cnt;
}

size_t RingBuffer::getLine(Stream & src) {
	char c;
	size_t cnt = 0;

	while ( src.readBytes(&c, 1)) {
		if ( c == '\n' || c == '\r' ) {
			return cnt;
		}
		write(c);
		cnt++;
	}
	return cnt;
}

