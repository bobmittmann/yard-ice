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
 * @file arping.c
 * @brief IP address self configuring
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef ARPING_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef CONFIG_H
#include "config.h"
#endif

#include <netinet/ip_icmp.h>

#include <tcpip/ifnet.h>
#include <tcpip/ip.h>
#include <tcpip/icmp.h>

#include <sys/dcclog.h>

int arp_ping_ipconfig(struct ifnet * __if, struct iphdr * __ip, 
					  struct icmphdr * __icmp, int __len)
{
	in_addr_t ipaddr;
	in_addr_t netmask;

	if (__icmp->type == ICMP_ECHO) {
		DCC_LOG1(LOG_TRACE, "ICMP: %d bytes: echo request", __len);

		ipaddr = __ip->daddr;
		if (IN_CLASSA(ipaddr))
			netmask = IN_CLASSA_NET;
		else {
			if (IN_CLASSB(ipaddr))
				netmask = IN_CLASSB_NET;
			else {
				if (IN_CLASSC(ipaddr))
					netmask = IN_CLASSC_NET;
				else	
					netmask = INADDR_NONE;
			}
		}

		ifn_ipv4_set(__if, ipaddr, netmask);

		return icmp_echoreplay(__if, __ip, __icmp, __len);
	} else
		return -1;
}

