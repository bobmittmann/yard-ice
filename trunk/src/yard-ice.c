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

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>
#include <sys/dcclog.h>

#include <thinkos.h>

#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/loopif.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void tcpip_init(void);

int network_config(void)
{
	struct ifnet * ifn;
	in_addr_t ip_addr;
	in_addr_t netmask = INADDR_ANY;
	in_addr_t gw_addr = INADDR_ANY;
	char s[64];
//	char * env;
	char * ip;
	int dhcp;

//	if ((env=getenv("IPCFG")) == NULL) {
		printf("IPCFG not set, using defaults!\n");
		/* default configuration */
		strcpy(s, "192.168.1.22 255.255.255.0 192.168.1.254 0");
		/* set the default configuration */
//		setenv("IPCFG", s, 1);
//	} else {
//		strcpy(s, env);
//	}

//	printf("IPCFG='%s'\n", s);

	printf("ipcfg=%s\n", s);
	ip = strtok(s, " ,");

	printf("ip=%s\n", ip);


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
	ifn = ethif_init(ip_addr, netmask);
//	ifn = loopif_init(ip_addr, netmask);

	printf("* netif ");
	ifn_getname(ifn, s);
	printf("%s: ", s);

	ifn_ipv4_get(ifn, &ip_addr, &netmask);
	printf("%s, ", inet_ntop(AF_INET, (void *)&ip_addr, s, 16));
	printf("%s\n", inet_ntop(AF_INET, (void *)&netmask, s, 16));

	if (gw_addr != INADDR_ANY) {
		/* add the default route (gateway) to ethif */
		route_add(INADDR_ANY, INADDR_ANY, gw_addr, ifn);
		printf("* default route gw: ");
		printf("%s\n", inet_ntop(AF_INET, (void *)&gw_addr, s, 16));
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

int main(int argc, char ** argv)
{
	int i;

	DCC_LOG(LOG_TRACE, "cm3_udelay_calibrate()");
	cm3_udelay_calibrate();
	udelay(100000);
	DCC_LOG(LOG_TRACE, "stm32f_usart_open().");
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);
	udelay(100000);

	DCC_LOG(LOG_TRACE, "thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "tcpip_init().");
	tcpip_init();

	DCC_LOG(LOG_TRACE, "network_config().");
	network_config();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" YAR-ICE\n");
	printf("---------------------------------------------------------\n");
	printf("\n");


	for (i = 0; i < 1000; i++) {
		DCC_LOG1(LOG_TRACE, "%d.", i);
		thinkos_sleep(2000);
	}

	printf("---------------------------------------------------------\n");
	delay(10);

	return 0;
}

