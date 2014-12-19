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
 * @file inet_ntop.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

const char * inet_ntop(int af, const void * cp, char * buf, size_t len)
{
	char * ip;
	
	if (af != AF_INET)
		return NULL;

	ip = (char *)cp;

	snprintf(buf, len, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

	return buf;
}
