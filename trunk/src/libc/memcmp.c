/* $Id: memcmp.c,v 1.1 2008/03/29 11:24:05 bob Exp $ 
 *
 * File:	memcmp.c
 * Module:	libc
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <string.h>

int memcmp(const void * s1, const void * s2, size_t n)
{
	register unsigned char *cp1 = (unsigned char *) s1;
	register unsigned char *cp2 = (unsigned char *) s2;

	while (n--) {
		if (*cp1 != *cp2)
			return *cp1 - *cp2;
		cp1++;
		cp2++;
	}

	return 0;
}
