/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
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
 * @file inet_aton.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdlib.h>
#include <ctype.h>
#include <netinet/in.h>

int inet_aton(const char * name, struct in_addr * addr)
{
	unsigned char * ip;
	int a1;
	int a2;
	int a3;
	int a4;
	char * cp;

	for (cp = (char *)name; isspace(*cp); cp++);

	ip = (unsigned char *)addr;

	if (*cp == '*') {
		ip[0] = 0;
		ip[1] = 0;
		ip[2] = 0;
		ip[3] = 0;
		return 1;
	}

 	a1 = strtoul(cp, &cp, 0);
 	if ((*cp++ != '.' ) || (a1 > 255))
		return 0;
		
	a2 = strtoul(cp, &cp, 0);
	if ((*cp++ != '.' ) || (a2 > 255))
		return 0;
		
 	a3 = strtoul(cp, &cp, 0);
	if ((*cp++ != '.' ) || (a3 > 255))
		return 0;

	a4 = strtoul(cp, &cp, 0);

	for (;isspace(*cp); cp++);

	if ((*cp != '\0' ) || (a4 > 255))
		return 0;

#if __BYTE_ORDER == __LITTLE_ENDIAN
	ip[0] = a1;
	ip[1] = a2;
	ip[2] = a3;
	ip[3] = a4;
#elif __BYTE_ORDER == __BIG_ENDIAN
	ip[3] = a1;
	ip[2] = a2;
	ip[1] = a3;
	ip[0] = a4;
#else
# error "Please fix <bits/endian.h>"
#endif

	return 1;
}

