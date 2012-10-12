/* $Id: strlen.c,v 2.0 2006/03/28 15:10:26 bob Exp $ 
 *
 * File:	strlen.c
 * Module:	libc
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <string.h>

size_t strlen(const char * s)
{
	register char *cp = (char *)s;
	int n = 0;

	if (!cp)
		return 0;

	while (*cp++)
		n++;

	return n;
}

