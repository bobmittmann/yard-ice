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
 * @file ip_route.c
 * @brief IP layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifdef IPROUTE_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#define __USE_SYS_ROUTE__
#include <sys/route.h>

#include <sys/net.h>

#include <sys/dcclog.h>

#ifndef IP_ROUTE_TABLE_LEN
#define IP_ROUTE_TABLE_LEN 4
#endif

struct route __route__[IP_ROUTE_TABLE_LEN];
const uint8_t ip_maxroute = IP_ROUTE_TABLE_LEN;

struct route * __route_lookup(in_addr_t __target)
{
	int i;
	struct route * p;

	p = __route__;
	i = IP_ROUTE_TABLE_LEN;
	do {
		if (p->rt_ifn == NULL) {
			DCC_LOG1(LOG_INFO, "route to %I not found", __target);
			return NULL;
		}
		if ((__target & p->rt_genmask) == (p->rt_dst)) {
			DCC_LOG3(LOG_INFO, "(0x%05x) %I -> %I", 
				(int)p, __target, __target);
			return p;
		}
		p++;
	} while (--i);

	DCC_LOG1(LOG_INFO, "route to %I not found", __target);

	return NULL;
}

int __route_del(in_addr_t __dst)
{
	int i;
	struct route * p;
	struct route * next;

	p = __route__;
	for (i = IP_ROUTE_TABLE_LEN; i; p++) {
		if (p->rt_ifn == NULL)
			return -1;
		i--;
		if (__dst == p->rt_dst)
			break;
	}

	if (i == 0)
		return -1;

	next = p;
	for (; i; i--) {
		next++;
		if (next->rt_ifn == NULL)
			break;
		*p = *next;
		p = next;
	}

	p->rt_dst = 0;
	p->rt_genmask = 0;
	p->rt_gateway = 0;
	p->rt_ifn = NULL;

	return 0;
}

int __route_add(in_addr_t __dst, in_addr_t __mask, 
	in_addr_t __gw, struct ifnet * __if)
{

	int i;
	int n;
	in_addr_t addr;
	struct route * p;
	struct route * new;
	void * tmp;

	p = __route__;
	/* remove a possibly previous entry */
	__route_del(__dst);

	/* locate the possition for the new entry by its netmask */
	p = __route__;
	for (i = IP_ROUTE_TABLE_LEN; i; i--, p++) {
		if (p->rt_genmask <= __mask) {
				break;
		}
	}

	if (i == 0) {
		DCC_LOG(LOG_WARNING, "no room to add a new route");
		return -1;
	}

	new = p;
	n = 0;
	while (p->rt_ifn != NULL) {
		n++;
		p++;
		i--;
		if (i == 0) {
			DCC_LOG(LOG_WARNING, "no room to add a new route");
			return -1;
		}
	}

	p = new;
	for (; n; n--) {
		addr = p->rt_dst;
		p->rt_dst = __dst;
		__dst = addr;
		
		addr = p->rt_genmask;
		p->rt_genmask = __mask;
		__mask = addr;
		
		addr = p->rt_gateway;
		p->rt_gateway = __gw;
		__gw = addr;

		tmp = p->rt_ifn;
		p->rt_ifn = __if;
		__if = tmp;

		p++;
	}

	p->rt_dst = __dst;
	p->rt_genmask = __mask;
	p->rt_gateway = __gw;
	p->rt_ifn = __if;

	DCC_LOG4(LOG_INFO, "(0x%05x) %I (%I) -> %I", 
		(int)p, p->rt_dst, p->rt_genmask, p->rt_gateway);

	return 0;
}

int ipv4_route_add(in_addr_t __dst, in_addr_t __mask, 
	in_addr_t __gw, struct ifnet * __if)
{
	int ret;

	tcpip_net_lock();

	ret = __route_add(__dst, __mask, __gw, __if);

	tcpip_net_unlock();

	return ret;
}

int ipv4_route_del(in_addr_t __dst)
{
	int ret;

	tcpip_net_lock();

	ret = __route_del(__dst);

	tcpip_net_unlock();

	return ret;
}

struct route * ipv4_route_lookup(in_addr_t __target)
{
	struct route * p;

	tcpip_net_lock();

	p =  __route_lookup(__target);

	tcpip_net_unlock();

	return p;
}



struct route * ipv4_route_get(in_addr_t __dst, in_addr_t __mask)
{
	int i;
	struct route * p;

	p = __route__;
	for (i = IP_ROUTE_TABLE_LEN; i; p++) {
		if (p->rt_ifn == NULL)
			return NULL;
		i--;
		if ((__dst == p->rt_dst) && (__mask == p->rt_genmask))
			return p;
	}

	return NULL;
}

int ipv4_route_enum(int (* __callback)(struct route *, void *), void * __parm)
{
	int i;
	struct route * p;
	int n;

	p = __route__;
	n = 0;
	for(i = 0; i < (IP_ROUTE_TABLE_LEN - 1); i++) {
		if (p->rt_ifn == NULL)
			break;
		n++;
		if (__callback != NULL) {
			if (__callback(p, __parm))
				break;
		}
		p++;
	}

	return n;
}

