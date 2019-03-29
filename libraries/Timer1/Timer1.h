/*
 * Timer.h
 *
 *  Created on: 2012/02/18
 *      Author: sin
 *
 *      Provides Access structure for Timer/Counter 0,1,2
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include "Arduino.h"
#include <stdint.h>

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class atmega328_Timer1 {
private:
	uint16_t tcnt1_start_val;
	uint16_t prescaler_val;

	// Clear control registers
	// COM1A1 COM1A0 COM1B1 COM1B0 – – WGM11 WGM10 | TCCR1A
	// ICNC1 ICES1 –--- WGM13 WGM12 CS12 CS11 CS10 | TCCR1B
	// –-- –-- ICIE1 –-- –-- OCIE1B OCIE1A TOIE1 | TIMSK1

	void init_control_registers() {
		TCCR1A = 0;
		TCCR1B = 0;
		TCCR1C = 0;

		tcnt1_start_val = 0;
		prescaler_val = 0;
	}

public:

	enum {
		CS_STOP = 0,
		CS_DIV1 = (1 << CS10),
		CS_DIV8 = (1 << CS11),
		CS_DIV64 = (1 << CS11) | (1 << CS10),
		CS_DIV256 = (1 << CS12),
		CS_DIV1024 = (1 << CS12) | (1 << CS10),
		CS_EXTT1_FALLING = (1 << CS12)| (1 << CS10),
		CS_EXTT1_RISING = (1 << CS12) | (1 << CS10) | (1 << CS10),

		CS_MASK = (1 << CS12) | (1 << CS10) | (1 << CS10),
	};

	atmega328_Timer1() {
		tcnt1_start_val = 0;
		prescaler_val = 0;
	}

	uint16_t count_start_val() {
		return tcnt1_start_val;
	}

	void start(unsigned int cs_val) {
		prescaler_val = cs_val;
		restart();
	}

	void restart() {
		TCCR1B |= (prescaler_val & CS_MASK);
	}

	void stop() {
		TCCR1B &= ~CS_MASK;
	}

	void mode_normal(unsigned int count_from) {
		init_control_registers();

		TIMSK1 &= ~((1<<ICIE1) | (1 << OCIE1A) | (1 << OCIE1B));
		TIMSK1 |= (1 << TOIE1);

		tcnt1_start_val = count_from;
		TCNT1 = tcnt1_start_val;
	}

	void mode_ctc(uint16_t limit_A, uint16_t limit_B = 0) {
		init_control_registers();
		TCCR1B |= (1 << WGM12);			// CTC mode

		OCR1A = limit_A;           // must not be zero
		OCR1B = limit_B;
		TIMSK1 &= ~((1 << ICIE1) | (1 << TOIE1)); // clear Overflow Interrupt
		TIMSK1 |= (limit_B ? (1 << OCIE1B) : 0) | (1 << OCIE1A);
	}
};

extern atmega328_Timer1 Timer1;

#endif /* TIMER1_H_ */
