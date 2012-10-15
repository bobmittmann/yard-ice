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
 * @file tcpip/icmp.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TCPIP_ICMP_H__
#define __TCPIP_ICMP_H__

#include <stdint.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <tcpip/stat.h>

#define ICMP_HEADER_LEN 4

#ifdef __cplusplus
extern "C" {
#endif

int icmp_input(struct ifnet * __if, struct iphdr * __ip, 
			   struct icmphdr * __icmp, int __len);

int icmp_error(struct iphdr * __ip, int __type, 
	int __code, struct ifnet * __if);

int icmp_echoreplay(struct ifnet * __if, struct iphdr * __ip, 
					struct icmphdr * __icmp, int __len);

void icmp_proto_getstat(struct proto_stat * __st, int __rst);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_ICMP_H__ */

