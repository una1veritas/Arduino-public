//============================================================================
// Name        : avr-test.cpp
// Author      : Sin
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif

#include <stdio.h>

int main() {
	printf("!!!Hello World!!!\r\n");
	return 0;
}
