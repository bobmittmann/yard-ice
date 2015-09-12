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
#include <thinkos.h>
#include <tcpip/net.h>

#include <trace.h>

#include "board.h"

#define VERSION_NUM "0.3"
#define VERSION_DATE "Mar, 2015"

const char * version_str = "HTTP Server Demo " \
							VERSION_NUM " - " VERSION_DATE;
const char * copyright_str = "(c) Copyright 2015 - Bob Mittmann";

void stdio_init(void);
int stdio_shell(void);
int tcp_echo_start(void);
int tcp_qotd_start(void);
int tftpd_start(void);

extern struct httpdobj www_root[];
extern struct httpdobj www_img[];
extern struct httpdobj www_lib[];
extern struct httpdobj www_cgi[];

struct httpddir httpd_dir[] = {
	{ .path = "/", .objlst = www_root },
	{ .path = "/img", .objlst = www_img },
	{ .path = "/cgi", .objlst = www_cgi },
	{ .path = "/lib", .objlst = www_lib },
	{ .path = NULL, .objlst = NULL }
};

int httpd_server_task(struct httpd * httpd)
{
	struct httpctl httpctl;
	struct httpctl * ctl = &httpctl;
	const struct httpdobj * obj;
	unsigned int cnt = 0;
	unsigned int id = thinkos_thread_self();
	(void)id;

	for (;;) {
		INF("<%d> Wating for connection...", thinkos_thread_self());

		if (http_accept(httpd, ctl) < 0) {
			printf("tcp_accept() failed!\n");
			thinkos_sleep(100);
			continue;
		}

		INF("<%d> Connection accepted...", thinkos_thread_self());
		if ((obj = http_obj_lookup(ctl)) != NULL) {
			cnt++;
			switch (ctl->method) {
			case HTTP_GET:
				DBG("%2d %6d GET \"%s\"", id, cnt, obj->oid);
				http_get(ctl, obj);
				break;
			case HTTP_POST:
				DBG("%2d %6d POST \"%s\"", id, cnt, obj->oid);
				http_post(ctl, obj);
				break;
			}
		}

		http_close(ctl);
	}

	return 0;
}

void __attribute__((noreturn)) supervisor_task(void)
{
	struct trace_entry ent;
	struct timeval tv;
	char s[80];

	INF("Supervisory task started (thread %d).", thinkos_thread_self());

	trace_tail(&ent);

	for (;;) {
		thinkos_sleep(250);

		/* Flush the trace ring */
		while (trace_getnext(&ent, s, sizeof(s)) >= 0) {
			trace_ts2timeval(&tv, ent.dt);
			printf("%s %2d.%06d: %s\n", trace_lvl_nm[ent.ref->lvl],
					(int)tv.tv_sec, (int)tv.tv_usec, s);
		}

		trace_flush(&ent);
	}
}


uint32_t supervisor_stack[128];

const struct thinkos_thread_inf supervisor_inf = {
	.stack_ptr = supervisor_stack,
	.stack_size = sizeof(supervisor_stack),
	.priority = 1,
	.thread_id = 1,
	.paused = false,
	.tag = "SUPV"
};

void supervisor_init(void)
{
	trace_init();

	thinkos_thread_create_inf((void *)supervisor_task, (void *)NULL,
							  &supervisor_inf);
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
	int dhcp = 1;

	tcpip_init();

	esn = *((uint64_t *)STM32F_UID);
	INF("ESN=0x%08x%08x", esn >> 32, esn);

	ethaddr[0] = ((esn >>  0) & 0xfc) | 0x02; /* Locally administered MAC */
	ethaddr[1] = ((esn >>  8) & 0xff);
	ethaddr[2] = ((esn >> 16) & 0xff);
	ethaddr[3] = ((esn >> 24) & 0xff);
	ethaddr[4] = ((esn >> 32) & 0xff);
	ethaddr[5] = ((esn >> 40) & 0xff);

	printf("* mac addr: %02x-%02x-%02x-%02x-%02x-%02x\n ",
		   ethaddr[0], ethaddr[1], ethaddr[2],
		   ethaddr[3], ethaddr[4], ethaddr[5]);


//	if ((env = getenv("IPCFG")) == NULL) {
	if (1) {
		printf("IPCFG not set, using defaults!\n");
		/* default configuration */
		strcpy(s, "192.168.10.128 255.255.255.0 192.168.10.254 0");
//		strcpy(s, "0.0.0.0 0.0.0.0 0.0.0.0 1");
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
		/* start the DHCP client daemon */
		dhcpc_start();
		/* schedule the interface to be configured through DHCP */
		dhcpc_ifconfig(ifn, NULL);
	}

	loopif_init();

	return 0;
}

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	/* USART5 TX */
	stm32_gpio_mode(UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART5_TX, GPIO_AF8);
	/* USART5 RX */
	stm32_gpio_mode(UART5_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART5_RX, GPIO_AF8);

	/* USART6_TX */
	stm32_gpio_mode(UART6_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART6_TX, GPIO_AF7);
	/* USART6_RX */
 	stm32_gpio_mode(UART6_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART6_RX, GPIO_AF7);

}

uint32_t httpd1_stack[384];
uint32_t httpd2_stack[384];
uint32_t httpd3_stack[384];
uint32_t httpd4_stack[384];


const struct thinkos_thread_inf httpd1_inf = {
	.stack_ptr = httpd1_stack,
	.stack_size = sizeof(httpd1_stack),
	.priority = 32,
	.thread_id = 4, 
	.paused = 0,
	.tag = "HTTPD 1"
};

const struct thinkos_thread_inf httpd2_inf = {
	.stack_ptr = httpd2_stack,
	.stack_size = sizeof(httpd2_stack),
	.priority = 32,
	.thread_id = 5,
	.paused = 0,
	.tag = "HTTPD 2"
};

const struct thinkos_thread_inf httpd3_inf = {
	.stack_ptr = httpd3_stack,
	.stack_size = sizeof(httpd3_stack),
	.priority = 32,
	.thread_id = 6,
	.paused = 0,
	.tag = "HTTPD 3"
};

const struct thinkos_thread_inf httpd4_inf = {
	.stack_ptr = httpd4_stack,
	.stack_size = sizeof(httpd4_stack),
	.priority = 32,
	.thread_id = 7,
	.paused = 0,
	.tag = "HTTPD 4"
};

int main(int argc, char ** argv)
{
	struct httpd *  httpd;
	int port = 80;

	thinkos_udelay_factor(&udelay_factor);

	stm32f_nvram_env_init();

	io_init();

	stdio_init();

	printf("1. supervisor_init()... \n");
	supervisor_init();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS (HTTPD Test)\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	network_config();

	httpd = httpd_alloc();
	httpd_init(httpd, port, 4, httpd_dir, NULL);
	printf("Listening on port %d.\n", port);

	thinkos_thread_create_inf((void *)httpd_server_task, (void *)httpd,
							  &httpd1_inf);
#if 1
	thinkos_thread_create_inf((void *)httpd_server_task, (void *)httpd,
							  &httpd2_inf);

	thinkos_thread_create_inf((void *)httpd_server_task, (void *)httpd,
							  &httpd3_inf);

	thinkos_thread_create_inf((void *)httpd_server_task, (void *)httpd,
							  &httpd4_inf);
#endif

#if 1
	tcp_echo_start();

	tcp_qotd_start();

	tftpd_start();
#endif

	for (;;) {
		stdio_shell();
		thinkos_sleep(1000);
	}

	return 0;
}


