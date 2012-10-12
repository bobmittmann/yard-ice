/* $Id: fprintf.c,v 1.1 2007/09/26 02:01:22 bob Exp $ 
 *
 * File:	fprintf.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2006-2007 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdio.h>

int fprintf(FILE * f, const char * fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vfprintf(f, fmt, ap);
	va_end(ap);

	return n;
}

