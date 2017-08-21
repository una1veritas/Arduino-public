/*
 * RingBuffer.h
 *
 *  Created on: 2014/04/01
 *      Author: sin
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <Arduino.h>
#include <Stream.h>
#include <Printable.h>

class RingBuffer : public Stream, public Printable {
private:
	char * _buffer;
	size_t _capa;
	size_t _head, _tail;

public:
	RingBuffer(char * buf, const size_t sz) :
		_buffer(buf), _capa(sz) {
		_buffer[0] = 0;
		_head = 0;
		_tail = 0;
		// protected
		setTimeout(0);
	}

	virtual int available() { return (_tail != _head); }
	virtual int read() {
		if ( _head != _capa ) return _buffer[_tail++];
		return -1;
	}
	virtual int peek() { return _buffer[_tail]; }
	virtual void flush() { _head = _tail; }

	int length(void) { return (_tail + _capa - _head) % _capa; }

    virtual size_t write(uint8_t);
    virtual size_t printTo(Print& p) const;

    char operator[](const size_t i) const {
    	return _buffer[i % _capa];
    }

    size_t copyTo(char * ptr);
    size_t getLine(Stream & src);
};


#endif /* RINGBUFFER_H_ */
