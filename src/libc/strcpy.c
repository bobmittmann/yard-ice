/* $Id: strcpy.c,v 1.1 2007/09/26 02:01:22 bob Exp $ 
 *
 * File:	strcpy.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <string.h>

char * strcpy(char * dst, const char * src)
{
	register char *cp = dst;

	if ((!cp) || (!src))
		return NULL;

	while ((*cp++ = *src++));

	return dst;
}
