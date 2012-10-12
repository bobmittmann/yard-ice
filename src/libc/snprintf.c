/* $Id: snprintf.c,v 2.2 2008/04/17 18:24:49 bob Exp $ 
 *
 * File:	sprintf.c
 * Module:	ulibc
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2008 BORESTE (www.boreste.com). All Rights Reserved.
 */

#include <stdio.h>

int snprintf(char * str, size_t size, const char *fmt, ...) 
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(str, size, fmt, ap);
	va_end(ap);

	return n;
}

