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

#ifndef __SYS_ROUTE_H__
#define __SYS_ROUTE_H__

#ifndef __USE_SYS_ROUTE__
#error "Never use <sys/route.h> directly"
#endif 

#define  __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <stdint.h>
#include <netinet/in.h>

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

#ifdef __cplusplus
extern "C" {
#endif

int __route_add(in_addr_t __dst, in_addr_t __mask, 
	in_addr_t __gw, struct ifnet * __if);

int __route_del(in_addr_t __dst);

struct route * __route_lookup(in_addr_t __target);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SYS_ROUTE_H__ */

