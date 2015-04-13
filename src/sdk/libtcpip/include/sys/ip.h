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
 * @file sys/ip.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_IP_H__
#define __SYS_IP_H__

#include <netinet/ip.h>

struct ip_info {
	uint8_t info_tag;	
    uint8_t ttl;
	uint8_t proto;
    uint8_t res;
    uint32_t saddr;
    uint32_t daddr;
};

extern uint16_t ip_sequence;
extern const uint8_t ip_defttl;
extern const uint8_t ip_deftos;

#ifdef __cplusplus
extern "C" {
#endif

int ip_input(struct ifnet * __if, struct iphdr * __ip, int __len);

int ip_output(struct ifnet * __ifn, struct route * __rt, struct iphdr * __ip);

struct iphdr * iph_template(struct iphdr * __ip, int proto, int ttl, int tos);

struct iphdr * mk_iphdr(struct iphdr * iph, in_addr_t saddr, 
						in_addr_t daddr, int len);

void * ip_resolve(struct iphdr * __ip, struct route * __rt);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_IP_H__ */

