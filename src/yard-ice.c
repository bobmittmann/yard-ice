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

#include <jtag.h>
#include <trace.h>

#include <yard-ice/drv.h>

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

#ifndef ENABLE_NETWORK
#define ENABLE_NETWORK 0
#endif

#ifndef ENABLE_USB
#define ENABLE_USB 0
#endif

#ifndef ENABLE_TELNET
#define ENABLE_TELNET 0
#endif

#ifndef ENABLE_TFTP
#define ENABLE_TFTP 0
#endif

void tcpip_init(void);

const struct file stm32f_uart_file = {
	.data = STM32F_UART5, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	stderr = (struct file *)&stm32f_uart_file;
	stdout = uart_console_fopen(uart_console_init(115200, SERIAL_8N1));
	stdin = stdout;
}

int supervisor_task(void)
{
	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		trace_fprint(stdout, TRACE_FLUSH);
		thinkos_sleep(250);
	}
}

#if ENABLE_NETWORK
int network_config(void)
{
	struct ifnet * ifn;
	in_addr_t ip_addr;
	in_addr_t netmask = INADDR_ANY;
	in_addr_t gw_addr = INADDR_ANY;
	char s[64];
	char * env;
	char * ip;
	int dhcp;

	DCC_LOG(LOG_TRACE, "tcpip_init().");
	tcpip_init();

	if ((env = getenv("IPCFG")) == NULL) {
		printf("IPCFG not set, using defaults!\n");
		/* default configuration */
		strcpy(s, "192.168.1.22 255.255.255.0 192.168.1.254 0");
		/* set the default configuration */
		setenv("IPCFG", s, 1);
	} else {
		strcpy(s, env);
	}

	printf("IPCFG='%s'\n", s);

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
#endif

void debugger_init(void);

int init_debugger(void) 
{
#if 0
	struct target_info * target;
#endif

	debugger_init();

#if 0
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
#endif
	return 0;
}

uint32_t supervisor_stack[256];

int console_shell(void);
void telnet_shell(void);
void usb_shell(void);
int sys_start(void);

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();
	trace_init();
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));
	stdio_init();

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

#if ENABLE_NETWORK
	DCC_LOG(LOG_TRACE, "network_config().");
	network_config();
#endif

	DCC_LOG(LOG_TRACE, "modules_init().");
	modules_init();

	printf("* Starting system module ...");
	DCC_LOG(LOG_TRACE, "sys_start().");
	sys_start();
	printf("ok\n");

	printf("* Initializing JTAG module ...");
	if (jtag_start() < 0) {
		printf("fail!\n");
		return 0;
	}
	printf("ok\n");

#if (ENABLE_NAND)
	printf("* Initializing NAND module...");
	if (mod_nand_start() < 0) {
		printf("fail!\n");
		return 0;
	}
	printf("ok\n");
#endif

#if (ENABLE_TFTP)
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

	__os_sleep(150);
	init_debugger();

#if ENABLE_USB
	printf("* starting USB shell ... ");
	usb_shell();
	printf("ok\n");
#endif

#if ENABLE_TELNET
	printf("* starting TELNET server ... ");
	telnet_shell();
	printf("ok\n");
#endif

	return console_shell();
}

