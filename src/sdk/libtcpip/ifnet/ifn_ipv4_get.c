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
 * @file ifn_ipv4_get.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <stdlib.h>

int ifn_ipv4_get(struct ifnet * __if, in_addr_t * __addr, in_addr_t * __mask)
{
	if (__if == NULL)
		return -1;

	tcpip_net_lock();

	if (__addr != NULL)
		*__addr = __if->if_ipv4_addr;

	if (__mask != NULL)
		*__mask = __if->if_ipv4_mask;

	tcpip_net_unlock();

	return 0;
}

