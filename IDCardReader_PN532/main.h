#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <Arduino.h>

//#define DEBUG_FELICA

#define CR F("\r")
#define NL F("\r\n")

#define PN532_IRQ  2
#define PN532_RST  PIN_NOT_DEFINED
// Not connected by default on the NFC Shield
// tied with CPU RESET

#define BUZZ  5
#define LED_RED 5
#define LED_GREEN 6

#define CS_SD 		4
#define CS_SPIFRAM 	10


struct IDIizuka {
	char div;
	char occup;
	char ID[8];
	char issue;
};


struct IDLog {
	uint32_t date;
	uint32_t time;
	IDIizuka id;
	ISO14443Card card;
	uint8_t xsum;
};

// util funcs
/*
class Hex : public Printable {
	uint8_t num;
	char * ptr;

public:
	Hex(uint8_t b) {
		num = b;
		ptr = NULL;
	}

	Hex(char * a, uint8_t n) {
		num = n;
		ptr = a;
	}

	virtual size_t printTo(Print & prn) const {
		if ( ptr == NULL ) {
			prn.print(num>>4&0x0f, HEX);
			prn.print(num&0x0f, HEX);
			return 2;
		} else {
			for(int i = 0; i < num; i++) {
				prn.print((char)ptr[i]);
			}
			return num*3;
		}
	}
};
*/
//size_t printTimestampString(Print &, uint32_t, uint32_t);

#endif /* _MAIN_H_ */
