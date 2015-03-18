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
 * @file cmd_ifconfig.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <tcpip/ethif.h>
#include <tcpip/ifnet.h>

#include <sys/shell.h>


/*
 * ifconfig
 */

#define BUF_SIZE 32

static int show_ifn(struct ifnet * __if, FILE * f)
{
	struct ifnet_info inf;
	char buf[BUF_SIZE];
	in_addr_t ipv4_bcast;
	in_addr_t ipv4_addr;
	in_addr_t ipv4_mask;

	ifn_getinfo(__if, &inf);

	fprintf(f, "%s, \tLink encap:", inf.name);

	if (inf.type == IFT_ETHER) {
		fprintf(f, "Ethernet  HWaddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
			inf.hw_addr[0], inf.hw_addr[1], inf.hw_addr[2], 
			inf.hw_addr[3], inf.hw_addr[4], inf.hw_addr[5]);
	} else { 
		if (inf.type == IFT_PPP)
			fprintf(f, "POINTTOPOINT ");
		else {
			if (inf.type == IFT_LOOP)
				fprintf(f, "Local Loopback\n");
			else
				fprintf(f, "Unknown\n");
		}
	}

	ifn_ipv4_get(__if, &ipv4_addr, &ipv4_mask);

	fprintf(f, "\tinet addr: %s", 
		   inet_ntop(AF_INET, (void *)&ipv4_addr, buf, 16));

	if (inf.flags & IFF_BROADCAST) {
		ipv4_bcast = (ipv4_addr & ipv4_mask) | ~ipv4_mask;
		if (ipv4_bcast != ipv4_addr) {
			fprintf(f, " Bcast: %s" , 
					inet_ntop(AF_INET, (void *)&ipv4_bcast, buf, 16));
		}
	}
	fprintf(f, " Mask: %s\n\t", 
			inet_ntop(AF_INET, (void *)&ipv4_mask, buf, 16));

	if (inf.flags & IFF_UP)
		fprintf(f, "UP ");
	if (inf.flags & IFF_BROADCAST)
		fprintf(f, "BROADCAST ");
	if (inf.flags & IFF_LINK_UP)
		fprintf(f, "RUNNING ");
	if (inf.flags & IFF_POINTTOPOINT)
		fprintf(f, "POINTTOPOINT ");
	if (inf.flags & IFF_LOOPBACK)
		fprintf(f, "LOOPBACK ");
	if (inf.flags & IFF_NOARP)
		fprintf(f, "NOARP ");
	if (inf.flags & IFF_MULTICAST)
		fprintf(f, "MULTICAST ");

	fprintf(f, "MTU:%d\n", inf.mtu);
	if (strlen(inf.desc) > 0)
		fprintf(f, "\t%s\n", inf.desc);

	fprintf(f, "\n");

	return 0;
}

int cmd_ifconfig(FILE *f, int argc, char ** argv)
{

	in_addr_t ip_addr;
	in_addr_t netmask;
	struct ifnet * ifn;

	if (argc > 4) {
		return SHELL_ERR_EXTRA_ARGS;
	}

	if (--argc == 0) {
		ifn_enum((void *)show_ifn, (void *)f);
		return 0;
	}

	if (strcmp(argv[1], "help") == 0) {
		/* FIXME implement help */
		return 0;
	}

	if (inet_aton(argv[1], (struct in_addr *)&ip_addr) == 0) {
		if ((ifn = get_ifn_byname(argv[1])) == NULL) {
			fprintf(f, "network interface not found.\n");
			return SHELL_ERR_ARG_INVALID;
		}
		if (argc == 1) {
			show_ifn(ifn, NULL);
			return 0;
		}
		if (inet_aton(argv[2], (struct in_addr *)&ip_addr) == 0) {
			fprintf(f, "ip address invalid.\n");
			return SHELL_ERR_ARG_INVALID;
		}
		argc--;
	} else {
		ifn = get_ifn_byipaddr(INADDR_ANY);
	}

	if (argc > 1) {
		if (inet_aton(argv[argc], (struct in_addr *)&netmask) == 0) {
			fprintf(f, "network mask invalid.\n");
			return SHELL_ERR_ARG_INVALID;
		}
	} else {
		if (IN_CLASSA(ip_addr))
			netmask = IN_CLASSA_NET;
		else {
			if (IN_CLASSB(ip_addr))
				netmask = IN_CLASSB_NET;
			else {
				if (IN_CLASSC(ip_addr))
					netmask = IN_CLASSC_NET;
				else	
					netmask = INADDR_NONE;
			}
		}
	}

	ifn_ipv4_set(ifn, ip_addr, netmask);

	show_ifn(ifn, f);

	return 0;
}

