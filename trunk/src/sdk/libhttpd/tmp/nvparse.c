/* $Id: nvparse.c,v 2.1 2006/04/06 18:16:26 bob Exp $ 
 *
 * File:	nvparse.c
 * Module:  httpd
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment: Name Value Parser
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <tcpip/httpd.h>

#ifdef HTTPD_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

int httpd_nvparse(const char * s, char * name[], char * value[], int count)
{
	char * cp;
	int n;
	int m;
	int i;
	
	cp = (char *)s;
	n = 0;
	i = 0;
	for(;;) {
		name[i] = &cp[n];
		for (; (cp[n] != '='); n++) {
			if (cp[n] == '\0') 
				return i;
		}
		cp[n] = '\0';
		n++;
		value[i] = &cp[n];
		m = n;
		for (; (cp[n] != '&'); n++) {
			if (cp[n] == '\0')
				break;
			/* */
			if (cp[n] == '+')
				cp[m] = ' ';
			else {
				if (cp[n] == '%') {
					char c0;
					char c1;

					c0 = cp[n + 1];
					c1 = cp[n + 2];
					if (c0 == '\0' || c1 == '\0')
						break;
					cp[m] = ((c0 - '0') << 4) + c1 - '0';
					n += 2;
				} else {
					cp[m] = cp[n];
				}
			}
			m++;
		}
		cp[m] = '\0';
		n++;
		i++;
		if (i == count)
			break;
	}

	return i;
}

