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
 * @file sys/udp.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_UDP_H__
#define __SYS_UDP_H__

#ifndef __USE_SYS_UDP__
#error "Never use <sys/udp.h> directly; include <tcpip/udp.h> instead."
#endif 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifndef ENABLE_NET_UDP
#define ENABLE_NET_UDP 1
#endif

#ifndef NET_UDP_PCB_MAX
#define NET_UDP_PCB_MAX 2
#endif

#ifndef NET_UDP_RECV_QUEUE_LEN
#define NET_UDP_RECV_QUEUE_LEN 4
#endif

#ifndef NET_UDP_DEFAULT_TTL
#define NET_UDP_DEFAULT_TTL 127
#endif

#ifndef NET_UDP_DEFAULT_TOS
#define NET_UDP_DEFAULT_TOS 0x80
#endif

#include <stdint.h>
#include <netinet/in.h>
#include <netinet/udp.h>

#include <thinkos.h>
#include <sys/mbuf.h>
#include <sys/net.h>
#include <sys/pcb.h>
#define __USE_SYS_NET__
#include <sys/net.h>
#define __USE_SYS_IFNET__
#include <sys/ifnet.h>
#define __USE_SYS_ARP__
#include <sys/arp.h>
#define __USE_SYS_ROUTE__
#include <sys/route.h>
#include <sys/ip.h>

#include <tcpip/stat.h>

#ifdef UDP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#ifndef ENABLE_UDP_PROTO_STAT
#define ENABLE_UDP_PROTO_STAT 0
#endif

struct udp_dgram {
	in_addr_t addr;
	uint16_t port;
	uint16_t len;
	struct mbuf * q;
}; /* 12 bytes */

/*! \brief UDP protocol control block. */
struct udp_pcb {
	/*! foreign address */
	in_addr_t u_faddr;
	/*! local address */
	in_addr_t u_laddr;
	union {
		struct {
			/*! foreign port */
			uint16_t u_fport;
			/*! local port */
			uint16_t u_lport;
		};
		uint32_t u_ports;
	};
	/* 12 */
	/*! udp flags  */
	uint8_t u_flags : 7;
	uint8_t u_icmp_err : 1;
	uint8_t u_rcv_cond;
	/* receiving queue */
	volatile uint8_t u_rcv_tail;
	volatile uint8_t u_rcv_head;
	/* 16 */
	struct udp_dgram u_rcv_buf[NET_UDP_RECV_QUEUE_LEN];
	/* 4 * 12 = 48 + 16 = 64*/
};

struct udp_pcb_link {
	struct pcb_link * next;
	struct udp_pcb pcb;
};

struct udp_system {
#ifdef ENABLE_UDP_CACHE
	struct udp_pcb * cache;
#endif
	/* list of free PCBs */
	struct pcb_list free;
	/* active PCBs list */
	struct pcb_list active;
#if ENABLE_UDP_PROTO_STAT
	struct proto_stat stat;
#endif
	struct udp_pcb_link pcb_pool[NET_UDP_PCB_MAX];
};

#if ENABLE_UDP_PROTO_STAT
#define UDP_PROTO_STAT_ADD(STAT, VAL) __udp__.stat.STAT += (VAL)
#else
#define UDP_PROTO_STAT_ADD(STAT, VAL)
#endif

extern struct udp_system __udp__;

//extern uthread_mutex_t udp_input_mutex;

#ifdef __cplusplus
extern "C" {
#endif

/* Internals */

int udp_input(struct ifnet * __if, struct iphdr * __ip, 
			   struct udphdr * __udp, int __len);

void udp_output(struct udp_pcb * __up);

/*
 * This function is called from the ICMP input to notify an
 * UDP connection when the remote host sends a ICMP_PORT_UNREACH
 * message.
 */
int udp_port_unreach(in_addr_t __faddr, uint16_t __fport, 
					 in_addr_t __laddr, uint16_t __lport);

int udp_release(struct udp_pcb * __up);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_UDP_H__ */

