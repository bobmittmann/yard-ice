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
#include <tcpip/arp.h>

/* ARP protocol opcodes. */
#define	ARPOP_REQUEST	1		/* ARP request.  */
#define	ARPOP_REPLY	2		/* ARP reply.  */
#define	ARPOP_RREQUEST	3		/* RARP request.  */
#define	ARPOP_RREPLY	4		/* RARP reply.  */
#define	ARPOP_InREQUEST	8		/* InARP request.  */
#define	ARPOP_InREPLY	9		/* InARP reply.  */
#define	ARPOP_NAK	10		/* (ATM)ARP NAK.  */

/* See RFC 826 for protocol description.  ARP packets are variable
   in size; the arphdr structure defines the fixed-length portion.
   Protocol type values are the same as those for 10 Mb/s Ethernet.
   It is followed by the variable-sized fields ar_sha, arp_spa,
   arp_tha and arp_tpa in that order, according to the lengths
   specified.  Field names used correspond to RFC 826.  */

struct arphdr {
	/* Format of hardware address.  */
	uint16_t ar_hdr;
	/* Format of protocol address.  */
	uint16_t ar_pro;
	/* Length of hardware address.  */
	uint8_t ar_hln;
	/* Length of protocol address.  */
	uint8_t ar_pln;
	/* ARP opcode (command).  */
	uint16_t ar_op;
};


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

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __SYS_ARP_H__ */

