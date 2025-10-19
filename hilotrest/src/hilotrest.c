/*
 ============================================================================
 Name        : hilotrest.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	uint32_t qword = 0;
	uint16_t word = 0;

	((uint8_t * ) &word)[0] = 6;
	((uint8_t *) &word)[1] = 0xea;
	((uint8_t *) &qword)[1] = 0x11;
	((uint8_t *) &qword)[2] = 'C';

	printf("%x\n", word);
	printf("%x\n", qword);

	return EXIT_SUCCESS;
}
