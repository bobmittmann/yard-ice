/* $Id: strtoul.c,v 2.0 2006/03/28 15:10:26 bob Exp $ 
 *
 * File:	strtok.c
 * Module:	ulibc
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <string.h>

char * strtok(char * str, const char * delim)
{
	static char * rem;
	char * tok;
	char * dp;
	int d;
	int c;

	if ((str == NULL) && ((str = rem) == NULL)) {
		return NULL;
	}

	/* Skip leading delimiters */
	c = *str;
	dp = (char *)delim;
	while ((d = *dp) != '\0') {
		if (c == d) {
			c = *++str;
			dp = (char *)delim;
		} else {
			dp++;
		}
	}

	if (c == '\0') {
		rem = NULL;
		return rem;
	}

	tok = str;

	for (;;) {
		c = *str;
		dp = (char *)delim;
		for (;;) {	
			if ((d = *dp) == c) {
				if (c == '\0') {
					rem = NULL;
				} else {
					rem = str + 1;
					*str = '\0';
				}
				return tok;
			}

			if (d == '\0')
				break;
			dp++;
		}
		str++;
	}
}

