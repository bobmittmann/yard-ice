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
 * @file sys/raw.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_RAW_H__
#define __SYS_RAW_H__

#ifndef __USE_SYS_RAW__
#error "Never use <sys/raw.h> directly; include <tcpip/raw.h> instead."
#endif 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef RAW_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#define __USE_SYS_NET__
#include <sys/net.h>
#define __USE_SYS_ROUTE__
#include <sys/route.h>
#define __USE_SYS_IFNET__
#include <sys/ifnet.h>
#define __USE_SYS_ARP__
#include <sys/arp.h>

#include <sys/pcb.h>
#include <sys/ip.h>

#include <tcpip/in.h>


/* raw pcb flags */
#define RF_NONBLOCK     0x80

struct raw_stat {
	uint32_t rx_pkt;
	uint32_t rx_drop;
	uint32_t rx_err;
	uint32_t tx_pkt;
	uint32_t tx_drop;
	uint32_t tx_err;
};

struct raw_pcb {
	/*! foreign address */
	in_addr_t r_faddr;
	/*! local address */
	in_addr_t r_laddr;

	union {
		struct {
			/* foreign port */
			uint16_t r_fport;
			/* local port */
			uint16_t r_lport;
		};
		uint32_t r_ports;
	};

	uint8_t r_flags;
	uint8_t r_state;

	int8_t r_cond;

	/*! ip type of service  */
	uint8_t r_tos;

	uint16_t r_protocol;

	/*! mtu */
	uint16_t r_mtu;

	uint16_t r_len;

	void * r_buf;
	struct iphdr * r_ip;
};

struct raw_system {
	/*! pcb list */
	struct pcb_list list;
};

extern struct raw_system __raw__;

#ifdef __cplusplus
extern "C" {
#endif

int raw_pcb_free(struct raw_pcb * __raw);

int raw_input(struct ifnet * __if, struct iphdr * __ip, int __len);

void raw_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_RAW_H__ */

