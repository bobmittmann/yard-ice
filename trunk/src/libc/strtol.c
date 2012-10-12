/* 
 * File:	strtol.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:	
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */
 
#include <stdlib.h>

long int strtol(const char * __s, char ** __endp, int __base)
{
	char c;
	char * cp = (char *)__s;
	int val = 0;
	int inv = 0;

	for (; ((c = *cp) == ' '); cp++);

	if (c == '-') {
		inv = 1;
		cp++;
	} 

	val = strtoul(cp, __endp, __base);

	if (inv)
		val = -val;
	
	return val;
}

