/* 
 * File:	tcpip/ip.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __TCPIP_IP_H__
#define __TCPIP_IP_H__

#include <netinet/ip.h>

#include <tcpip/route.h>
#include <tcpip/stat.h>

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

void ip_input(struct ifnet * __if, struct iphdr * __ip, int __len);

int ip_output(struct route * __rt, struct iphdr * __ip);

struct iphdr * iph_template(struct iphdr * __ip, int proto, int ttl, int tos);

struct iphdr * mk_iphdr(struct iphdr * iph, in_addr_t saddr, 
						in_addr_t daddr, int len);

void * ip_resolve(struct iphdr * __ip, struct route * __rt);

void ip_proto_getstat(struct proto_stat * __st, int __rst);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_IP_H__ */

