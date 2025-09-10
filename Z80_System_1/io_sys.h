/*
 * IO_sys.h
 *
 *  Created on: 2025/08/20
 *      Author: sin
 */

#ifndef IO_SYS_H_
#define IO_SYS_H_

#ifndef uint8
typedef uint8_t uint8;
#endif

struct IO_SYS {
public:
	static void out(const uint8 & port, const uint8 & a) {
		switch(port) {
		case 2:  // putchar
			putchar(a);
			break;
		case 16:
			break;
		case 17:
			break;
		case 18: // sector
			break;
		case 20: // dma L
			break;
		case 21: // dma H
			break;
		case 22:
			break;
		}
	}

	void putchar(const uint & a) {
		Serial.print((char) a);
	}
};

#endif /* IO_SYS_H_ */
