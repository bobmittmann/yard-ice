/* 
 * File:	printf.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stddef.h>
#include <stdio.h>

int printf(const char * fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vfprintf(stdout, fmt, ap);
	va_end(ap);

	return n;
}

