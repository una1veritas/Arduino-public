/*
 ============================================================================
 Name        : union_test.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

union HighLow {
	uint16_t highlow;
	struct {
		uint8_t low, high;
	};
	uint8_t bytes[2];
} addr;

int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	addr.highlow = 0x1f3b;
	printf("highlow = %x, high = %x, low = %x\n", addr.highlow, addr.high, addr.low);
	printf("bytes:\n");
	for(int i = 0; i < 2; ++i) {
		printf("%x, ", addr.bytes[i]);
	}
	printf("\n");
	return EXIT_SUCCESS;
}
