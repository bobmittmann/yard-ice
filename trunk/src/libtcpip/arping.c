/*
 * File:	arping.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment:	
 * Copyright(c) 2004-2010 BORESTE (www.boreste.com). All Rights Reserved.
 *
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

