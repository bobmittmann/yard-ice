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
 * @file sys/ifnet.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_IFNET_H__
#define __SYS_IFNET_H__

#ifndef __USE_SYS_IFNET__
#error "Never use <sys/ifnet.h> directly; include <tcpip/ifnet.h> instead."
#endif 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef IFNET_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#define __need_size_t
#include <stddef.h>

#define __USE_SYS_NET__
#include <sys/net.h>

#include <stdint.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <tcpip/in.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#ifndef ENABLE_NETIF_STAT
#define ENABLE_NETIF_STAT 0
#endif

#ifndef IFNET_INTERFACES_MAX
#define IFNET_INTERFACES_MAX 1
#endif

enum {
	IFN_PROTO_IP = 0,
	IFN_PROTO_ETHARP
};

struct ifnet;

struct ifnet_operations {
	/* interface type */
	uint8_t op_type;

	/* mac address length */
	uint8_t op_addrlen;

	/* initialize de interface */
	int (* op_init)(struct ifnet * __if);

	/* remove form ifnterface list */
	int (* op_cleanup)(struct ifnet * __if);

	/* request a memory region */
	void * (* op_mmap)(struct ifnet * __if, size_t __length);

	/* receive a frame */
	int (* op_pkt_recv)(struct ifnet * __if, uint8_t ** __src,
						unsigned int * __proto, uint8_t ** __pkt);

	/* release a frame */
	int (* op_pkt_free)(struct ifnet * __if, uint8_t * __pkt);

	/* send a frame */
	int (* op_send)(struct ifnet * __if, const uint8_t * __dst,
		int __proto, const void * __buf, int __len);

	/* translate ipv4 address into hardware address */
	void * (* op_arplookup)(struct ifnet * __if, in_addr_t __ipaddr);	

	int (* op_arpquery)(struct ifnet * __if, in_addr_t __ipaddr);

	/* get the interface mac address if any */
	int (* op_getaddr)(struct ifnet * __if, uint8_t * __buf);	

	/* put the interface in a sleep state */
	int (* op_sleep)(struct ifnet * __if);

	/* wakeup the interface from sleep state */
	int (* op_wakeup)(struct ifnet * __if);

	/* return a string describing the interface. */
	int (* op_getdesc)(struct ifnet * __if,  char * __desc, int len);

	/* release a memory region */
	int (* op_munmap)(struct ifnet * __if, void * __mem);
};

struct ifnet {
	/* low level io address */
	void * if_io;

	/* interface index for reverse lookup */
	uint8_t if_idx;

	/* irq number */
	uint8_t if_irq_no;

	/* interface id */
	uint8_t if_id;

	/* flags (IFF_*) */
	uint16_t if_flags;

	/* maximum transmission unit */
	uint16_t if_mtu;

	/* interface link speed (bps) */
	uint32_t if_link_speed;

	/* IPV4 address */
	in_addr_t if_ipv4_addr;

	/* IPV4 network mask */
	in_addr_t if_ipv4_mask;

	/* low level device driver private structure */
	void * if_drv;

	/* interface operations */
	const struct ifnet_operations * if_op;
#if (ENABLE_NETIF_STAT)
	struct ifnet_stat stat;
#endif
};


#if ENABLE_NETIF_STAT
#define NETIF_STAT_ADD(NETIF, STAT, VAL) (NETIF)->stat.STAT += (VAL)
#else
#define NETIF_STAT_ADD(NETIF, STAT, VAL)
#endif

struct ifnet_system {
	int evset;
	struct ifnet ifn[IFNET_INTERFACES_MAX];
};

extern  const uint8_t ifnet_max;

extern  struct ifnet_system __ifnet__;

extern const char ifn_name_lut[][5];

static inline void ifn_signal_i(int event) {
	thinkos_ev_raise_i(__ifnet__.evset, event);
}

static inline void ifn_signal(int event) {
	thinkos_ev_raise(__ifnet__.evset, event);
}

extern inline int ifn_init(struct ifnet * __if) {
	return __if->if_op->op_init(__if);
}

extern inline int ifn_cleanup(struct ifnet * __if) {
	return __if->if_op->op_cleanup(__if);
}

extern inline void * ifn_mmap(struct ifnet * __if, int __length) {
	return __if->if_op->op_mmap(__if, __length);
}

extern inline int ifn_munmap(struct ifnet * __if, void * __mem) {
	return __if->if_op->op_munmap(__if, __mem);
}

extern inline int ifn_send(struct ifnet * __if, const uint8_t * __dst, 
							  int __proto, const void * __buf, int __len) {
	return __if->if_op->op_send(__if, __dst, __proto, __buf, __len);
}

extern inline int  ifn_sleep(struct ifnet * __if) {
	return __if->if_op->op_sleep(__if);
}

extern inline int ifn_wakeup(struct ifnet * __if) {
	return __if->if_op->op_wakeup(__if);
}

extern inline int ifn_getaddr(struct ifnet * __if, uint8_t * __buf) {
	return __if->if_op->op_getaddr(__if, __buf);
}

extern inline int ifn_getdesc(struct ifnet * __if, char * __s, int __len) {
	return __if->if_op->op_getdesc(__if, __s, __len);
}

extern inline void * ifn_arplookup(struct ifnet * __if, in_addr_t __ipaddr) {
	return __if->if_op->op_arplookup(__if, __ipaddr);
}

extern inline int ifn_arpquery(struct ifnet * __if, in_addr_t __ipaddr) {
	return __if->if_op->op_arpquery(__if, __ipaddr);
}

extern inline int in_broadcast(in_addr_t __addr, struct ifnet * __if) {
	return (__addr | __if->if_ipv4_mask) == INADDR_BROADCAST;
}

extern inline int ifn_pkt_recv(struct ifnet * __if, uint8_t ** __src,
					unsigned int * __proto, uint8_t ** __pkt) {
	return __if->if_op->op_pkt_recv(__if, __src, __proto, __pkt);
}

extern inline int ifn_pkt_free(struct ifnet * __if, uint8_t * __pkt) {
	return __if->if_op->op_pkt_free(__if, __pkt);
}

#ifdef __cplusplus
extern "C" {
#endif

int ifn_sleep(struct ifnet * __if);

int ifn_wakeup(struct ifnet * __if);

int ifn_init(struct ifnet * __if);

void * ifn_mmap(struct ifnet * __if, int __length);

int ifn_send(struct ifnet * __if, const uint8_t * __dst, 
			 int __proto, const void * __buf, int __len);

int ifn_add(struct ifnet * __if);

int ifn_del(struct ifnet * __if);

int in_broadcast(in_addr_t __addr, struct ifnet * __if);

int ifnet_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_IFNET_H__ */

