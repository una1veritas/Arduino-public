
#ifndef SRAMBUFFER_H
#define SRAMBUFFER_H

#include <Arduino.h>
#include <Stream.h>

class SRAMBuffer : public Stream {
private:
	byte * array;
	const size_t length;
	long _readpt, _writept;

public:

	SRAMBuffer(byte * a, const size_t n);

	void init();
	inline void begin() {
		init();
	}
	inline void setSPIMode();

	byte read(const long & address);
	void read(const long & address, byte *buffer, const long & size);
	void write(const long & address, byte data);
	void write(const long & address, byte *buffer, const long & size);
	const long & readpoint() { return _readpt; }
	const long & writepoint() { return _writept; }
	virtual size_t write(uint8_t val) { write(_writept, val); return 1; }
	virtual int read(void) { return read(_readpt); }
	virtual int peek(void) { uint8_t val = read(_readpt); _readpt--; return val; }
	virtual int available(void);
	virtual void flush(void) { _readpt = _writept; }
	void reset(long val = (long)0) { _readpt = val; _writept = _readpt; }

};

#endif
