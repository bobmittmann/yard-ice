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
 * @file jtagtool3.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <netinet/in.h>

#include <sys/dcclog.h>

#include "module.h"
#include "debugger.h"
#include "gdb_rspd.h"
#include "tftpd.h"
#include "dcc_tcp.h"
#include "version.h"
#include "vcom.h"
#include "jtag.h"
#include "dbglog.h" 
#include "nand.h" 

#if defined(ENABLE_TELNET) || defined(ENABLE_TFTP) \
	|| defined(ENABLE_GDB) || defined(ENABLE_DCC) \
    || defined(ENABLE_VCOM)
#ifndef ENABLE_NETWORK
#define ENABLE_NETWORK
#endif
#endif

#define DBG_MAX_MEM_INFO 4

#ifdef ENABLE_NETWORK
int network_config(void)
{
	struct ifnet * ifn;
	in_addr_t ip_addr;
	in_addr_t netmask = INADDR_ANY;
	in_addr_t gw_addr = INADDR_ANY;
	char s[64];
	char * env;
	int dhcp;

	if ((env=getenv("IPCFG")) == NULL) {
		printf("IPCFG not set, using defaults!\n");
		/* default configuration */
		strcpy(s, "192.168.0.2 255.255.255.0 192.168.0.254 0");
		/* set the default configuration */
		setenv("IPCFG", s, 1);
	} else {
		strcpy(s, env);
	}

//	printf("IPCFG='%s'\n", s);

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
	ifn = ethif_init(ip_addr, netmask);

	printf("* netif ");
	ifn_getname(ifn, s);
	printf("%s: ", s);
	printf("%s, ", inet_ntop(AF_INET, (void *)&ifn->if_ipv4_addr, s, 16));
	printf("%s\n", inet_ntop(AF_INET, (void *)&ifn->if_ipv4_mask, s, 16));

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
#endif

void debugger_init(void);

int init_debugger(void) 
{
	struct target_info * target;

	debugger_init();

	target = target_lookup(getenv("TARGET"));

	if (target == NULL) {
		target = target_first();
		printf("WARNING: invalid target.\n");
		printf("Fallback to default: %s.\n", target->name);
	};

	printf("* target select...\n");

	/* TODO: ice driver selection */
	if (target_configure(stdout, target, 1) < 0) {
		printf("ERROR: target_configure()!\n");
		return 0;
	}

	printf("* target: %s [%s-%s-%s %s-%s-%s]\n", target->name, 
		   target->arch->name, target->arch->model, target->arch->vendor,
		   target->arch->cpu->family, target->arch->cpu->model, 
		   target->arch->cpu->vendor);

	return 0;
}

int console_shell(void);
void telnet_shell(void);
int sys_start(void);

int main(void)
{
	DCC_LOG(LOG_TRACE, "...");

	printf("\nJTAGTOOL-III\n\n");

#ifdef ENABLE_NETWORK
//	uthread_sleep(100);
	network_config();
#endif

	modules_init();

	tracef("* Starting system module ...");
	sys_start();

	tracef("* Initializing JTAG module.");
	if (jtag_start() < 0) {
		tracef("jtag_start() failed!");
		return 0;
	}

#if (ENABLE_NAND)
	printf("* Initializing NAND module...");
	if (mod_nand_start() < 0) {
		printf("fail!\n");
		return 0;
	}
	printf("ok\n");
#endif

#ifdef ENABLE_TELNET
	printf("* starting TELNET server ... ");
	telnet_shell();
	printf("ok\n");
#endif

#ifdef ENABLE_TFTP
	printf("* starting TFTP server ... ");
	tftpd_start();
	printf("ok\n");
#endif

#ifdef ENABLE_GDB
	printf("* starting GDB daemon ... ");
	gdb_rspd_start();
	printf("ok\n");
#endif

#ifdef ENABLE_COMM
	printf("* starting COMM daemon ... ");
	comm_tcp_start(&debugger.comm);
	printf("ok\n");
#endif

#ifdef ENABLE_VCOM
	printf("* starting VCOM daemon ... ");
	vcom_start();
	printf("ok\n");
#endif

	uthread_sleep(150);
	init_debugger();

	return console_shell();
}

