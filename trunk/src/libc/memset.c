/* $Id: memset.c,v 1.2 2008/02/21 12:15:26 bob Exp $ 
 *
 * File:	memset.c
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2007 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdint.h>
#include <string.h>

void * memset(void * s, int c, size_t n)
{
	register uint8_t * cp = (uint8_t *)s;

	while (n) {
		*cp++ = c;
		n--;
	}

	return s;
}

