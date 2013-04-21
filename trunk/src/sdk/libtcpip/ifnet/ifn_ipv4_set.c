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
 * @file ifn_ipv4_set.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <stdlib.h>

int ifn_ipv4_set(struct ifnet * __if, in_addr_t __addr, in_addr_t __mask)
{
	if (__if == NULL)
		return -1;

	DCC_LOG(LOG_TRACE, "tcpip_net_lock().");
	DCC_LOG(LOG_TRACE, "1.");
	DCC_LOG(LOG_TRACE, "2.");
	DCC_LOG(LOG_TRACE, "3.");
	DCC_LOG(LOG_TRACE, "4.");
	DCC_LOG(LOG_TRACE, "5.");
	DCC_LOG(LOG_TRACE, "6.");
	tcpip_net_lock();

	/* 
	 * TODO: remove all routes bounded to this interface 
	 */
	if (__if->if_ipv4_addr)
		route_del(__if->if_ipv4_addr & __if->if_ipv4_mask);

	__if->if_ipv4_addr = __addr;
	__if->if_ipv4_mask = __mask;

	if (__if->if_ipv4_addr) {
		route_add(__if->if_ipv4_addr & __if->if_ipv4_mask, 
		__if->if_ipv4_mask, 0, __if);
	}

	DCC_LOG4(LOG_TRACE, "%s%d - addr=%I mask=%I", 
		ifn_type_name[__if->if_id >> 4], __if->if_id & 0x0f,
		__if->if_ipv4_addr, __if->if_ipv4_mask);

	DCC_LOG(LOG_TRACE, "tcpip_net_unlock().");
	tcpip_net_unlock();

	DCC_LOG(LOG_TRACE, "done.");

	return 0;
}

