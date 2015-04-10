/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libtcpip.
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
 * @file ifn_input.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>
#define __USE_SYS_ROUTE__
#include <sys/etharp.h>
#include <sys/route.h>
#include <sys/net.h>
#include <sys/ip.h>

#include <tcpip/ifnet.h>

#include <stdlib.h>

const uint8_t ifnet_max = IFNET_INTERFACES_MAX;

struct ifnet_system __ifnet__;

void __attribute__((noreturn)) ifnet_input_task(void * arg)
{
	struct ifnet * ifn = NULL;
	unsigned int proto;
	uint8_t * pkt; 
	uint8_t * src; 
	int idx;
	int len;

	for (;;) {
		DCC_LOG(LOG_INFO, "wait...");
		/* wait for an event form a network interface */
		idx = thinkos_ev_wait(__ifnet__.evset);
		DCC_LOG1(LOG_INFO, "idx=%d", idx);

		/* lookup the interface */
		ifn = &__ifnet__.ifn[idx];

		/* get the packet from the network interface */
		while ((len = ifn_pkt_recv(ifn, &src, &proto, &pkt)) > 0) {
			tcpip_net_lock();

			NETIF_STAT_ADD(ifn, rx_pkt, 1);
			if (proto == IFN_PROTO_IP) {
				DCC_LOG(LOG_INFO, "IP");
				ip_input(ifn, (struct iphdr *)pkt, len);
			} else {
				if (proto == IFN_PROTO_ETHARP) {
					DCC_LOG(LOG_INFO, "ARP");
					etharp_input(ifn, (struct etharp*)pkt, len);
				} else {
					NETIF_STAT_ADD(ifn, rx_drop, 1);
					DCC_LOG(LOG_INFO, "unhandled protocol");
				}
			}

			tcpip_net_unlock();

			ifn_pkt_free(ifn, pkt);
		}
	}
}

uint32_t ifnet_stack[128];

const struct thinkos_thread_inf ifnet_input_inf = {
	.stack_ptr = ifnet_stack, 
	.stack_size = sizeof(ifnet_stack), 
	.priority = 32,
	.thread_id = 32, 
	.paused = 0,
	.tag = "NET_RCV"
};

int ifnet_init(void)
{
	__ifnet__.evset = thinkos_ev_alloc();

	DCC_LOG(LOG_TRACE, "thinkos_thread_create_inf()");
	thinkos_thread_create_inf((void *)ifnet_input_task, NULL, &ifnet_input_inf);

	return 0;
}

