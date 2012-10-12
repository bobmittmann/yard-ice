/* $Id: etharp.h,v 2.4 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcpip/etharp.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:	TCPIP
 * Copyright(c) 2003-2005 CNX Technologies. All Rights Reserved.
 *
 */

#ifndef __TCPIP_ETHARP_H__
#define __TCPIP_ETHARP_H__

#include <stdint.h>
#include <net/ethernet.h>
#include <net/if_arp.h>

#include <tcpip/ifnet.h>
#include <tcpip/stat.h>

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

int etharp_enum(int (* __callback)(struct etharp_entry *, void *), 
				void * __parm);

void etharp_input(struct ifnet * __if, struct etharp * __etharp, int __len);

int etharp_query(struct ifnet * __if, in_addr_t __addr);

void * etharp_lookup(struct ifnet * __if, in_addr_t __ipaddr);

void etharp_proto_getstat(struct proto_stat * __st, int __rst);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_ETHARP_H__ */

