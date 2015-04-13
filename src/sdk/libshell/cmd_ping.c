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
 * @file cmd_ping.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
//#include <sys/ioctl.h>

#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/raw.h>

#include <sys/shell.h>

#include <thinkos.h>

#include <tcpip/in.h>

#define DATA_LEN 64
#define BUF_LEN (DATA_LEN + sizeof(struct iphdr) + sizeof(struct icmphdr))

const char msg_ping_usage[] = 
	"usage: ping HOST\n";

int cmd_ping(FILE * f, int argc, char ** argv)
{
	struct raw_pcb * raw;
	uint8_t buf[BUF_LEN];
	in_addr_t ip_addr;
	struct sockaddr_in sin;
	struct iphdr * ip;
	struct icmphdr * icmp;
	uint8_t * data;
	int iphdrlen;
	int datalen;
	char s[16];
	int len;
	int id;
	int seq;
	int32_t dt;
	uint32_t ts;
	uint32_t now;
	int cnt;
	int i;
	int ret = 0;

	if (argc < 2) {
		fprintf(f, msg_ping_usage);
		return SHELL_ERR_ARG_MISSING;
	}

	if (argc > 3) {
		fprintf(f, msg_ping_usage);
		return SHELL_ERR_EXTRA_ARGS;
	}

	if (strcmp(argv[1], "help") == 0) {
		fprintf(f, "ping - send ICMP ECHO_REQUEST to network host\n");
		fprintf(f, msg_ping_usage);
		return 0;
	}

	if (inet_aton(argv[1], (struct in_addr *)&ip_addr) == 0) {
		fprintf(f, "ip address invalid.\n");
		return SHELL_ERR_ARG_INVALID;
	}

	if (argc > 2) {
		cnt = strtol(argv[2], NULL, 0);
	} else {
		cnt = 5;
	}

	raw = raw_pcb_new(IPPROTO_ICMP);

	id = thinkos_thread_self();	
	datalen = DATA_LEN;

	fprintf(f, "PING %s: %d octets data.\n", 
			inet_ntop(AF_INET, (void *)&ip_addr, s, 16), datalen);

	for (seq = 1; seq <= cnt; seq++) {
		icmp = (struct icmphdr *)(void *)buf;
		icmp->type = ICMP_ECHO;
		icmp->un.echo.id = id;
		icmp->un.echo.sequence = seq;
		data = buf + sizeof(struct icmphdr);
		for (i = 0; i < datalen; i++) {
			data[i] = i;
		}
		len = datalen + sizeof(struct icmphdr);

		sin.sin_addr.s_addr = ip_addr;
		sin.sin_family = AF_INET;

		ts = thinkos_clock();
//		gettimeofday(&tv, NULL);
		memcpy(data, &ts, sizeof(uint32_t));

		icmp->chksum = 0;
		icmp->chksum = ~in_chksum(0, icmp, len);

		raw_sendto(raw, buf, len, (struct sockaddr_in *)&sin);

		len = raw_recvfrom_tmo(raw, buf, BUF_LEN, 
							   (struct sockaddr_in *)&sin, 1000);

		if (len < 0) {
			if (len != -ETIMEDOUT) {
				ret = -1;
				break;
			}
			fprintf(f, "timed out.\n");
			continue;
		}

		now = thinkos_clock();

		ip = (struct iphdr *)buf;
		iphdrlen = ip->hlen * 4;
		icmp = (struct icmphdr *)(buf + iphdrlen);

		if ((icmp->type == ICMP_ECHOREPLY) && 
			(icmp->un.echo.id == id)) {

			memcpy(&ts, buf + iphdrlen + sizeof(struct icmphdr), 
				   sizeof(uint32_t));

			len -= iphdrlen + sizeof(struct icmphdr);

			dt = (int32_t)(now - ts);

			fprintf(f, "%d octets from %s: icmp_seq=%d "
					"ttl=%d time=%d ms\n",
					len, inet_ntop(AF_INET, (void *)&sin.sin_addr, s, 16), 
					icmp->un.echo.sequence, ip->ttl, dt);
		} else {
			fprintf(f, "icmp: %d\n", icmp->type);
		}

		thinkos_sleep(250);
	}

	raw_close(raw);

	return ret;
}

