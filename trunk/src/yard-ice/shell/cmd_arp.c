/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file cmd_arp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>

#include <tcpip/ethif.h>
#include <tcpip/etharp.h>
#include <tcpip/route.h>

#include <sys/shell.h>

static int arp_query(uint32_t daddr)
{
	struct route * rt;
	struct ifnet * ifn;

	if ((rt = route_lookup(daddr)) == NULL)
		return -1;

	if (rt->rt_gateway)
		/* non local address */
		return -1;

	/* get interface */
	ifn = rt->rt_ifn;

	/* ehternet arp  query */
	etharp_query(ifn, daddr);

	return 0;
}

/*
   arp
*/
static int show_etharp(struct etharp_entry * p, FILE * f)
{
	char buf[16];

	fprintf(f, "%-15s %02x:%02x:%02x:%02x:%02x:%02x\n", 
			inet_ntop(AF_INET, (void *)&p->ipaddr, buf, 16),
			p->hwaddr[0], p->hwaddr[1],
			p->hwaddr[2], p->hwaddr[3],
			p->hwaddr[4], p->hwaddr[5]);

	return 0;
}
 
int cmd_arp(FILE *f, int argc, char ** argv)
{
	uint32_t ipaddr = 0;

	if (argc > 2) {
		fprintf(f, "usage: arp [ipaddr hwaddr]\n");
		return -1;
	}

	if (argc > 1) {
		if (inet_aton(argv[1], (struct in_addr *)&ipaddr) == 0) {
			fprintf(f, "ip address invalid.\n");
			return -1;
		}
		arp_query(ipaddr);
	} else  {
		fprintf(f, "Address         HWaddress           Flags\n");
		etharp_enum((void *)show_etharp, (void *)f);
	}

	return 0;
}

