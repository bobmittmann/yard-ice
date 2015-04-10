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
 * @file etharp.c
 * @brief Ethernet ARP
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#define __USE_SYS_ARP__
#include <sys/arp.h>
#define __USE_SYS_ROUTE__
#include <sys/route.h>

#include <sys/etharp.h>

#include <netinet/in.h>
#include <string.h> 
#include <stdlib.h> 
#include <errno.h> 


int ipv4_arp_lookup(in_addr_t __ipaddr, struct ipv4_arp * __arp)
{
	struct route * rt;
	struct ifnet * ifn;
	int ret = -ENOENT;

	tcpip_net_lock();

	/* lookup for the interface of the local address */
	if (((rt = __route_lookup(__ipaddr)) != NULL) && 
		(rt->rt_gateway = INADDR_ANY)) {
		void * hwaddr;

		/* get interface */
		ifn = rt->rt_ifn;
		hwaddr = ifn_arplookup(ifn, __ipaddr);
		if (hwaddr != NULL) {
			/* FIXME: other than ethernet interface ... */
			__arp->type = ARPHRD_ETHER;
			__arp->flags = 0;
			__arp->ipaddr = __ipaddr;
			__arp->ifn = ifn;
			memcpy(__arp->hwaddr, hwaddr, ETH_ADDR_LEN);
			ret = 0;
		}
	}

	tcpip_net_unlock();

	return ret;
}

int ipv4_arp_query(in_addr_t __ipaddr)
{
	struct route * rt;
	struct ifnet * ifn;
	int ret = -ENOENT;

	tcpip_net_lock();

	DCC_LOG1(LOG_TRACE, "route lookup %I", __ipaddr);

	/* lookup for the interface of the local address */
	if (((rt = __route_lookup(__ipaddr)) != NULL) && 
		(rt->rt_gateway == INADDR_ANY)) {

		/* get interface */
		ifn = rt->rt_ifn;

		/* ARP request */
		ret = ifn_arpquery(ifn, __ipaddr);
	}

	tcpip_net_unlock();

	return ret;
}

int ipv4_arp_enum(int (* __callback)(struct ipv4_arp *, void *), void * __arg)
{
	int ret;

	tcpip_net_lock();

	ret = etharp_enum(__callback, __arg);

	tcpip_net_unlock();

	return ret;
}

