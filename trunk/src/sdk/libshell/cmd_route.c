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
 * @file cmd_route.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <tcpip/ethif.h>
#include <tcpip/route.h>

#include <sys/shell.h>

static int show_route(struct route * p, FILE * f)
{
	char st[16];
	char sn[16];
	char sg[16];
	char nm[8];

	struct ifnet * ifn;

	ifn = (struct ifnet *)p->rt_ifn;
	ifn_getname(ifn, nm);
	fprintf(f, "%-15s %-15s %-15s %s\n",
			inet_ntop(AF_INET, (void *)&p->rt_dst, st, 16),
			inet_ntop(AF_INET, (void *)&p->rt_gateway, sg, 16),
			inet_ntop(AF_INET, (void *)&p->rt_genmask, sn, 16), nm);

	return 0;
}

const char msg_route_usage[] = 
	"usage: route <add|del|help> dest [netmask [gateway]] iface \n";

int cmd_route(FILE * f, int argc, char ** argv)
{
	uint32_t dst;
	uint32_t gw;
	struct route * rt;
	in_addr_t ip_addr;
	in_addr_t netmask;
	struct ifnet * ifn;
	char buf[16];
	int argn;

	if (argc == 1) {
		fprintf(f, "Destination     Gateway         Genmask         Iface\n");
		ipv4_route_enum((void *)show_route, (void *)f);
		return 0;
	}

	if (strcmp(argv[1], "help") == 0) {
		fprintf(f, msg_route_usage);
		return 0;
	}

	if (argc < 3) {
		fprintf(f, msg_route_usage);
		return SHELL_ERR_ARG_MISSING;
	}

	if (argc > 6) {
		fprintf(f, msg_route_usage);
		return SHELL_ERR_EXTRA_ARGS;
	}

	if (strcmp(argv[1], "del") == 0) {
		if (inet_aton(argv[2], (struct in_addr *)&dst) == 0) {
			fprintf(f, "ip address invalid.\n");
			return SHELL_ERR_ARG_INVALID;
		}
		fprintf(f, "deleting entry %s.\n", 
				inet_ntop(AF_INET, (void *)&dst, buf, 16));
		return ipv4_route_del(dst);
	}

	if ((argc < 4) || (strcmp(argv[1], "add") != 0)) {
		fprintf(f, msg_route_usage);
		return SHELL_ERR_ARG_INVALID;
	}

	if (inet_aton(argv[2], (struct in_addr *)&dst) == 0) {
		fprintf(f, "ip address invalid.\n");
		return SHELL_ERR_ARG_INVALID;
	}

	argn = 3;
	if (argc > 3) {
		if (inet_aton(argv[argn++], (struct in_addr *)&netmask) == 0) {
			fprintf(f, "network mask invalid.\n");
			return SHELL_ERR_ARG_INVALID;
		}
	} else
		netmask = 0xffffffff;

	if (argc > 4) {
		if (inet_aton(argv[argn++], (struct in_addr *)&gw) == 0) {
			fprintf(f, "ip address invalid.\n");
			return SHELL_ERR_ARG_INVALID;
		}
	} else
		gw = 0;

	if (argc > 5) {
		if (inet_aton(argv[argn], (struct in_addr *)&ip_addr) > 0) {
			ifn = get_ifn_byipaddr(ip_addr);
		} else {
			ifn = get_ifn_byname(argv[argn]);
		}
		if (ifn == NULL) {
			fprintf(f, "interface not found %s.\n", argv[argn]);
			return SHELL_ERR_ARG_INVALID;
		}
	} else {
		if ((rt = ipv4_route_lookup(gw)) == NULL) {
			fprintf(f, "no route to gateway: %s", 
				   inet_ntop(AF_INET, (void *)&gw, buf, 16));
			return SHELL_ERR_ARG_INVALID;
		}
		ifn = rt->rt_ifn;
	}

	ipv4_route_add(dst, netmask, gw, ifn);

	return 0;
}

