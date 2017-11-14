/*
 * OC1AClock.cpp
 *
 *  Created on: 2017/11/13
 *      Author: sin
 */

#include <AVR/io.h>
#include <avr/interrupt.h>
#include "OC1AClock.h"

static OCA1Clock OC1A;

void OC1AClock_setup(uint8_t presc, uint16_t top) {
	OC1A.prescaler = presc;
	OC1A.topvalue = top;
}

void OC1AClock_start(void) {
    const uint8_t MODE = 4;
    cli();

    TCCR1A = 0; TCCR1B = 0; TCCR1C = 0;

    TCNT1 = 0;
    OCR1A = OC1A.topvalue - 1;

    TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
    TCCR1B |= ((MODE >> 2 & 0x03) << 3) |  (OC1A.prescaler<<CS10);
    TCCR1C |= (1 << FOC1A);

    sei();
  }

void OC1AClock_stop() {
    TCCR1A = 0;
    TCCR1B &= ~(0x07<<CS10);
}

void OC1AClock_reset(uint8_t presc, uint16_t top) {
    OC1AClock_stop();
    OC1AClock_setup(presc, top);
    OC1AClock_start();
}

void OC1AClock_restart(void) {
    TCCR1B |= (OC1A.prescaler<<CS10);
}
