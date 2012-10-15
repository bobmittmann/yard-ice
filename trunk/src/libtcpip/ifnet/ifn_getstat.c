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
 * @file ifn_getstat.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <tcpip/ifnet.h>

#include <string.h>

void ifn_getstat(struct ifnet * __if, struct ifnet_stat * __st, int __rst)
{
#if ENABLE_NETIF_STAT
	if (__if == NULL) {
		DCC_LOG(LOG_WARNING, "null pointer");
		return;
	}

	tcpip_net_lock();
	if (__st != NULL) {
		memcpy(__st, &__if->stat, sizeof(struct ifnet_stat)); 
		if (__rst) {
			memset(__st, 0, sizeof(struct ifnet_stat)); 
		}
	}
	tcpip_net_unlock();
#endif
}


