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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <tcpip/ethif.h>
#include <tcpip/etharp.h>
#include <tcpip/route.h>
#include <tcpip/raw.h>

#include <sys/shell.h>

#define DATA_LEN 64
#define BUF_LEN (DATA_LEN + sizeof(struct iphdr) + sizeof(struct icmphdr))

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
//	int opt;
//	int tm;
	int dt;
	int i;
	int n;
	struct timeval tv;
	struct timeval * ts;

	if (argc > 3) {
		fprintf(f, "ping - send ICMP ECHO_REQUEST to network host\n");
	 	fprintf(f, "usage: ping HOST\n");
		return -1;
	}

	if (inet_aton(argv[1], (struct in_addr *)&ip_addr) == 0) {
		fprintf(f, "ip address invalid.\n");
		return -1;
	}

	if (argc > 2) {
		n = strtol(argv[2], NULL, 0);
	} else {
		n = 5;
	}

	raw = raw_pcb_new(IPPROTO_ICMP);

//	opt = 1;
//	raw_ioctl(raw, FIONBIO, &opt);

	id = uthread_id();	
	datalen = DATA_LEN;

	fprintf(f, "PING %s: %d octets data.\n", 
			inet_ntop(AF_INET, (void *)&ip_addr, s, 16), datalen);

	for (seq = 1; seq <= n; seq++) {
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
		gettimeofday(&tv, NULL);
		memcpy(data, &tv, sizeof(struct timeval));

		icmp->chksum = 0;
		icmp->chksum = ~in_chksum(0, icmp, len);

		raw_sendto(raw, buf, len, (struct sockaddr_in *)&sin);
	
/*		for (tm = 100; tm > 0; tm--) {
			len = raw_recvfrom(raw, buf, BUF_LEN, (struct sockaddr_in *)&sin);

			if (len < 0) {
				if (len != -EAGAIN)
					return -1;
				uthread_sleep(10);
				continue;
			}

			ip = (struct iphdr *)buf;
			iphdrlen = ip->hlen * 4;
			icmp = (struct icmphdr *)(buf + iphdrlen);

			if ((icmp->type == ICMP_ECHOREPLY) && 
				(icmp->un.echo.id == id)) {

				ts = (struct timeval *)(buf + iphdrlen + 
										sizeof(struct icmphdr));

				len -= iphdrlen + sizeof(struct icmphdr);

				gettimeofday(&tv, NULL);
				dt = (int)(tv.tv_sec - ts->tv_sec) * 1000;
				dt += (int)(tv.tv_usec - ts->tv_usec) / 1000;

				fprintf(f, "%d octets from %s: icmp_seq=%d "
						"ttl=%d time=%d ms\n",
						len, inet_ntop(AF_INET, (void *)&sin.sin_addr, s, 16), 
						icmp->un.echo.sequence, len, dt);
				break;
			}
		}
		if (tm == 0) {
			fprintf(f, "timed out.\n");
		}

		for (; tm > 0; tm--) {
			uthread_sleep(10);
		}
		*/

		len = raw_recvfrom_tmo(raw, buf, BUF_LEN, 
							   (struct sockaddr_in *)&sin, 1000);

		if (len < 0) {
			if (len != -ETIMEDOUT) {
				return -1;
			}
			fprintf(f, "timed out.\n");
			continue;
		}

		gettimeofday(&tv, NULL);

		ip = (struct iphdr *)buf;
		iphdrlen = ip->hlen * 4;
		icmp = (struct icmphdr *)(buf + iphdrlen);

		if ((icmp->type == ICMP_ECHOREPLY) && 
			(icmp->un.echo.id == id)) {

			ts = (struct timeval *)(buf + iphdrlen + 
									sizeof(struct icmphdr));

			len -= iphdrlen + sizeof(struct icmphdr);

			dt = (int)(tv.tv_sec - ts->tv_sec) * 1000;
			dt += (int)(tv.tv_usec - ts->tv_usec) / 1000;

			fprintf(f, "%d octets from %s: icmp_seq=%d "
					"ttl=%d time=%d ms\n",
					len, inet_ntop(AF_INET, (void *)&sin.sin_addr, s, 16), 
					icmp->un.echo.sequence, ip->ttl, dt);
		} else {
			fprintf(f, "icmp: %d\n", icmp->type);
		}

		uthread_sleep(250);
	}

	raw_close(raw);

	return 0;
}

