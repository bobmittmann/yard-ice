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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <tcpip/ethif.h>

#include <sys/shell.h>

/*
 * ifconfig
 */

#define BUF_SIZE 32

static int show_ifn(struct ifnet * __if, FILE * f)
{
	uint32_t bcast;
	char buf[BUF_SIZE];
	int type;

	ifn_getname(__if, buf);

	fprintf(f, "%s, \tLink encap:", buf);

	type = __if->if_id & IFT_MASK;
	if (type == IFT_ETHER) {
		ifn_getaddr(__if, (unsigned char *)buf);
		fprintf(f, "Ethernet  HWaddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
			buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
	} else { 
		if (type == IFT_PPP)
			fprintf(f, "POINTTOPOINT ");
		else {
			if (type == IFT_LOOP)
				fprintf(f, "Local Loopback\n");
			else
				fprintf(f, "Unknown\n");
		}
	}
	fprintf(f, "\tinet addr: %s", 
		   inet_ntop(AF_INET, (void *)&__if->if_ipv4_addr, buf, 16));

	if (__if->if_flags & IFF_BROADCAST) {
		bcast = (__if->if_ipv4_addr & __if->if_ipv4_mask) | ~__if->if_ipv4_mask;
		if (bcast != __if->if_ipv4_addr) {
			fprintf(f, " Bcast: %s" , 
					inet_ntop(AF_INET, (void *)&bcast, buf, 16));
		}
	}
	fprintf(f, " Mask: %s\n\t", 
			inet_ntop(AF_INET, (void *)&__if->if_ipv4_mask, buf, 16));

	if (__if->if_flags & IFF_UP)
		fprintf(f, "UP ");
	if (__if->if_flags & IFF_BROADCAST)
		fprintf(f, "BROADCAST ");
	if (__if->if_flags & IFF_LINK_UP)
		fprintf(f, "RUNNING ");
	if (__if->if_flags & IFF_POINTTOPOINT)
		fprintf(f, "POINTTOPOINT ");
	if (__if->if_flags & IFF_LOOPBACK)
		fprintf(f, "LOOPBACK ");
	if (__if->if_flags & IFF_NOARP)
		fprintf(f, "NOARP ");
	if (__if->if_flags & IFF_MULTICAST)
		fprintf(f, "MULTICAST ");

	fprintf(f, "MTU:%d\n", __if->if_mtu);
	if (ifn_getinfo(__if, buf, BUF_SIZE) > 0) {
		fprintf(f, "\t%s\n", buf);
	}

	fprintf(f, "\n");

	return 0;
}

int cmd_ifconfig(FILE *f, int argc, char ** argv)
{

	in_addr_t ip_addr;
	in_addr_t netmask;
	struct ifnet * ifn;

	if (argc > 4) {
		return -1;
	}

	if (--argc == 0) {
		ifn_enum((void *)show_ifn, (void *)f);
		return 0;
	}

	if (strcmp(argv[1], "help") == 0) {
//		shell_write(msg_ifconfig_usage, strlen(msg_ifconfig_usage), tty);
		return 0;
	}

	if (inet_aton(argv[1], (struct in_addr *)&ip_addr) == 0) {
		if ((ifn = get_ifn_byname(argv[1])) == NULL) {
			fprintf(f, "network interface not found.\n");
			return -1;
		}
		if (argc == 1) {
			show_ifn(ifn, NULL);
			return 0;
		}
		if (inet_aton(argv[2], (struct in_addr *)&ip_addr) == 0) {
			fprintf(f, "ip address invalid.\n");
			return -1;
		}
		argc--;
	} else {
		ifn = get_ifn_byipaddr(INADDR_ANY);
	}

	if (argc > 1) {
		if (inet_aton(argv[argc], (struct in_addr *)&netmask) == 0) {
			fprintf(f, "network mask invalid.\n");
			return -1;
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

	ifn_ipconfig(ifn, ip_addr, netmask);
	show_ifn(ifn, f);

	return 0;
}

