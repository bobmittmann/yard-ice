/* $Id: icmp.h,v 2.2 2008/05/23 03:31:04 bob Exp $ 
 *
 * File:	tcpip/icmp.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:	TCPIP
 * Copyright(c) 2003-2005 CNX Technologies. All Rights Reserved.
 *
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

