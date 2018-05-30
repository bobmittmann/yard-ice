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
#ifndef THINKAPP
#define __THINKOS_DBGMON__
#include <thinkos/dbgmon.h>
#endif

#include <tcpip/net.h>
#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/loopif.h>
#include <tcpip/arp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/tty.h>

#include <jtag.h>
#include <trace.h>
#include <ctype.h>

#include <yard-ice/drv.h>

#include "board.h"
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

#ifndef ENABLE_MONITOR
#define ENABLE_MONITOR 0
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

const struct file stm32_uart_file = {
	.data = STM32_UART5, 
	.op = &stm32_usart_fops 
};

void stdio_init(void)
{
	struct serial_dev * ser;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	ser = stm32f_uart5_serial_init(115200, SERIAL_8N1);
	f_raw = serial_fopen(ser);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = (struct file *)&stm32_uart_file;
	stdout = f_tty;
	stdin = f_tty;
}

void rtc_init(void)
{
//	stm32f_rtc_init();
}

/* -------------------------------------------------------------------------
 * System Supervision
 * ------------------------------------------------------------------------- */

FILE * volatile spv_fout;
volatile bool spv_auto_flush = false;

void trace_output_set(FILE * f, bool flush)
{
	WARN("trace output set to 0x%08x", (uint32_t)f);

	spv_fout = f;
	spv_auto_flush = flush;
}

void __attribute__((noreturn)) supervisor_task(void)
{
	struct trace_entry trace;
	bool eth_link_up = false;
	uint32_t eth_tmo;
	uint32_t clk;

	DCC_LOG(LOG_TRACE, "1.");
	INF("<%d> started...", thinkos_thread_self());

	DCC_LOG(LOG_TRACE, "2.");
	trace_tail(&trace);

	DCC_LOG(LOG_TRACE, "3.");
	clk = thinkos_clock();
	eth_tmo = clk + 100;
	for (;;) {
		struct timeval tv;
		char s[64];

		/* 8Hz periodic task */
		clk += 125;
		thinkos_alarm(clk);

		while (trace_getnext(&trace, s, sizeof(s)) >= 0) {
			trace_ts2timeval(&tv, trace.dt);
			if (trace.ref->lvl <= TRACE_LVL_WARN)
				fprintf(spv_fout, "%s %2d.%06d: %s,%d: %s\n",
						trace_lvl_nm[trace.ref->lvl],
						(int)tv.tv_sec, (int)tv.tv_usec,
						trace.ref->func, trace.ref->line, s);
			else
				fprintf(spv_fout, "%s %2d.%06d: %s\n",
						trace_lvl_nm[trace.ref->lvl],
						(int)tv.tv_sec, (int)tv.tv_usec, s);
		}

		if (spv_auto_flush)
			trace_flush(&trace);

		if ((int32_t)(clk - eth_tmo) >= 0) {
			bool up = ethif_link_up();
			if (up) {
				if (!eth_link_up) {
					struct route * rt;
					INF("Ethernet link UP!");
					if ((rt = ipv4_route_get(INADDR_ANY, INADDR_ANY)) != NULL) {
						/* send a gratuitous ARP request to the default gateway */
						ipv4_arp_query(rt->rt_gateway);
					}
				}
				eth_tmo += 250;
			} else {
				if (eth_link_up) {
					WARN("Ethernet link DOWN!");
				}
				eth_tmo += 1000;
			}
			eth_link_up = up;
		}
	}
}


uint32_t supervisor_stack[128];

const struct thinkos_thread_inf supervisor_inf = {
	.stack_ptr = supervisor_stack,
	.stack_size = sizeof(supervisor_stack),
	.priority = 32,
	.thread_id = 18,
	.paused = false,
	.tag = "SUPV"
};

void supervisor_init(void)
{
	thinkos_thread_create_inf((void *)supervisor_task, (void *)NULL,
							  &supervisor_inf);

	DCC_LOG(LOG_TRACE, "thinkos_sleep()...");
	thinkos_sleep(1);
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
int stm32f_get_esn(void * arg)
{
	uint64_t * esn = (uint64_t *)arg;
	*esn = *((uint64_t *)STM32F_UID);
	return 0;
}

void on_dhcpc_event(struct dhcp * dhcp, enum dhcp_event event)
{
	in_addr_t ip_addr;
	in_addr_t netmask;
//	in_addr_t gw_addr;
	struct ifnet * ifn;
	char s[16];
	char s1[16];
	char s2[16];

	dhcp_ipv4_bind(dhcp);
	ifn = dhcp_ifget(dhcp);

	ifn_getname(ifn, s);
	ifn_ipv4_get(ifn, &ip_addr, &netmask);
	INF("* netif %s: %s, %s", s,
		   inet_ntop(AF_INET, (void *)&ip_addr, s1, 16),
		   inet_ntop(AF_INET, (void *)&netmask, s2, 16));
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
	char s3[16];
	char * env;
	uint8_t * ethaddr;
	uint64_t esn;
	int use_dhcp = 0;
	struct dhcp * dhcp;

	/* Initialize MAC address with the MCU's UID */
	ethaddr = (uint8_t *)&esn;
	/* The UID register is located in a system area not mapped
	   in the MPU user region. We need to escalate the privilege to 
	   have access to this area. */
//	thinkos_escalate(stm32f_get_esn, &esn);
	stm32f_get_esn(&esn);
	DCC_LOG2(LOG_TRACE, "ESN=0x%08x%08x", esn >> 32, esn);
	ethaddr[0] = (ethaddr[0] & 0xfc) | 0x02; /* Locally administered MAC */


	DCC_LOG(LOG_TRACE, "tcpip_init().");
	tcpip_init();

	if ((env = getenv("IPCFG")) == NULL) {
		INF("IPCFG not set, using defaults!\n");
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
		INF("Ethernet MAC address not set, using defaults!");
		DCC_LOG(LOG_WARNING, "Ethernet MAC address not set.");
	}

    INF("* mac addr: %02x-%02x-%02x-%02x-%02x-%02x",
		   ethaddr[0], ethaddr[1], ethaddr[2],
		   ethaddr[3], ethaddr[4], ethaddr[5]);

	if (!inet_aton(strtok(s, " ,"), (struct in_addr *)&ip_addr)) {
		DCC_LOG(LOG_WARNING, "inet_aton() failed.");
		return -1;
	}

	if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&netmask)) {
		if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&gw_addr)) {
			use_dhcp = strtoul(strtok(NULL, ""), NULL, 0);
		}
	}

	/* initialize the Ethernet interface */
	/* configure the ip address */
	ifn = ethif_init(ethaddr, ip_addr, netmask);

	ifn_getname(ifn, s);
	ifn_ipv4_get(ifn, &ip_addr, &netmask);
	INF("* netif %s: %s, %s", s, 
		   inet_ntop(AF_INET, (void *)&ip_addr, s1, 16),
		   inet_ntop(AF_INET, (void *)&netmask, s2, 16));
	(void)s1;
	(void)s2;

	if (gw_addr != INADDR_ANY) {
		/* add the default route (gateway) to ethif */
		ipv4_route_add(INADDR_ANY, INADDR_ANY, gw_addr, ifn);
		INF("* default route gw: %s", 
			   inet_ntop(AF_INET, (void *)&gw_addr, s1, 16));
	}

	if ((env = getenv("ROUTE")) != NULL) {
		strcpy(s, env);
		if (inet_aton(strtok(s, " ,"), (struct in_addr *)&ip_addr)) {
			if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&netmask)) {
				if (inet_aton(strtok(NULL, " ;"), (struct in_addr *)&gw_addr)) {
					ipv4_route_add(ip_addr, netmask, gw_addr, ifn);
					INF("* route: %s %s %s",
						inet_ntop(AF_INET, (void *)&ip_addr, s1, 16),
						inet_ntop(AF_INET, (void *)&netmask, s2, 16),
						inet_ntop(AF_INET, (void *)&gw_addr, s3, 16));
				}
			}
		}
	}

	/* enable the interface to be configured through DHCP */
	dhcp = dhcpc_ifattach(ifn, on_dhcpc_event);
	if (use_dhcp) {
		/* schedule the interface to be configured through DHCP */
		dhcp_resume(dhcp);
		INF("DHCP started.");
	}


//	loopif_init();

	return 0;
}
#endif

int init_target(void) 
{
	struct target_info * target;

	target = target_lookup(getenv("TARGET"));

	if (target == NULL) {
		target = target_first();
		INF("WARNING: invalid target.");
		INF("Fallback to default: %s.", target->name);
	};

	INF("* target select...");
	DCC_LOG1(LOG_TRACE, "target='%s'", target->name);

	/* TODO: ice driver selection */
	if (target_ice_configure(stdout, target, 1) < 0) {
		INF("ERROR: target_ice_configure()!");
		DCC_LOG(LOG_WARNING, "target_ice_configure() failed!");
		return -1;
	}

	INF("* target: %s [%s-%s-%s %s-%s-%s]", target->name, 
		   target->arch->name, target->arch->model, target->arch->vendor,
		   target->arch->cpu->family, target->arch->cpu->model, 
		   target->arch->cpu->vendor);

	if (target_config(stdout) < 0) {
		INF("ERROR: target_config()!");
		DCC_LOG(LOG_WARNING, "target_config() failed!");
		return -1;
	}

	return 0;
}

#ifndef THINKAPP
void board_init(void);
void monitor_task(struct dmon_comm * comm);

void monitor_init(void)
{
	struct dmon_comm * comm;

	thinkos_console_init();

	comm = usb_comm_init(&stm32f_otg_fs_dev);

	thinkos_dbgmon_init(comm, monitor_task);
}
#endif

void io_init(void);
int stdio_shell(void);
FILE * console_shell(void);
int telnet_shell(void);
int sys_start(void);

int main(int argc, char ** argv)
{
	int ret;
	io_init();

#ifdef THINKAPP
	DCC_LOG(LOG_TRACE, " 1. thinkos_udelay_factor().");
	thinkos_udelay_factor(&udelay_factor);
#else
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

#ifndef UDELAY_FACTOR 
	DCC_LOG(LOG_TRACE, " 1. cm3_udelay_calibrate().");
	cm3_udelay_calibrate();
#endif

	DCC_LOG(LOG_TRACE, " 2. this_board.init().");
	board_init();

	DCC_LOG(LOG_TRACE, " 3. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

#if ENABLE_MONITOR
	DCC_LOG(LOG_TRACE, " 4. monitor_init().");
	monitor_init();
#endif

#endif

	DCC_LOG(LOG_TRACE, " 5. stdio_init().");
	stdio_init();

	printf("\n---\n");
	/* set monitor to stderr */
	spv_fout = stderr;

	DCC_LOG(LOG_TRACE, " 6. trace_init().");
	trace_init();

	INF("## YARD-ICE " VERSION_NUM " - " VERSION_DATE " ##");

	DCC_LOG(LOG_TRACE, " 7. supervisor_init().");
	supervisor_init();

	DCC_LOG(LOG_TRACE, " 8. stm32f_nvram_env_init().");
	stm32f_nvram_env_init();

	DCC_LOG(LOG_TRACE, " 9. rtc_init().");
	rtc_init();

	DCC_LOG(LOG_TRACE, " 10. modules_init().");
	modules_init();

	INF("* Starting system module ...");
	DCC_LOG(LOG_TRACE, " 11. sys_start().");
	sys_start();

	INF("* Initializing YARD-ICE debugger...");
	DCC_LOG(LOG_TRACE, " 12. debugger_init().");
	debugger_init();

	INF("* Initializing JTAG module ...");
	DCC_LOG(LOG_TRACE, " 13. jtag_start().");
	if ((ret = jtag_start()) < 0) {
		INF("jtag_start() failed! [ret=%d]", ret);
		debugger_except("JTAG driver fault");
	}

#if (ENABLE_NAND)
	INF("* Initializing NAND module...");
	DCC_LOG(LOG_TRACE, " 14. mod_nand_start().");
	if (mod_nand_start() < 0) {
		INF("mod_nand_start() failed!");
		return 1;
	}
#endif

#if (ENABLE_I2C)
	INF("* starting I2C module ... ");
	DCC_LOG(LOG_TRACE, "15. i2c_init().");
	i2c_init();
#endif

#if ENABLE_NETWORK
	DCC_LOG(LOG_TRACE, " 17. network_config().");
	INF("* Initializing network...");
	network_config();
#endif

#if (ENABLE_VCOM)
	INF("* starting VCOM daemon ... ");
	/* connect the UART to the JTAG auxiliary pins */
	jtag3ctrl_aux_uart(true);
	DCC_LOG(LOG_TRACE, "18. vcom_start().");
	vcom_start();
#endif

#if (ENABLE_COMM)
	INF("* starting COMM daemon ... ");
	DCC_LOG(LOG_TRACE, "19. comm_tcp_start().");
	comm_tcp_start(&debugger.comm);
#endif

#if (ENABLE_TFTP)
	INF("* starting TFTP server ... ");
	DCC_LOG(LOG_TRACE, "20. tftpd_start().");
	tftpd_start();
#endif

#if (ENABLE_GDB)
	INF("* starting GDB daemon ... ");
	DCC_LOG(LOG_TRACE, "21. gdb_rspd_start().");
	gdb_rspd_start();
#endif

#if ENABLE_MONITOR
	INF("* starting console shell ... ");
	DCC_LOG(LOG_TRACE, "22. console_shell().");
	{
		FILE * f;
		char * env;

		f = console_shell();
		if ((env = getenv("TRACE")) != NULL) {
			INF("TRACE='%s'", env);
			if (strcmp(env, "console") == 0)
				trace_output_set(f, true);
		} else
			INFS("TRACE environment not set.");
	}
#endif

#if ENABLE_TELNET
	INF("* starting TELNET server ... ");
	DCC_LOG(LOG_TRACE, "24. telnet_shell().");
	telnet_shell();
#endif

	INF("* configuring initial target ... ");
	DCC_LOG(LOG_TRACE, "16. init_target().");
	init_target();

	for (;;) {
		thinkos_sleep(250);

		DCC_LOG(LOG_TRACE, "25. stdio_shell().");
		stdio_shell();
	}

	return 0;
}

