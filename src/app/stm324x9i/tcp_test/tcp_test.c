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
#include <sys/console.h>
#include <sys/tty.h>
#include <sys/null.h>

#include <thinkos.h>

#include <tcpip/tcp.h>
#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/loopif.h>
#include <tcpip/net.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "board.h"

void tcpip_init(void);
void env_init(void);

void stdio_init(void)
{
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	f_raw = console_fopen();
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = console_fopen();
//	stdout = f_tty;
	stdout = null_fopen(0);
	stdin = f_tty;
}

extern const char zarathustra_0[];
extern const char zarathustra_1[];
extern const char zarathustra_2[];
extern const char zarathustra_3[];
extern const char zarathustra_4[];
extern const char zarathustra_5[];
extern const char zarathustra_6[];
extern const char zarathustra_7[];
extern const char zarathustra_8[];
extern const char zarathustra_9[];
extern const char zarathustra_10[];

const char const * zarathustra[] = {
	zarathustra_0,
	zarathustra_1,
	zarathustra_2,
	zarathustra_3,
	zarathustra_4,
	zarathustra_5,
	zarathustra_6,
	zarathustra_7,
	zarathustra_8,
	zarathustra_9,
	zarathustra_10,
	NULL
};

int tcp_server_task(void * arg)
{
	struct tcp_pcb * svc;
	struct tcp_pcb * tp;
	uint32_t start;
	uint32_t end;
	unsigned int ms;
	char * s;
	int i;
	int n;
	int size;
	int again;
	int port = 100;

	svc = tcp_alloc();

	tcp_bind(svc, INADDR_ANY, htons(port));

	if (tcp_listen(svc, 1) != 0) {
		printf("Can't register the TCP listner!\n");
		return -1;
	}

//	printf(" - %s(%d): starting...\n", __FUNCTION__, id);

	printf("Listening on port %d.\n", port);

	for (;;) {
		printf("Wating for connection.\n");
		if ((tp = tcp_accept(svc)) == NULL) {
			printf("tcp_accept() failed!\n");
			break;
		}

		printf("Connection accepted.\n");

		again = 100;

		size = 0;
//		clock_gettime(CLOCK_REALTIME, &start);
		start = thinkos_clock();

		do {
			i = 0;
			again--;
			while ((s = (char *)zarathustra[i++]) != NULL) {
				n = strlen(s);
				if ((n = tcp_send(tp, s, n, 0)) < 0) {
					again = 0;
					break;
				}
				size += n;
			}
		} while (again);

		end = thinkos_clock();

		tcp_close(tp);

		ms = (int32_t)(end - start);

		printf("EOT: size=%d tm=%d(ms) speed=%d(KiB/s)\n", size, ms, size / ms);
//		show_tcpip_stat();
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
		tracef("DHCP started.\n");
#endif
	}

	return 0;
}

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOG);
	stm32_gpio_mode(LED1, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED2, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED3, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED4, OUTPUT, OPEN_DRAIN | SPEED_MED);

	stm32_gpio_set(LED1);
	stm32_gpio_set(LED2);
	stm32_gpio_set(LED3);
	stm32_gpio_set(LED4);
}

uint32_t server_stack[256];

int main(int argc, char ** argv)
{
	stm32f_nvram_env_init();

	io_init();

	stdio_init();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS (TCP Test)\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	network_config();

	thinkos_thread_create((void *)tcp_server_task, (void *)NULL,
						  server_stack, sizeof(server_stack) |
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(8));


	for (;;) {
		thinkos_sleep(1000);
		printf(".");
	}

	return 0;
}


