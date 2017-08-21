/*
 * main.c
 *
 *  Created on: 2017/07/18
 *      Author: sin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

int main(int argc, char * argv[]) {
	long lval;
	int ival;

	strncpy((char*)(&lval), "Prey", 8);
	((char*)&lval)[7] = 0;

	printf("%ld\n", sizeof(lval));
	printf("str: %s\n", (char*)&lval);
	printf("long: %lx\n", lval);

	return EXIT_SUCCESS;
}


