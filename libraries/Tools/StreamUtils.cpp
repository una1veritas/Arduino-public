/*
 * StreamUtils.cpp
 *
 *  Created on: 2014/04/10
 *      Author: sin
 */

#include "StreamUtils.h"

size_t printBytes(Print & prn, const uint8_t * p, const uint8_t length, const uint8_t base, const char sep) {
	size_t n = 0;
	int bit, val;
	int i = 0;
	while ( i < length ) {
		if ( base == 0 && isprint(*p) ) {
			n += prn.print((char) *p);
		} else if ( base == 2 ) {
			val = *p;
			for(bit = 7; bit >= 0; bit--) {
				if ( val & 0x80 )
					n += prn.print('1');
				else
					n += prn.print('0');
				val <<= 1;
			}
		} else {
			//if ( base == 16 )
			n += prn.print(*p>>4&0x0f, HEX);
			n += prn.print(*p&0x0f, HEX);
		}
		p++;
		i++;
		if ( i < length && sep )
			n += prn.print(sep);
	}
	return n;
}

size_t printByte(Print & prn, const uint8_t val, const uint8_t base) {
	return printBytes(prn, &val, 1, 16, 0);
}

size_t readToken(Stream & stream, char buf[], const int maxlen, long timeout) {
	long msec = millis();
	int bp = 0;
	byte c = 0;
	buf[bp] = 0;

	while ( bp < maxlen && millis() < timeout + msec ) {
		if ( !stream.available() )
			continue;
		c = stream.read();
		if (isspace(c)) {
			if (bp != 0)
				break;
			continue;
		} else {
			msec = millis();
		}
		buf[bp++] = c;
	}
	buf[bp] = 0;
	return bp;
}

