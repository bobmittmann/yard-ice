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
 * @file tcpip/ifnet.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TCPIP_NETIF_H__
#define __TCPIP_NETIF_H__

#include <stdint.h>
#include <netinet/in.h>

struct ifnet;

struct ifnet_operations;

struct ifnet_stat {
	uint32_t rx_pkt;
	uint32_t rx_drop;
	uint32_t rx_err;
	uint32_t tx_pkt;
	uint32_t tx_drop;
	uint32_t tx_err;
};

#define IFNET_INFO_NAME_MAX 7
#define IFNET_INFO_DESC_MAX 63
#define IFNET_HW_ADDR_MAX 16

struct ifnet_info {
	uint8_t type;
	uint16_t flags;
	uint16_t mtu;
	uint32_t lnk_speed;
	uint8_t hw_addr[IFNET_HW_ADDR_MAX];
	char name[IFNET_INFO_NAME_MAX + 1];
	char desc[IFNET_INFO_DESC_MAX + 1];
};

/* Network Interface Flags */
/* interface is up */
#define	IFF_UP              0x0001		
/* broadcast address valid */
#define IFF_BROADCAST       0x0002
/* is a loopback net */
#define IFF_LOOPBACK        0x0004
/* interface is has p-p link */
#define IFF_POINTTOPOINT    0x0008
/* no ARP protocol */
#define IFF_NOARP           0x0010
/* receive all packets */
/* supports multicast */
#define IFF_MULTICAST       0x0020
/* link is active */
#define IFF_LINK_UP         0x0080

/* Network Interface Type */
enum {
	IFT_LOOP  = 0,
	IFT_ETHER = 1,
	IFT_SLIP  = 2,
	IFT_PPP   = 3,
	IFT_OTHER = 4
};

#ifdef __cplusplus
extern "C" {
#endif

/* Network interface database lookup */
int ifn_enum(int (* __callback)(struct ifnet *, void *), void * __parm);

struct ifnet * ifn_get_first(void);

struct ifnet * ifn_get_next(struct ifnet * __if);

struct ifnet * get_ifn_byname(const char * __s);

struct ifnet * get_ifn_byipaddr(in_addr_t __addr);

int ifn_getname(struct ifnet * __if, char * __s);

char * ifn_name(struct ifnet * __if);

/* Netork device initialization */
struct ifnet * ifn_register(void * __drv, const struct ifnet_operations * __op,
							void * __io, int __irq_no);

/* Netork device cleanup */
int ifn_unregister(struct ifnet * __if);

/* get network ipv4 address */
int ifn_ipv4_set(struct ifnet * __if, in_addr_t __addr, in_addr_t __mask);

/* get network ipv4 address */
int ifn_ipv4_get(struct ifnet * __if, in_addr_t * __addr, in_addr_t * __mask);

/* get the interface media address if any */
int ifn_getaddr(struct ifnet * __if, uint8_t * __buf);

/* return a structure describing the interface. */
int ifn_getinfo(struct ifnet * __if, struct ifnet_info * __info);

/* get the network interface statistic counters, 
   optionally reseting the counters */
void ifn_getstat(struct ifnet * __if, struct ifnet_stat * __st, int __rst);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_NETIF_H__ */

