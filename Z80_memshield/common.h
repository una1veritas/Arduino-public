#ifndef _COMMON_H_
#define _COMMON_H_

#include <inttypes.h>

#define DDR(port)  (*((&port)-1))
#define PIN(port)  (*((&port)-2))

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef volatile BYTE * PORT;

#endif /* _COMMON_H_ */
