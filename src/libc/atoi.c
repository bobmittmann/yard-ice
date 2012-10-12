/* 
 * File:	atoi.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2007 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdlib.h>

int atoi(const char * s)
{
	int c;
	int n = 0;

	for (;;) {
		c = *s++;

		if ((c < '0') || (c > '9'))
			return n;

/*		n = (n * 10) + (c - '0'); */ 
		n = (((n << 2) + n) << 1) + (c - '0');
	};
}

