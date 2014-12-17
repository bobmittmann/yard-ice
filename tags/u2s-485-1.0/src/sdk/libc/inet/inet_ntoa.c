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
 * @file inet_ntoa.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifdef __CONFIG__
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <tcpip/in.h>

char * inet_ntoa(struct in_addr addr)
{
	static char buf[15];
	
	sprintf(buf, "%d.%d.%d.%d", IP4_ADDR1(addr.s_addr),
		IP4_ADDR2(addr.s_addr), IP4_ADDR3(addr.s_addr), 
		IP4_ADDR4(addr.s_addr));	

	return buf;
}
