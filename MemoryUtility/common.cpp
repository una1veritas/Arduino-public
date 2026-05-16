/*
 * utils.cpp
 *
 *  Created on: 2026/04/28
 *      Author: sin
 */

#include <Arduino.h>
//#include <stdarg.h>
#include "common.h"


uint8_t charToNibble(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }
  return 0;
}

uint8_t hexToUint8(String hex, int startpos = 0) {
	if (startpos + 2 > hex.length()) {
		return 0;
	}
	uint8_t val = 0;
	val <<= 4;
	val |= charToNibble(hex[startpos++]);
	val <<= 4;
	val |= charToNibble(hex[startpos]);
	return val;
}

int Serialsnprint(char buf[], unsigned int n, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buf, n, format, args);
    va_end(args);
    Serial.print(buf128);
    return result;
}

int Serialsnprintln(char buf[], unsigned int n, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buf, n, format, args);
    va_end(args);
    result += Serial.println(buf);
    return result;
}

/*
void verifyData() {
  Serial.println(F("\nVerifying loaded data..."));

  if (wstatus.totalBytesWritten == 0) {
    Serial.println(F("No data loaded."));
    return;
  }

  // Simple verification: read back a sample of written data
  // This is a basic check; a full CRC would be more robust
  uint32_t sampled = 0;
  uint32_t verified = 0;

  Serial.println(F("Sample verification complete."));
  Serial.print(F("Total bytes to verify: "));
  Serial.println(wstatus.totalBytesWritten);
}
*/
