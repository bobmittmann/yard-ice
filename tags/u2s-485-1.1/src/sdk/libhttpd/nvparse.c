/*
 * Copyright(c) 2004-2014 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libhttpd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file nvparse.c
 * @brief HTTPD library
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "httpd-i.h"

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

