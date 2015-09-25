/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/delay.h>
#include <sys/console.h>
#include <sys/tty.h>
#include <sys/null.h>
#include <trace.h>

#include <thinkos.h>

#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/loopif.h>
#include <tcpip/udp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#include "simlnk.h"
#include "simrpc.h"
#include "board.h"
#include "net.h"
#include "io.h"

#define VERSION_NUM "0.3"
#define VERSION_DATE "Nov, 2014"

const char * version_str = "SLC Device Hub " \
							VERSION_NUM " - " VERSION_DATE;
const char * copyright_str = "(c) Copyright 2014 - Mircom Group";


int stdio_shell(void);
void tcpip_init(void);
int webserver_start(void);

/* -------------------------------------------------------------------------
 * System Supervision
 * ------------------------------------------------------------------------- */

const char * const trace_lvl_tab[] = {
		"   NONE",
		"  ERROR",
		"WARNING",
		"   INFO",
		"  DEBUG"
};

FILE * trace_file;
bool trace_auto_flush = true;
const char trace_host[] = "192.168.10.254";
const uint16_t trace_port = 11111;

void __attribute__((noreturn)) supervisor_task(void)
{
	struct trace_entry trace;
    struct sockaddr_in sin;
    struct udp_pcb * udp;
	uint32_t clk;
	int n;

	INF("<%d> started...", thinkos_thread_self());

	/* set the supervisor stream to stdout */
	trace_file = stdout;
	/* enable auto flush */
	trace_auto_flush = true;

	trace_tail(&trace);

    if ((udp = udp_alloc()) == NULL) {
        abort();
    }

	if (!inet_aton(trace_host, &sin.sin_addr)) {
		abort();
	}
	sin.sin_port = htons(trace_port);

    if (udp_bind(udp, INADDR_ANY, htons(10)) < 0) {
        abort();
    }

//    if (udp_connect(udp, sin.sin_addr.s_addr, htons(sin.sin_port)) < 0) {
//		abort();
//    }

	clk = thinkos_clock();
	for (;;) {
		struct timeval tv;
		char msg[80];
		char s[128];

		/* 8Hz periodic task */
		clk += 125;
		thinkos_alarm(clk);

		while (trace_getnext(&trace, msg, sizeof(msg)) >= 0) {
			trace_ts2timeval(&tv, trace.dt);
			if (trace.ref->lvl <= TRACE_LVL_WARN)
				n = sprintf(s, "%s %2d.%06d: %s,%d: %s\n",
						trace_lvl_nm[trace.ref->lvl],
						(int)tv.tv_sec, (int)tv.tv_usec,
						trace.ref->func, trace.ref->line, msg);
			else
				n = sprintf(s, "%s %2d.%06d: %s\n",
						trace_lvl_nm[trace.ref->lvl],
						(int)tv.tv_sec, (int)tv.tv_usec, msg);

			/* sent log to remote station */
            udp_sendto(udp, s, n, &sin);

            /* write log to local console */
			fwrite(s, n, 1, trace_file);
		}

		if (trace_auto_flush)
			trace_flush(&trace);
	}
}

uint32_t supervisor_stack[256];

const struct thinkos_thread_inf supervisor_inf = {
	.stack_ptr = supervisor_stack,
	.stack_size = sizeof(supervisor_stack),
	.priority = 32,
	.thread_id = 31,
	.paused = false,
	.tag = "SUPV"
};

void supervisor_init(void)
{
	trace_init();

	thinkos_thread_create_inf((void *)supervisor_task, (void *)NULL,
							  &supervisor_inf);
}

/* -------------------------------------------------------------------------
 * Stdio
 * ------------------------------------------------------------------------- */

void __attribute__((noreturn)) stdio_shell_task(void * arg)
{
	DBG("<%d> started...", thinkos_thread_self());
	
	for (;;) {
		printf("\n...\n");
		stdio_shell();
	}
}

uint32_t stdio_shell_stack[1024 + 256];

const struct thinkos_thread_inf stdio_shell_inf = {
	.stack_ptr = stdio_shell_stack,
	.stack_size = sizeof(stdio_shell_stack),
	.priority = 32,
	.thread_id = 31,
	.paused = false,
	.tag = "SHELL"
};

void stdio_init(void)
{
	FILE * f;

	f = console_fopen();
//	f = null_fopen(NULL);

	/* initialize STDIO */
	stderr = f;
	stdout = f;
	stdin = f;

//	thinkos_thread_create_inf((void *)stdio_shell_task, (void *)NULL,
//			&stdio_shell_inf);
}

/* -------------------------------------------------------------------------
 * TCP/IP network
 * ------------------------------------------------------------------------- */

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

void remote_test(unsigned int daddr)
{
	struct simrpc_pcb * sp;
	char s[128];

	if ((sp = simrpc_open(daddr)) == NULL) {
		return;
	}

	if (simrpc_suspend(sp) < 0) {
		WARN("simrpc_suspend() failed!");
	}

	thinkos_sleep(500);

	//		DCC_LOG(LOG_TRACE, "11. simrpc_mem_lock()...");
	simrpc_mem_lock(sp, 0x08010000, 8192);

	if (simrpc_mem_erase(sp, 0, 2048) < 0) {
		WARN("simrpc_mem_erase() failed!");
	}

	if (simrpc_mem_write(sp, "Hello world!", 14) < 0) {
		WARN("simrpc_mem_write() failed!");
	}

	if (simrpc_mem_seek(sp, 0) < 0) {
		WARN("simrpc_mem_seek() failed!");
	}

	if (simrpc_mem_read(sp, s, 14) < 0) {
		WARN("simrpc_mem_write() failed!");
	}

	if (simrpc_mem_unlock(sp, 0x08010000, 8192) < 0) {
		WARN("simrpc_mem_unlock() failed!");
	}

	thinkos_sleep(500);

	if (simrpc_resume(sp) < 0) {
		WARN("simrpc_resume() failed!");
	}

	simrpc_close(sp);
}

void flash_test(void);

int main(int argc, char ** argv)
{
	int i;

	thinkos_udelay_factor(&udelay_factor);

	stm32f_nvram_env_init();

	iodrv_init();

	stdio_init();

	trace_init();

	supervisor_init();

	led_set_rate(LED_X1, RATE_2BPS);

	network_config();

	INF("7. Starting webserver...");
	webserver_start();

	INF("Starting SLCDEV simulator test");

	simrpc_init();

	for (;;) {
		thinkos_sleep(60000);
		INF("Tick %d ...", ++i);
	}

//	flash_test();

	for (i = 0; i < 10000; ++i) {
		remote_test(1);
		remote_test(2);
		remote_test(3);
		remote_test(4);
		remote_test(5);
		thinkos_sleep(1000);
	}

	for (i = 0; i < 100000000; ++i) {
//		DCC_LOG(LOG_TRACE, "11. simlink_send...");
//		simlnk_send(simlnk[0], "Hello world\n", 12); 
//		simlnk_send(simlnk[1], "Hello world\n", 12); 
//		simlnk_send(simlnk[2], "Hello world\n", 12); 
//		simlnk_send(simlnk[3], "Hello world\n", 12); 
//		simlnk_send(simlnk[4], "Hello world\n", 12); 
//		thinkos_sleep(1000);
		simrpc_mem_lock(0, 0x08000000, 1024);
	}

//	net_init();

	return 0;
}

