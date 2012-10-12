/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libtcpip.
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
 * @file stat.c
 * @brief collect protocol statistics
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_NET__
#include <sys/net.h>

#include <tcpip/stat.h>
#include <string.h>
#include <stdlib.h>

void proto_stat_copy(struct proto_stat * __dst, 
					 struct proto_stat * __src, int __rst)
{
	tcpip_net_lock();

	if (__dst != NULL) {
		memcpy(__dst, __src, sizeof(struct proto_stat)); 
	}

	if (__rst) {
		memset(__src, 0, sizeof(struct proto_stat)); 
	}

	tcpip_net_unlock();
}


