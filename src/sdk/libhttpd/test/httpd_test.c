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
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/serial.h>
#include <sys/delay.h>
#include <sys/dcclog.h>
#include <tcpip/tcp.h>
#include <netinet/in.h>

#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>

#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/loopif.h>
#include <tcpip/httpd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void tcpip_init(void);
void env_init(void);
void stdio_init(void);

extern struct httpdobj www_root[];
extern struct httpdobj www_img[];

struct httpddir httpd_dir[] = {
	{ .path = "/", .objlst = www_root },
	{ .path = "/img", .objlst = www_img },
	{ .path = NULL, .objlst = NULL }
};

int httpd_server_task(struct httpd * httpd)
{
	struct httpctl httpctl;
	struct httpctl * ctl = &httpctl;

	for (;;) {
//		printf("Wating for connection.\n");
		DCC_LOG1(LOG_INFO, "<%d> Wating for connection...", 
				 thinkos_thread_self());
		if (http_accept(httpd, ctl) < 0) {
//			printf("tcp_accept() failed!\n");
			break;
		}

//		printf("Connection accepted.\n");
		DCC_LOG1(LOG_INFO, "<%d> Connection accepted...", 
				 thinkos_thread_self());

		switch (ctl->method) {
		case HTTP_GET:
			DCC_LOG(LOG_INFO, "GET");
			http_get(ctl);
			break;
		case HTTP_POST:
			DCC_LOG(LOG_TRACE, "POST");
//			http_post(httpd, ctl);
			break;
		}

		http_close(ctl);
	}

	return 0;
}


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
	/* TODO: random initial mac address */
	uint8_t ethaddr[6] = { 0x1c, 0x95, 0x5d, 0x00, 0x00, 0x80};
	uint64_t esn;
	int dhcp;

	esn = *((uint64_t *)STM32F_UID);
	DCC_LOG2(LOG_TRACE, "ESN=0x%08x%08x", esn >> 32, esn);

	ethaddr[0] = ((esn >>  0) & 0xfc) | 0x02; /* Locally administered MAC */
	ethaddr[1] = ((esn >>  8) & 0xff);
	ethaddr[2] = ((esn >> 16) & 0xff);
	ethaddr[3] = ((esn >> 24) & 0xff);
	ethaddr[4] = ((esn >> 32) & 0xff);
	ethaddr[5] = ((esn >> 40) & 0xff);

	printf("* mac addr: %02x-%02x-%02x-%02x-%02x-%02x\n ", 
		   ethaddr[0], ethaddr[1], ethaddr[2],
		   ethaddr[3], ethaddr[4], ethaddr[5]);

	DCC_LOG(LOG_TRACE, "tcpip_init().");
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
		DCC_LOG(LOG_WARNING, "inet_aton() failed.");
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
		tracef("DHCP started.\n");
#endif
	}

	return 0;
}

uint32_t server_stack1[1024];
uint32_t server_stack2[1024];

int main(int argc, char ** argv)
{
	struct httpd httpd;
	int port = 80;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. env_init().");
	env_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "3. stdio_init().");
	stdio_init();


	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS (TCP Test)\n");
	printf("---------------------------------------------------------\n");
	printf("\n");


	DCC_LOG(LOG_TRACE, "4. network_config().");
	network_config();

	httpd_start(&httpd, port, 4, httpd_dir, NULL);
	printf("Listening on port %d.\n", port);

	DCC_LOG(LOG_TRACE, "2. starting workers...");
	thinkos_thread_create((void *)httpd_server_task, (void *)&httpd,
						  server_stack1, sizeof(server_stack1) |
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(8));

	thinkos_thread_create((void *)httpd_server_task, (void *)&httpd,
						  server_stack2, sizeof(server_stack2) |
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(7));


	for (;;) {
		thinkos_sleep(1000);
	}

	return 0;
}


