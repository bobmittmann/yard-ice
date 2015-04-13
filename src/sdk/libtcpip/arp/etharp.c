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
 * @file etharp.c
 * @brief Ethernet ARP
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/etharp.h>

#define __USE_SYS_ROUTE__
#include <sys/route.h>

#include <netinet/in.h>
#include <string.h> 
#include <stdlib.h> 

#ifndef ETHARP_TABLE_LEN
#define ETHARP_TABLE_LEN 2
#endif

#ifndef ENABLE_ETHARP_PROTO_STAT
#define ENABLE_ETHARP_PROTO_STAT 0
#endif

#if ENABLE_ETHARP_PROTO_STAT
struct proto_stat etharp_stat;
#define ETHARP_PROTO_STAT_ADD(STAT, VAL) etharp_stat.STAT += (VAL)
#else
#define ETHARP_PROTO_STAT_ADD(STAT, VAL)
#endif


static const struct etharp_entry bcast = {
	0, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, 0xffffffff
};

struct etharp_system {
	/* one behind cache */
	struct etharp_entry * cache;
	in_addr_t pending;
	struct etharp_entry tab[ETHARP_TABLE_LEN];
};

struct etharp_system __etharp__ = {
	.cache = (struct etharp_entry *)&bcast,
	.pending = INADDR_ANY
};

int etharp_enum(int (* __callback)(struct ipv4_arp *, void *), void * __parm)
{
	struct etharp_entry * p;
	struct ipv4_arp arp;
	int i;
	int n;

	p = __etharp__.tab;
	n = 0;
	for(i = 0; i < ETHARP_TABLE_LEN; i++) {
		if (p->ipaddr) {
			n++;
			arp.type = ARPHRD_ETHER;
			arp.flags = 0;
			arp.ipaddr = p->ipaddr;
			arp.ifn = NULL;
			memcpy(arp.hwaddr, p->hwaddr, ETH_ADDR_LEN);
			if (__callback(&arp, __parm))
				break;
		}
		p++;
	}

	return n;
}

int etharp_add(uint32_t ipaddr, uint8_t * hwaddr)
{
	int i;
	struct etharp_entry * p;
	struct etharp_entry * new;
	int min;
	int n;

	p = __etharp__.tab;
	min = 100000;
	i = ETHARP_TABLE_LEN;
	new = NULL;

	for(i = 0; i < ETHARP_TABLE_LEN; i++) {
		if (((p->ipaddr == ipaddr) ) || 
			((p->hwaddr[0] == hwaddr[0]) &&
			(p->hwaddr[1] == hwaddr[1]) &&
			(p->hwaddr[2] == hwaddr[2]) &&
			(p->hwaddr[3] == hwaddr[3]) &&
			(p->hwaddr[4] == hwaddr[4]) &&
			(p->hwaddr[5] == hwaddr[5]))) {
			min  = 0;
			new = p;
			DCC_LOG(LOG_INFO, "replace");
		} else {
			n = p->count;
			if (n < min) {
				min = n;
				new = p;
			}
		}
		p++;
	}

	new->ipaddr = ipaddr;
	new->hwaddr[0] = hwaddr[0];
	new->hwaddr[1] = hwaddr[1];
	new->hwaddr[2] = hwaddr[2];
	new->hwaddr[3] = hwaddr[3];
	new->hwaddr[4] = hwaddr[4];
	new->hwaddr[5] = hwaddr[5];
	new->count = 0;

	return 0;
}

void * etharp_lookup(struct ifnet * __if, in_addr_t __ipaddr)
{
	int i;
	struct etharp_entry * p;

	if (__etharp__.cache->ipaddr == __ipaddr) {
		DCC_LOG1(LOG_INFO, "cache: %I", __ipaddr);
		p = __etharp__.cache;
		p->count++;
		return p->hwaddr;
	}

	if (__ipaddr == INADDR_BROADCAST) {
		DCC_LOG1(LOG_INFO, "bcast: %I", __ipaddr);
		return (void *)bcast.hwaddr;
	}
	
	p = __etharp__.tab;
	i = ETHARP_TABLE_LEN;
	do {
		if (p->ipaddr == __ipaddr) {
			__etharp__.cache = p;
			DCC_LOG1(LOG_INFO, "found: %I", __ipaddr);
			p->count++;
			return p->hwaddr;
		}
		p++;
	} while(--i);

	DCC_LOG1(LOG_INFO, "not found: %I", __ipaddr);

	/* address pending for resolution ... */
	__etharp__.pending = __ipaddr;

	return NULL;
}

int etharp_query(struct ifnet * __if, in_addr_t __addr)
{
	struct etharp * arp;
	uint32_t ip;
	int ret;

	DCC_LOG1(LOG_TRACE, "%I", __addr);

	/* request a network buffer */
	arp = (struct etharp *)ifn_mmap(__if, sizeof(struct etharp));
	if (arp == NULL) {
		DCC_LOG(LOG_ERROR, "ifn_mmap() fail");
		/* TODO: errno */
		return -1;
	}

	arp->arp_hdr = HTONS(ARPHRD_ETHER);
	arp->arp_pro = HTONS(ETH_P_IP);
	arp->arp_hln = ETH_ADDR_LEN;
	arp->arp_pln = 4;
	arp->arp_op = HTONS(ARPOP_REQUEST);

	ifn_getaddr(__if, arp->arp_sha);
	ip = __if->if_ipv4_addr;

	arp->arp_spa[0] = ip;
	arp->arp_spa[1] = ip >> 8;
	arp->arp_spa[2] = ip >> 16;
	arp->arp_spa[3] = ip >> 24;

	arp->arp_dha[0] = 0;
	arp->arp_dha[1] = 0;
	arp->arp_dha[2] = 0;
	arp->arp_dha[3] = 0;
	arp->arp_dha[4] = 0;
	arp->arp_dha[5] = 0;

	arp->arp_dpa[0] = __addr;
	arp->arp_dpa[1] = __addr >> 8;
	arp->arp_dpa[2] = __addr >> 16;
	arp->arp_dpa[3] = __addr >> 24;

	ETHARP_PROTO_STAT_ADD(tx_ok, 1);

	if ((ret = ifn_send(__if, bcast.hwaddr, HTONS(ETH_P_ARP), 
			 arp, sizeof(struct etharp))) < 0) {
		DCC_LOG(LOG_WARNING, "ifn_send() fail!");
		return ret;
	}

	return 0;
}

int etharp_reply(struct ifnet * __if, uint8_t * __dha, in_addr_t __addr)
{
	struct etharp * arp;
	uint32_t ip;
	int ret;

	/* request a network buffer */
	arp = (struct etharp *)ifn_mmap(__if, sizeof(struct etharp));

	if (arp == NULL) {
		DCC_LOG(LOG_ERROR, "ifn_mmap() fail");
		/* TODO: errno */
		return -1;
	}
	

	arp->arp_hdr = HTONS(ARPHRD_ETHER);
	arp->arp_pro = HTONS(ETH_P_IP);
	arp->arp_hln = ETH_ADDR_LEN;
	arp->arp_pln = 4;
	arp->arp_op = HTONS(ARPOP_REPLY);

	ifn_getaddr(__if, arp->arp_sha);
	ip = __if->if_ipv4_addr;

	DCC_LOG6(LOG_TRACE, "%02x:%02x:%02x:%02x:%02x:%02x", 
			arp->arp_sha[0], arp->arp_sha[1], arp->arp_sha[2], 
			arp->arp_sha[3], arp->arp_sha[4], arp->arp_sha[5]);

	arp->arp_spa[0] = ip;
	arp->arp_spa[1] = ip >> 8;
	arp->arp_spa[2] = ip >> 16;
	arp->arp_spa[3] = ip >> 24;

	arp->arp_dha[0] = __dha[0];
	arp->arp_dha[1] = __dha[1];
	arp->arp_dha[2] = __dha[2];
	arp->arp_dha[3] = __dha[3];
	arp->arp_dha[4] = __dha[4];
	arp->arp_dha[5] = __dha[5];

	arp->arp_dpa[0] = __addr;
	arp->arp_dpa[1] = __addr >> 8;
	arp->arp_dpa[2] = __addr >> 16;
	arp->arp_dpa[3] = __addr >> 24;

	ETHARP_PROTO_STAT_ADD(tx_ok, 1);

	if ((ret = ifn_send(__if, __dha, HTONS(ETH_P_ARP), 
			 arp, sizeof(struct etharp))) < 0) {
		DCC_LOG(LOG_WARNING, "ifn_send() fail!");
		return ret;
	}

	return 0;
}

/*
  return value:
    -1 : error not processed.
     0 : ok processed, packet can be released.
     1 : ok processed, packet reused, don't release.
*/

int etharp_input(struct ifnet * __if, struct etharp * __arp, int __len)
{
	in_addr_t daddr;
	in_addr_t saddr;
//	uint8_t dha[6];
	uint8_t sha[6];
	int i;

	ETHARP_PROTO_STAT_ADD(rx_ok, 1);

	if (__len < sizeof(struct etharp)) {
		DCC_LOG2(LOG_WARNING, "len = %d < %d!", 
				 __len, (int)sizeof(struct etharp));
		ETHARP_PROTO_STAT_ADD(rx_drop, 1);
		ETHARP_PROTO_STAT_ADD(rx_err, 1);
		return -1;
	}

	/* arp destination ip address */
	daddr = IP4_ADDR(__arp->arp_dpa[0], __arp->arp_dpa[1],
		__arp->arp_dpa[2], __arp->arp_dpa[3]);

	/* arp source ip address */
	saddr = IP4_ADDR(__arp->arp_spa[0], __arp->arp_spa[1],
		__arp->arp_spa[2], __arp->arp_spa[3]);

	for (i = 0; i < 6; i++) {
		sha[i] = __arp->arp_sha[i];
//		dha[i] = __arp->arp_dha[i];
	}

	/* is a valid destination ? */
	if (daddr != __if->if_ipv4_addr) {
		DCC_LOG2(LOG_TRACE, "%I:%I, not for me!", saddr, daddr);
		ETHARP_PROTO_STAT_ADD(rx_drop, 1);
		return -1;
	}

	DCC_LOG2(LOG_TRACE, "%I:%I", saddr, daddr);

	/* arp hardware type */
	if (__arp->arp_hdr != HTONS(ARPHRD_ETHER)) {
		DCC_LOG2(LOG_WARNING, "hdr = %d != %d!", 
				 __arp->arp_hdr, HTONS(ARPHRD_ETHER));
		ETHARP_PROTO_STAT_ADD(rx_drop, 1);
		return -1;
	}

	/* arp protocol type */
	if (__arp->arp_pro  != HTONS(ETH_P_IP)) {
		DCC_LOG2(LOG_WARNING, "pro = %d != %d!", 
				 __arp->arp_pro, HTONS(ETH_P_IP));
		ETHARP_PROTO_STAT_ADD(rx_drop, 1);
		return -1;
	}

	/* arp hardware len */
	if (__arp->arp_hln  != ETH_ADDR_LEN) {
		DCC_LOG2(LOG_WARNING, "pro = %d != %d!", 
				 __arp->arp_hln, ETH_ADDR_LEN);
		ETHARP_PROTO_STAT_ADD(rx_drop, 1);
		return -1;
	}

	/* arp protocol len */
	if (__arp->arp_pln != 4) {
		DCC_LOG2(LOG_WARNING, "pln = %d != %d!", __arp->arp_pln, 4);
		ETHARP_PROTO_STAT_ADD(rx_drop, 1);
		return -1;
	}

	etharp_add(saddr, __arp->arp_sha);

	if (__arp->arp_op == HTONS(ARPOP_REQUEST)) {
		DCC_LOG2(LOG_TRACE, "ARP REQUEST %I:%I", saddr, 
				IP4_ADDR(__arp->arp_dpa[0], __arp->arp_dpa[1],
						 __arp->arp_dpa[2], __arp->arp_dpa[3]));

		return etharp_reply(__if, sha, saddr);
	} else {
		DCC_LOG2(LOG_TRACE, "ARP REPLY %I:%I", saddr, 
				IP4_ADDR(__arp->arp_dpa[0], __arp->arp_dpa[1],
						 __arp->arp_dpa[2], __arp->arp_dpa[3]));
		return 0;
	}
}

int etharp_query_pending(void)
{
	struct route * rt;
	in_addr_t daddr;

	if ((daddr = __etharp__.pending) == INADDR_ANY)
		return 0;

	__etharp__.pending = INADDR_ANY;

	if ((rt = __route_lookup(daddr)) == NULL) {
		DCC_LOG1(LOG_WARNING, "no route to host: %I", daddr);
		return -1;
	}

	DCC_LOG1(LOG_TRACE, "pending: %I", daddr);

	if (rt->rt_gateway != INADDR_ANY)
		daddr = rt->rt_gateway;

	return etharp_query(rt->rt_ifn , daddr);
}


void etharp_proto_getstat(struct proto_stat * __st, int __rst)
{
#if ENABLE_ETHARP_PROTO_STAT
	proto_stat_copy(__st, &etharp_stat, __rst);
#else
	memset(__st, 0, sizeof(struct proto_stat)); 
#endif
}

