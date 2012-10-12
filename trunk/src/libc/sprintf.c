/* $Id: sprintf.c,v 1.2 2008/04/17 18:24:21 bob Exp $ 
 *
 * File:	sprintf.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2006-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <limits.h>
#include <stdio.h>

int sprintf(char * str, const char * fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(str, UINT_MAX, fmt, ap);
	va_end(ap);

	return n;
}

