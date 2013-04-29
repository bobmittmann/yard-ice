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
#include <sys/tty.h>

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

#ifndef ENABLE_COMM
#define ENABLE_COMM 0
#endif

void tcpip_init(void);
void env_init(void);

const struct file stm32f_uart_file = {
	.data = STM32F_UART5, 
	.op = &stm32f_usart_fops 
};

#define UART_TX STM32F_GPIOC, 12
#define UART_RX STM32F_GPIOD, 2

void stdio_init(void)
{
	struct stm32f_usart * uart = STM32F_UART5;

	stm32f_gpio_clock_en(STM32F_GPIOC);
	stm32f_gpio_clock_en(STM32F_GPIOD);
	stm32f_gpio_mode(UART_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(UART_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(UART_RX, GPIO_AF8);
	stm32f_gpio_af(UART_TX, GPIO_AF8);

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stderr = (struct file *)&stm32f_uart_file;
	stdout = stderr;
	stdin = stdout;
}

int supervisor_task(void)
{
	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		thinkos_sleep(250);
		trace_fprint(stdout, TRACE_FLUSH);
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
	char s1[16];
	char s2[16];
	char * env;
	int dhcp;

	DCC_LOG(LOG_TRACE, "tcpip_init().");
	tcpip_init();

	if ((env = getenv("IPCFG")) == NULL) {
		tracef("IPCFG not set, using defaults!\n");
		/* default configuration */
		strcpy(s, "192.168.0.80 255.255.255.0 192.168.0.1 0");
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
	ifn = ethif_init(ip_addr, netmask);
//	ifn = loopif_init(ip_addr, netmask);

	ifn_getname(ifn, s);
	ifn_ipv4_get(ifn, &ip_addr, &netmask);
	tracef("* netif %s: %s, %s", s, 
		   inet_ntop(AF_INET, (void *)&ip_addr, s1, 16),
		   inet_ntop(AF_INET, (void *)&netmask, s2, 16));

	if (gw_addr != INADDR_ANY) {
		/* add the default route (gateway) to ethif */
		route_add(INADDR_ANY, INADDR_ANY, gw_addr, ifn);
		tracef("* default route gw: %s", 
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
#endif

void debugger_init(void);

int init_debugger(void) 
{
	struct target_info * target;

	debugger_init();

	target = target_lookup(getenv("TARGET"));

	if (target == NULL) {
		target = target_first();
		trace("WARNING: invalid target.");
		tracef("Fallback to default: %s.", target->name);
	};

	trace("* target select...");

	/* TODO: ice driver selection */
	if (target_configure(stdout, target, 1) < 0) {
		tracef("ERROR: target_configure()!");
		return 0;
	}

	tracef("* target: %s [%s-%s-%s %s-%s-%s]", target->name, 
		   target->arch->name, target->arch->model, target->arch->vendor,
		   target->arch->cpu->family, target->arch->cpu->model, 
		   target->arch->cpu->vendor);

	return 0;
}

uint32_t supervisor_stack[256];

int console_shell(void);
void telnet_shell(void);
void usb_shell(void);
int sys_start(void);

int main(int argc, char ** argv)
{
	int ret;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	stdio_init();
	printf("\n---\n");

	cm3_udelay_calibrate();
	printf(".1");
	trace_init();
	printf(".2");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));
	printf(".3");
	env_init();
	printf(".4");

	bsp_io_ini();

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));
	printf(".5");

#if ENABLE_NETWORK
	DCC_LOG(LOG_TRACE, "network_config().");
	network_config();
#endif

	printf(".6");

	DCC_LOG(LOG_TRACE, "modules_init().");
	modules_init();

	printf(".7");

	tracef("* Starting system module ...");
	DCC_LOG(LOG_TRACE, "sys_start().");
	sys_start();

	printf(".8");

	tracef("* Initializing JTAG module ...");
	if ((ret = jtag_start()) < 0) {
		printf("[%d]", ret);
		tracef("jtag_start() failed!");
		__os_sleep(1000);
	}

	printf(".9");

#if (ENABLE_NAND)
	tracef("* Initializing NAND module...");
	if (mod_nand_start() < 0) {
		tracef("mod_nand_start() failed!");
		return 0;
	}
#endif

#if (ENABLE_TFTP)
	tracef("* starting TFTP server ... ");
	tftpd_start();
#endif

#ifdef ENABLE_GDB
	tracef("* starting GDB daemon ... ");
	gdb_rspd_start();
#endif

#if (ENABLE_COMM)
	tracef("* starting COMM daemon ... ");
	comm_tcp_start(&debugger.comm);
#endif

#ifdef ENABLE_VCOM
	tracef("* starting VCOM daemon ... ");
	vcom_start();
#endif

	init_debugger();

#if ENABLE_USB
	tracef("* starting USB shell ... ");
	usb_shell();
#endif

#if ENABLE_TELNET
	tracef("* starting TELNET server ... ");
	telnet_shell();
#endif

	return console_shell();
}

