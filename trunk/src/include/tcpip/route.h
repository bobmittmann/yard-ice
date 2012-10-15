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
 * @file tcpip/route.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TCPIP_ROUTE_H__
#define __TCPIP_ROUTE_H__

#include <stdint.h>
#include <netinet/in.h>

#include <tcpip/ifnet.h>

struct route {
	/* Target address.  */
	in_addr_t rt_dst;
	/* Gateway addr (RTF_GATEWAY).  */
	in_addr_t rt_gateway;
	/* Target network mask (IP).  */
	in_addr_t rt_genmask;
	/* Network interface */
	struct ifnet * rt_ifn;
};

//uint16_t rt_flags;
//uint8_t rt_tos;
//uint8_t rt_metric;
/* Per route MTU/Window. */
//uint16_t rt_mtu;		
/* Initial RTT.  */
//uint16_t rt_irtt;		


#define	RTF_UP		0x0001		/* Route usable.  */
#define	RTF_GATEWAY	0x0002		/* Destination is a gateway.  */
#define	RTF_HOST	0x0004		/* Host entry (net otherwise).  */
#define RTF_REINSTATE	0x0008		/* Reinstate route after timeout.  */
#define	RTF_DYNAMIC	0x0010		/* Created dyn. (by redirect).  */
#define	RTF_MODIFIED	0x0020		/* Modified dyn. (by redirect).  */
#define RTF_MTU		0x0040		/* Specific MTU for this route.  */
#define RTF_WINDOW	0x0080		/* Per route window clamping.  */
#define RTF_IRTT	0x0100		/* Initial round trip time.  */
#define RTF_REJECT	0x0200		/* Reject route.  */
#define	RTF_STATIC	0x0400		/* Manually injected route.  */
#define	RTF_XRESOLVE	0x0800		/* External resolver.  */
#define RTF_NOFORWARD   0x1000		/* Forwarding inhibited.  */
#define RTF_THROW	0x2000		/* Go to next class.  */
#define RTF_NOPMTUDISC  0x4000		/* Do not send packets with DF.  */

extern const uint8_t ip_maxroute;

#ifdef __cplusplus
extern "C" {
#endif

int route_add(in_addr_t __dst, in_addr_t __mask, 
	in_addr_t __gw, struct ifnet * __if);

int route_del(in_addr_t __dst);

struct route * route_lookup(in_addr_t __target);

int route_enum(int (* __callback)(struct route *, void *), void * __parm);

struct route * route_get(in_addr_t __dst, in_addr_t __mask);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_ROUTE_H__ */

