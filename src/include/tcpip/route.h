/* $Id: route.h,v 2.1 2008/05/23 03:31:04 bob Exp $ 
 *
 * File:	tcpip/route.h
 * Module:	
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:	generic ipv4 routing scheme	
 * Copyright(c) 2003-2005 CNX Technologies. All Rights Reserved.
 *
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

