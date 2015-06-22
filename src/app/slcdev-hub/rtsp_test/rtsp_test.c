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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file rtp_test.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stm32f.h>
#include <errno.h>

#include <tcpip/udp.h>
#include <tcpip/tcp.h>
#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/loopif.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <thinkos.h>
#include <sys/console.h>

struct rtp_client {
	in_addr_t addr;
	uint16_t port;
	struct udp_pcb * udp;
};

struct rtsp_client {
	struct rtp_client rtp;
	uint16_t port;
	struct tcp_pcb * tcp;
};

int rtp_task(struct rtp_client * clnt)
{
	uint8_t buf[1472];
	struct sockaddr_in sin;
	struct udp_pcb * udp;
	int len;
	char s1[16];

	printf("- Thread: %d\n", thinkos_thread_self());
	printf("- UDP port: %d\n", clnt->port);

	if ((udp = udp_alloc()) == NULL) {
		return -1;
	}

	if (udp_bind(udp, INADDR_ANY, htons(clnt->port)) < 0) {
		return -1;
	}

	clnt->udp = udp;

	for (;;) {
		if ((len = udp_recv(udp, buf, sizeof(buf), &sin)) < 0) {
			if (len == -ECONNREFUSED) {
				printf("udp_rcv ICMP error: ECONNREFUSED\n");
			}
			if (len == -EFAULT) {
				printf("udp_rcv error: EFAULT\n");
			}
			if (len == -ENOTCONN) {
				printf("udp_rcv error: ENOTCONN\n");
			}
			continue;
		}


		printf("* UDP datagram from %s:%d, len=%d\n",
			   inet_ntop(AF_INET, (void *)&sin.sin_addr.s_addr, s1, 16),
			   ntohs(sin.sin_port), len);

	}	

	return 0;
}

uint32_t rtp_stack[1024];

const struct thinkos_thread_inf rtp_inf = {
	.stack_ptr = rtp_stack,
	.stack_size = sizeof(rtp_stack),
	.priority = 32,
	.thread_id = 8, 
	.paused = 0,
	.tag = "RTP"
};

void rtp_client_start(struct rtp_client * rtp)
{
	thinkos_thread_create_inf((void *)rtp_task, (void *)rtp, &rtp_inf);
}

void tcpip_init(void);

int network_config(void)
{
	struct ifnet * ifn;
	in_addr_t ip_addr;
	in_addr_t netmask = INADDR_ANY;
	in_addr_t gw_addr = INADDR_ANY;
	char s[64];
	char s1[16];
	char s2[16];
	char * env;
	uint8_t ethaddr[6] = { 0x1c, 0x95, 0x5d, 0x00, 0x00, 0x80};
	uint64_t esn;
	int dhcp;

	esn = *((uint64_t *)STM32F_UID);

	ethaddr[0] = ((esn >>  0) & 0xfc) | 0x02; /* Locally administered MAC */
	ethaddr[1] = ((esn >>  8) & 0xff);
	ethaddr[2] = ((esn >> 16) & 0xff);
	ethaddr[3] = ((esn >> 24) & 0xff);
	ethaddr[4] = ((esn >> 32) & 0xff);
	ethaddr[5] = ((esn >> 40) & 0xff);

	printf("* mac addr: %02x-%02x-%02x-%02x-%02x-%02x\n ", 
		   ethaddr[0], ethaddr[1], ethaddr[2],
		   ethaddr[3], ethaddr[4], ethaddr[5]);

	tcpip_init();

//	if ((env = getenv("IPCFG")) == NULL) {
	if (1) {
		printf("IPCFG not set, using defaults!\n");
		/* default configuration */
		strcpy(s, "192.168.10.128 255.255.255.0 192.168.10.254 0");
		/* set the default configuration */
		setenv("IPCFG", s, 1);
	} else {
		strcpy(s, env);
	}

	if (!inet_aton(strtok(s, " ,"), (struct in_addr *)&ip_addr)) {
		return -1;
	}

	if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&netmask)) {
		if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&gw_addr)) {
			dhcp = strtoul(strtok(NULL, ""), NULL, 0);
		}
	}

	/* initialize the Ethernet interface */
	/* configure the ip address */
	ifn = ethif_init(ethaddr, ip_addr, netmask);
//	ifn = loopif_init(ip_addr, netmask);

	ifn_getname(ifn, s);
	ifn_ipv4_get(ifn, &ip_addr, &netmask);
	printf("* netif %s: %s, %s\n", s, 
		   inet_ntop(AF_INET, (void *)&ip_addr, s1, 16),
		   inet_ntop(AF_INET, (void *)&netmask, s2, 16));

	if (gw_addr != INADDR_ANY) {
		/* add the default route (gateway) to ethif */
		ipv4_route_add(INADDR_ANY, INADDR_ANY, gw_addr, ifn);
		printf("* default route gw: %s\n", 
			   inet_ntop(AF_INET, (void *)&gw_addr, s1, 16));
	}

	if (dhcp) {
#if 0
		/* configure the initial ip address */
		dhcp_start();
		/* schedule the interface to be configured through dhcp */
		dhcp_ifconfig(ethif, dhcp_callback);
		printf("DHCP started.\n");
#endif
	}

	return 0;
}

void stdio_init(void)
{
	FILE * f;

	f = console_fopen();
	/* initialize STDIO */
	stderr = f;
	stdout = f;
	stdin = f;
}

#define BUF_LEN 128

int rtsp_request(in_addr_t host, const char * req)
{
	int port = 544;
	struct tcp_pcb * tp;
	uint8_t buf[BUF_LEN];
	int n;

	if ((tp = tcp_alloc()) == NULL) {
		fprintf(stderr, "can't allocate socket!\n");
		return -1;
	}

	if (tcp_connect(tp, host, htons(port)) < 0) {
		fprintf(stderr, "can't connect to host!\n");
		return -1;
	}

	if (tcp_send(tp, req, strlen(req), 0) < 0) {
		fprintf(stderr, "can't send!\n");
		return -1;
	}

	while ((n = tcp_recv(tp, buf, BUF_LEN)) > 0)  {
		buf[n] = '\0';
		printf((char *)buf);
	}

	tcp_close(tp);

	return 0;
}

int rtsp_connect(struct rtsp_client * rtsp, const char * host)
{
	int rtp_port = 6970;
	in_addr_t host_addr;
	char req[512];

	if (!inet_aton(host, (struct in_addr *)&host_addr)) {
		return -1;
	}

	sprintf(req,
			"C->S: DESCRIBE rtsp://%s/autio.au RTSP/1.0\r\n"
			"CSeq: 2\r\n", host);

	if (rtsp_request(host_addr, req) < 0)
		return -1;

	rtsp->rtp.port = rtp_port;
	rtsp->rtp.addr = host_addr;

	rtp_client_start(&rtsp->rtp);

	return 0;
}

volatile bool do_connect = true;

int main(int argc, char ** argv)
{
	struct rtsp_client rtsp;

	stdio_init();

	stm32f_nvram_env_init();

	network_config();

	for (;;) {
		if (do_connect) {
			do_connect = false;
			if (rtsp_connect(&rtsp, "192.168.10.254") < 0) {
				printf("\nRTSP connection failed!\n");
			}
		}
		thinkos_sleep(1000);
		printf(".");
	}

	return 0;
}


