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

extern const uint8_t ip_maxroute;

#ifdef __cplusplus
extern "C" {
#endif

int ipv4_route_add(in_addr_t __dst, in_addr_t __mask, 
	in_addr_t __gw, struct ifnet * __if);

int ipv4_route_del(in_addr_t __dst);

struct route * ipv4_route_lookup(in_addr_t __target);

int ipv4_route_enum(int (* __callback)(struct route *, void *), void * __parm);

struct route * ipv4_route_get(in_addr_t __dst, in_addr_t __mask);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_ROUTE_H__ */

