/* $Id: strcmp.c,v 2.2 2006/09/05 02:18:12 bob Exp $ 
 *
 * File:	strcmp.c
 * Module:	libc
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <string.h>

int strcmp(const char * s1, const char * s2)
{
	int c1;
	int c2;

	if (s1 == '\0')
		return 0;

	if (s2 == '\0')
		return 0;

	do {
		c1 = *s1++;
		c2 = *s2++;

		if ((c2 = c1 - c2) != 0)
			return c2;

	} while (c1 != '\0');

	return 0;
}

