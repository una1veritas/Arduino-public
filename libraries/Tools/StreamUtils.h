/*
  StreamUtils.h
*/

#ifndef _STREAMUTILS_H_
#define _STREAMUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdio.h> // for size_t
//#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

#include <Arduino.h>

const static char NL = '\n';
const static char CR = '\r';

template<class T>
inline Print & operator <<(Print &out, T arg) {
	out.print(arg);
	return out;
}

size_t printBytes(Print & prn, const uint8_t * p, const uint8_t length, const uint8_t base = 16, const char sep = ' ');
size_t printByte(Print & prn, const uint8_t val, const uint8_t base = 16);
inline size_t printBits(Print & prn, const uint8_t *p, const uint8_t length) {
	return printBytes(prn, p, length, 2, 0);
}

size_t readToken(Stream & stream, char buf[], const int maxlen, long timeout = 250);

#endif /* _STREAMUTILS_H_ */
