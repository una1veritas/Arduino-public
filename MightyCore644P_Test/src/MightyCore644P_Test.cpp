//============================================================================
// Name        : MightyCore644P_Test.cpp
// Author      : Sin
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // 1. Set Pin 5 of PORTB as OUTPUT
    DDRB |= (1 << PB5);

    while(1) {
        // 2. Set Pin 5 HIGH
        PORTB |= (1 << PB5);
        _delay_ms(1000);

        // 3. Set Pin 5 LOW
        PORTB &= ~(1 << PB5);
        _delay_ms(1000);
    }
	return 0;
}
