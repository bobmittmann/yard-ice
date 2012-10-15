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
 * @file sys/arp.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_ARP_H__
#define __SYS_ARP_H__

#ifndef __USE_SYS_ARP__
#error "Never use <sys/arp.h> directly; include <tcpip/arp.h> instead."
#endif 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef ARP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <tcpip/in.h>

/*
 * ARP cache
 */

#define ARP_MAX_HWADDR_LEN 6

struct arp_entry {
	uint8_t count;
	uint8_t flags;
	uint8_t hwaddr[ARP_MAX_HWADDR_LEN];
	in_addr_t ipv4_addr;
	struct ifnet * ifn;
};

extern inline void * arp_lookup(struct ifnet * __if, in_addr_t __ipaddr) {
	return __if->if_op->op_arplookup(__if, __ipaddr);
}

#ifdef __cplusplus
extern "C" {
#endif

int arp_query_pending(void);

void * arp_lookup(struct ifnet * __if, in_addr_t __ipaddr);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_ARP_H__ */

