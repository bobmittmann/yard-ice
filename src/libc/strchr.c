/*
 * File:	strchr.c
 * Module:	ulibc
 * Project:	
 * Author:	Robinson Mittmann (bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2008-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <string.h>
#include <stdlib.h>

char * strchr(const char * s, int c)
{
	register char *cp = (char *)s;

	while (*cp) {
		if (*cp == c)
			return cp;
	  	cp++;
	}
  
	return NULL;
}

