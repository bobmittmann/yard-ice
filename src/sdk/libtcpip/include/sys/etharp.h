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
 * @file sys/etharp.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_ETHARP_H__
#define __SYS_ETHARP_H__

#include <stdint.h>
#include <tcpip/stat.h>

#define __USE_SYS_ARP__
#include <sys/arp.h>
#include <sys/ethernet.h>

struct etharp {
	struct arphdr ea_hdr;	
	uint8_t arp_sha[6];
	uint8_t arp_spa[4];
	uint8_t arp_dha[6];
	uint8_t arp_dpa[4];
};

#define arp_hdr ea_hdr.ar_hdr
#define arp_pro ea_hdr.ar_pro
#define arp_hln ea_hdr.ar_hln
#define arp_pln ea_hdr.ar_pln
#define arp_op  ea_hdr.ar_op

/*
 * Ethernet ARP cache
 */

struct etharp_entry {
	uint16_t count;
	uint8_t hwaddr[ETH_ADDR_LEN];
	uint32_t ipaddr;
};

#define ETHARP_DGRAM_LEN 28

#ifdef __cplusplus
extern "C" {
#endif

void etharp_clear(void);

int etharp_add(uint32_t ipaddr, uint8_t * hwaddr);

int etharp_input(struct ifnet * __if, struct etharp * __etharp, int __len);

int etharp_query(struct ifnet * __if, in_addr_t __addr);

void * etharp_lookup(struct ifnet * __if, in_addr_t __ipaddr);

void etharp_proto_getstat(struct proto_stat * __st, int __rst);

int etharp_query_pending(void);

int etharp_enum(int (* __callback)(struct ipv4_arp *, void *), void * __parm);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_ETHARP_H__ */

