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
#include <ctype.h>

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
#include "i2c.h" 
#include "jtag3drv.h" 

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

#ifndef ENABLE_GDB
#define ENABLE_GDB 0
#endif

#ifndef ENABLE_I2C
#define ENABLE_I2C 0
#endif

#ifndef ENABLE_VCOM
#define ENABLE_VCOM 0
#endif

void tcpip_init(void);
void env_init(void);

const struct file stm32f_uart_file = {
	.data = STM32_UART5, 
	.op = &stm32f_usart_fops 
};

#define UART_TX STM32_GPIOC, 12
#define UART_RX STM32_GPIOD, 2

void stdio_init(void)
{
	struct stm32f_usart * uart = STM32_UART5;

	stm32_gpio_clock_en(STM32_GPIOC);
	stm32_gpio_clock_en(STM32_GPIOD);
	stm32_gpio_mode(UART_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(UART_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART_RX, GPIO_AF8);
	stm32_gpio_af(UART_TX, GPIO_AF8);

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stderr = (struct file *)&stm32f_uart_file;
	stdout = stderr;
	stdin = stdout;
}

void rtc_init(void)
{
	stm32f_rtc_init();
}

FILE * monitor_stream;
bool monitor_auto_flush;

void __attribute__((noreturn)) supervisor_task(void)
{
	int opt;

	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		thinkos_sleep(250);

		if (monitor_auto_flush)
			opt = TRACE_FLUSH;      
		else
			opt = 0;

		if (trace_fprint(monitor_stream, opt) < 0) {
			/* fall back to stdout */
			monitor_stream = stdout;
		}
	}
}

uint32_t supervisor_stack[512];

void supervisor_init(void)
{
	monitor_stream = stdout;

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));
}

int eth_strtomac(uint8_t ethaddr[], const char * s)
{
	char * cp = (char *)s;
	int i;
	char buf[4];
	long int val;

	while (isspace(*cp))
		cp++;

	for (i = 0; i < 6; ++i) {

		if (!isxdigit(*cp))
			break;

		buf[0] = *cp++;
		if (!isxdigit(*cp))
			break;
		buf[1] = *cp++;

		buf[2] = '\0';
		val = strtol(buf, NULL, 16);

		DCC_LOG1(LOG_TRACE, "val=%02x", val);

		ethaddr[i] = val;

		if (*cp == '\0')
			break;

		cp++;
	}

	return 0;
};

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
	/* TODO: random initial mac address */
	uint8_t ethaddr[6] = { 0x1c, 0x95, 0x5d, 0x00, 0x00, 0x80};
	int dhcp;

	DCC_LOG(LOG_TRACE, "tcpip_init().");
	tcpip_init();

	if ((env = getenv("IPCFG")) == NULL) {
		tracef("IPCFG not set, using defaults!\n");
		/* default configuration */
		strcpy(s, "192.168.0.128 255.255.255.0 192.168.0.1 0");
		/* set the default configuration */
		setenv("IPCFG", s, 1);
	} else {
		strcpy(s, env);
	}

	if ((env = getenv("ETHADDR")) != NULL) {
		eth_strtomac(ethaddr, env);
	} else {
		tracef("Ethernet MAC address not set, using defaults!");
		DCC_LOG(LOG_WARNING, "Ethernet MAC address not set.");
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
	tracef("* netif %s: %s, %s", s, 
		   inet_ntop(AF_INET, (void *)&ip_addr, s1, 16),
		   inet_ntop(AF_INET, (void *)&netmask, s2, 16));

	if (gw_addr != INADDR_ANY) {
		/* add the default route (gateway) to ethif */
		ipv4_route_add(INADDR_ANY, INADDR_ANY, gw_addr, ifn);
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

int init_target(void) 
{
	struct target_info * target;

	target = target_lookup(getenv("TARGET"));

	if (target == NULL) {
		target = target_first();
		trace("WARNING: invalid target.");
		tracef("Fallback to default: %s.", target->name);
	};

	trace("* target select...");

	/* TODO: ice driver selection */
	if (target_ice_configure(stdout, target, 1) < 0) {
		tracef("ERROR: target_ice_configure()!");
		return -1;
	}

	tracef("* target: %s [%s-%s-%s %s-%s-%s]", target->name, 
		   target->arch->name, target->arch->model, target->arch->vendor,
		   target->arch->cpu->family, target->arch->cpu->model, 
		   target->arch->cpu->vendor);

	if (target_config(stdout) < 0) {
		tracef("ERROR: target_config()!");
		return -1;
	}

	return 0;
}

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
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));
	trace_init();

	tracef("## YARD-ICE " VERSION_NUM " - " VERSION_DATE " ##");

	env_init();

	bsp_io_ini();

	rtc_init();

	supervisor_init();
	__os_sleep(10);

#if ENABLE_NETWORK
	DCC_LOG(LOG_TRACE, "network_config().");
	network_config();
#endif

	DCC_LOG(LOG_TRACE, "modules_init().");
	modules_init();

	tracef("* Starting system module ...");
	DCC_LOG(LOG_TRACE, "sys_start().");
	sys_start();

	tracef("* Initializing YARD-ICE debugger...");
	DCC_LOG(LOG_TRACE, "debugger_init().");
	debugger_init();

	tracef("* Initializing JTAG module ...");
	DCC_LOG(LOG_TRACE, "jtag_start().");
	if ((ret = jtag_start()) < 0) {
		tracef("jtag_start() failed! [ret=%d]", ret);
		debugger_except("JTAG driver fault");
	}

#if (ENABLE_NAND)
	tracef("* Initializing NAND module...");
	if (mod_nand_start() < 0) {
		tracef("mod_nand_start() failed!");
		return 0;
	}
#endif

#if (ENABLE_I2C)
	tracef("* starting I2C module ... ");
	i2c_init();
#endif

	tracef("* configuring initial target ... ");
	init_target();

#if (ENABLE_VCOM)
	tracef("* starting VCOM daemon ... ");
	/* connect the UART to the JTAG auxiliary pins */
	jtag3ctrl_aux_uart(true);
	vcom_start();
#endif

#if ENABLE_USB
	tracef("* starting USB shell ... ");
	usb_shell();
#endif

#if ENABLE_TELNET
	tracef("* starting TELNET server ... ");
	telnet_shell();
#endif

#if (ENABLE_COMM)
	tracef("* starting COMM daemon ... ");
	comm_tcp_start(&debugger.comm);
#endif

#if (ENABLE_TFTP)
	tracef("* starting TFTP server ... ");
	tftpd_start();
#endif

#if (ENABLE_GDB)
	tracef("* starting GDB daemon ... ");
	gdb_rspd_start();
#endif


//	__os_sleep(500);

	return console_shell();
}

