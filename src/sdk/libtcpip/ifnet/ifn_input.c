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
#include <stdlib.h>

#include <tcpip/ifnet.h>

#include <trace.h>
#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_NONE 

#if (THINKOS_EVENT_MAX == 0)
#error "THINKOS_EVENT_MAX == 0!"
#endif 

const uint8_t ifnet_max = IFNET_INTERFACES_MAX;

struct ifnet_system __ifnet__;

void __attribute__((noreturn)) ifnet_input_task(void * arg)
{
	struct ifnet * ifn = NULL;
	unsigned int proto;
	unsigned int idx;
	uint8_t * pkt; 
	uint8_t * src; 
	int ret;
	int len;

	for (;;) {
		DCC_LOG(LOG_INFO, "wait...");
		/* wait for an event form a network interface */
		idx = thinkos_ev_wait(__ifnet__.evset);
#if 0
		if (idx < 0) {
			DCC_LOG1(LOG_ERROR, "thinkos_ev_wait() failed: %d", idx);
			abort();
		} else if (idx > IFNET_INTERFACES_MAX) {
			DCC_LOG1(LOG_ERROR, "thinkos_ev_wait() invalid event: %d", idx);
			abort();
		}
#endif

		/* lookup the interface */
		ifn = &__ifnet__.ifn[idx];

		/* get the packet from the network interface */
		while ((len = ifn_pkt_recv(ifn, &src, &proto, &pkt)) > 0) {

			tcpip_net_lock();

			NETIF_STAT_ADD(ifn, rx_pkt, 1);
			if (proto == NTOHS(ETH_P_IP)) {
				DCC_LOG(LOG_INFO, "IP");
				DBG("IFNET: IP packet received."); 
				ret = ip_input(ifn, (struct iphdr *)pkt, len);
			} else if (proto == NTOHS(ETH_P_ARP)) {
				DCC_LOG(LOG_INFO, "ARP");
				ret = etharp_input(ifn, (struct etharp*)pkt, len);
			} else {
				NETIF_STAT_ADD(ifn, rx_drop, 1);
				DCC_LOG1(LOG_TRACE, "unhandled protocol: %d", proto);
				WARN("IFNET: unhandled protocol: %d", proto);
				ret = 0;
			}

			tcpip_net_unlock();

			if (ret <= 0)
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

