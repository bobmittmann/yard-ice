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
#include <arpa/inet.h>

#include <tcpip/arp.h>

#include <sys/shell.h>

/*
   arp
*/
static int show_arp(struct ipv4_arp * p, FILE * f)
{
	char buf[16];

	if (p->type == ARPHRD_ETHER) {
		fprintf(f, "%-15s %02x:%02x:%02x:%02x:%02x:%02x\n", 
				inet_ntop(AF_INET, (void *)&p->ipaddr, buf, 16),
				p->hwaddr[0], p->hwaddr[1],
				p->hwaddr[2], p->hwaddr[3],
				p->hwaddr[4], p->hwaddr[5]);
	}

	return 0;
}
 
int cmd_arp(FILE *f, int argc, char ** argv)
{
	uint32_t ipaddr = 0;

	if (argc > 3) {
		fprintf(f, "usage: arp [IPADDR [HWADDR]]\n");
		return SHELL_ERR_EXTRA_ARGS;
	}

	if (argc > 1) {
		if (inet_aton(argv[1], (struct in_addr *)&ipaddr) == 0) {
			fprintf(f, "ip address invalid.\n");
			return SHELL_ERR_ARG_INVALID;
		}
		if (argc > 2) {
			/* ARP add */
			fprintf(f, "Not implemented!\n");
			return SHELL_ERR_GENERAL;
		} else {
			fprintf(f, "Sending IPV4 ARP query... ");
			/* ARP query */
			if (ipv4_arp_query(ipaddr) < 0)
				fprintf(f, "failed!\n");
			else
				fprintf(f, "ok.\n");
		}
	} else  {
		fprintf(f, "Address         HWaddress           Flags\n");
		ipv4_arp_enum((void *)show_arp, (void *)f);
	}

	return 0;
}

