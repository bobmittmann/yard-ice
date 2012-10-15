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
#include <netinet/ip.h>

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

#ifdef __cplusplus
extern "C" {
#endif

/* Network interface database lookup */
int ifn_enum(int (* __callback)(struct ifnet *, void *), void * __parm);

struct ifnet * ifn_get_first(void);

struct ifnet * ifn_get_next(struct ifnet * __if);

struct ifnet * get_ifn_byname(const char * __s);

struct ifnet * get_ifn_byipaddr(in_addr_t __addr);

/* Netork device initialization */
struct ifnet * ifn_register(void * __drv, const struct ifnet_operations * __op,
							void * __io, int __irq_no);

/* Netork device cleanup */
int ifn_unregister(struct ifnet * __if);

/* get network ipv4 address */
int ifn_ipv4_set(struct ifnet * __if, in_addr_t __addr, in_addr_t __mask);

/* get network ipv4 address */
int ifn_ipv4_get(struct ifnet * __if, in_addr_t * __addr, in_addr_t * __mask);

/* get the interface name ('eth0'...) */
int ifn_getname(struct ifnet * __if, char * __s);

/* get the interface media address if any */
int ifn_getaddr(struct ifnet * __if, uint8_t * __buf);

/* return a string describing the interface. */
int ifn_getinfo(struct ifnet * __if, char * __s, int __len);

/* get the network interface statistic counters, 
   optionally reseting the counters */
void ifn_getstat(struct ifnet * __if, struct ifnet_stat * __st, int __rst);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_NETIF_H__ */

