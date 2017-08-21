/*
 * gpio.h
 *
 *  Created on: 2013/11/23
 *      Author: sin
 */

#ifndef GPIO_H_
#define GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifdef __AVR__
//#include <avr/io.h>

#define IOPORT(addr)  (*((volatile uint8_t *)(addr)))
#define DDRxOFFSET 1
#define PORTxOFFSET 2
#define INPORT(base)  (IOPORT(base))
#define DIRREG(base)  (IOPORT((base)+DDRxOFFSET))
#define OUTPORT(base)  (IOPORT((base)+PORTxOFFSET))

#define ATMEGA1284P

typedef uint16_t GPIOPin;
enum GPIOPin_Def {
//	NOT_A_PIN = 0,

	GPIOD0  = 0x2900 | (1<<0), // PIND, DDRD, PORTD
	GPIOD1  = 0x2900 | (1<<1),
	GPIOD2  = 0x2900 | (1<<2),
	GPIOD3  = 0x2900 | (1<<3),
	GPIOD4  = 0x2900 | (1<<4),
	GPIOD5  = 0x2900 | (1<<5),
	GPIOD6  = 0x2900 | (1<<6),
	GPIOD7  = 0x2900 | (1<<7),
	GPIOB0 = 0x2300 | (1<<0),
	GPIOB1 = 0x2300 | (1<<1),
	GPIOB2 = 0x2300 | (1<<2), // d8
	GPIOB3 = 0x2300 | (1<<3), // d9
	GPIOB4 = 0x2300 | (1<<4), // d10
	GPIOB5 = 0x2300 | (1<<5), // d11
	GPIOB6 = 0x2300 | (1<<6), // d12
	GPIOB7 = 0x2300 | (1<<7), // d13
#ifdef ATMEGA1284P
	D10 = GPIOB4,
	D11 = GPIOB5,
	D12 = GPIOB6,
	D13 = GPIOB7,
#endif
	PIN_NOT_DEFINED = (uint8_t) 0xffff,
};

#define pinmode(p,m) 	( m ? DIRREG((p)>>8 &0xff) |= (p)&0xff : DIRREG((p)>>8 &0xff) &= ~((p)&0xff) )
#define pinhigh(p)  	{ OUTPORT((p)>>8 &0xff) |= (p)&0xff; }
#define pinlow(p)  		{ OUTPORT((p)>>8 &0xff) &= ~((p)&0xff); }
#define pinwrite(p,v)  	( v ? OUTPORT((p)>>8 &0xff) |= (p)&0xff : OUTPORT((p)>>8 &0xff) &= ~((p)&0xff) )
#define pinread(p)   	( (INPORT((p)>>8 & 0xff) & ((p)&0xff)) != 0 )
#define pintoggle(p) 	{ OUTPORT((p)>>8&0xff) ^= ((p)&0xff); }

#endif

#ifdef __cplusplus
}
#endif


#endif /* GPIO_H_ */
