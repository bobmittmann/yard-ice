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
 * @file get_ifn_byipaddr.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <stdlib.h>

struct ifnet * get_ifn_byipaddr(in_addr_t __addr)
{
	struct ifnet * ifn = NULL;
	int i;

	tcpip_net_lock();

	for (i = 0; i < ifnet_max; i++) {
		/* naive lookup method to avoid division */
		if (__ifnet__.ifn[i].if_id != 0) {
			ifn = &__ifnet__.ifn[i];
			if (ifn->if_ipv4_addr == __addr)
				break;
		}
	}

	tcpip_net_unlock();

	return ifn;
}

