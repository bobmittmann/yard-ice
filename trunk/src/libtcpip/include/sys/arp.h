/* 
 * File:	sys/arp.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:	TCPIP
 * Copyright(c) 2003-2005 CNX Technologies. All Rights Reserved.
 *
 */

#ifndef __SYS_ARP_H__
#define __SYS_ARP_H__

#ifndef __USE_SYS_ARP__
#error "Never use <sys/raw.h> directly; include <tcpip/raw.h> instead."
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

