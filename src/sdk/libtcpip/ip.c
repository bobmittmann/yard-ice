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
 * @file ip.c
 * @brief IP layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef IP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#ifndef ENABLE_NET_ARP_PING_IPCONFIG
#define ENABLE_NET_ARP_PING_IPCONFIG 0
#endif

#define __USE_SYS_RAW__
#include <sys/raw.h>
#define __USE_SYS_UDP__
#include <sys/udp.h>
#define __USE_SYS_TCP__
#include <sys/tcp.h>
#define __USE_SYS_IFNET__
#include <sys/ifnet.h>
#define __USE_SYS_ARP__
#include <sys/arp.h>

#include <sys/ethernet.h>
#include <sys/etharp.h>

//#include <tcpip/ip.h>
#include <tcpip/icmp.h>
//#include <tcpip/in.h>

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <trace.h>

#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_NONE

#ifndef IP_DEFAULT_TTL
#define IP_DEFAULT_TTL 127
#endif
const uint8_t ip_defttl = IP_DEFAULT_TTL;

#ifndef IP_DEFAULT_TOS
#define IP_DEFAULT_TOS 0
#endif
const uint8_t ip_deftos = IP_DEFAULT_TOS;

uint16_t ip_sequence = 0;

#ifndef ENABLE_IP_PROTO_STAT
#define ENABLE_IP_PROTO_STAT 0
#endif

#if ENABLE_IP_PROTO_STAT
struct proto_stat ip_stat;
#define IP_PROTO_STAT_ADD(STAT, VAL) ip_stat.STAT += (VAL)
#else
#define IP_PROTO_STAT_ADD(STAT, VAL)
#endif

int arp_ping_ipconfig(struct ifnet * __if, struct iphdr * __ip, 
					  struct icmphdr * __icmp, int __len);
/*
  return value:
    -1 : error not processed.
     0 : ok processed, packet can be released.
     1 : ok processed, packet reused, don't release.
*/

int ip_input(struct ifnet * __if, struct iphdr * __ip, int __len)
{
	int hdr_len;
	int tot_len;
	int len;
	void * ptr;
	int ret;

	if (__ip->ver != 4) {
		/* unsupported version */
		DCC_LOG(LOG_WARNING, "not ipv4!");
		WARN("IP: not ipv4!");
		IP_PROTO_STAT_ADD(rx_drop, 1);
		return -1;
	}

	/* header length  */
	hdr_len = (__ip->hlen) << 2;
	tot_len = ntohs(__ip->tot_len);
	if ((hdr_len < sizeof(struct iphdr)) || (hdr_len > __len)) {
		/* invalid header length */
		DCC_LOG(LOG_WARNING, "invalid iphdr length!");
		WARN("IP: invalid iphdr length!");
		IP_PROTO_STAT_ADD(rx_drop, 1);
		return -1;
	}

	if (tot_len > __len) {
		/* invalid datagram length */
		DCC_LOG2(LOG_WARNING, "length error: ip.tot_len=%d frame.len=%d",
				 tot_len, __len);
		WARN("IP: length error: ip.tot_len=%d frame.len=%d",
			 tot_len, __len);
		IP_PROTO_STAT_ADD(rx_drop, 1);
		return -1;
	}

	/* TODO: check for IP flags */
	if (__ip->ip_off & NTOHS(IP_MF)) {
		DCC_LOG(LOG_WARNING, "ip fragmentation unsupported!");
		WARN("IP: fragmentation unsupported!");
		/* ip fragmentation flag set */
		IP_PROTO_STAT_ADD(rx_drop, 1);
		return -1;
	}

	if (in_chksum(0, (uint8_t *)__ip, hdr_len) != 0x0000ffff) {
		/* bad ip checksum */
		DCC_LOG(LOG_WARNING, "bad checksum!");
		IP_PROTO_STAT_ADD(rx_drop, 1);
		IP_PROTO_STAT_ADD(rx_err, 1);
		return -1;
	}

	IP_PROTO_STAT_ADD(rx_ok, 1);

	ptr = (void *)&__ip->opt[hdr_len - sizeof(struct iphdr)];
	len = tot_len - hdr_len;

	DCC_LOG3(LOG_INFO, "IP %I > %I (%d)", __ip->saddr, __ip->daddr, __len);
	DBG("IP %I > %I (%d)", __ip->saddr, __ip->daddr, __len);

	/* check the destination ip address */
	/* get interface ip address */
	if (__ip->daddr != __if->if_ipv4_addr) {
		/* broadcast ip address */
		if (__ip->daddr != INADDR_BROADCAST) {
			/* interface network broadcast ip address  */
			if (__ip->daddr != (__if->if_ipv4_addr | ~(__if->if_ipv4_mask))) {
				/* wrong destination ! */
				DCC_LOG3(LOG_WARNING, "invalid dest: IP %I > %I (%d)", 
						 __ip->saddr, __ip->daddr, len);
#if (ENABLE_NET_ARP_PING_IPCONFIG)
				if (__ip->proto == IPPROTO_ICMP)
					arp_ping_ipconfig(__if, __ip, (struct icmphdr *)ptr, len);
#endif
				IP_PROTO_STAT_ADD(rx_drop, 1);
				return -1;
			}
		}
	}

	switch (__ip->proto) {
	case IPPROTO_ICMP: 
		DCC_LOG(LOG_INFO, "ICMP");
		ret = icmp_input(__if, __ip, (struct icmphdr *)ptr, len);
		break;

#if (ENABLE_NET_UDP)
	case IPPROTO_UDP:
		DCC_LOG(LOG_INFO, "UDP");
		ret = udp_input(__if, __ip, (struct udphdr *)ptr, len);
		break;
#endif

#if (ENABLE_NET_TCP)
	case IPPROTO_TCP:
		DCC_LOG(LOG_INFO, "TCP");
		ret = tcp_input(__if, __ip, (struct tcphdr *)ptr, len);
		break;
#endif

	default:
		DCC_LOG1(LOG_INFO, "invalid protocol %d.", __ip->proto);
		ret = 0;
	}

	if (ret < 0) {
#if (ENABLE_NET_RAW)
		DCC_LOG(LOG_INFO, "RAW");
		if ((ret = raw_input(__if, __ip, tot_len)) < 0)
#endif
			IP_PROTO_STAT_ADD(rx_drop, 1);
	}

	return ret;
}

int ip_output(struct ifnet * __ifn, struct route * __rt, struct iphdr * __ip)
{
	void * dst_hwaddr;
	in_addr_t dst;
	int len;

	if ((__rt != NULL) && (__rt->rt_gateway != INADDR_ANY)) {
		dst = __rt->rt_gateway;
		DCC_LOG1(LOG_INFO, "gateway: %I", dst);
	} else {
		dst = __ip->daddr;
		DCC_LOG1(LOG_INFO, "direct: %I", dst);
	}

	dst_hwaddr = ifn_arplookup(__ifn, dst);

	if (dst_hwaddr == NULL) {
		/* if we can't get the link address return EGAIN.
		   The upper (transport) layer is responsible to 
		   resend the package */
		DCC_LOG1(LOG_WARNING, "ARP lookup fail: %I", dst);
		/* XXX: update statistics ??
		IP_PROTO_STAT_ADD(tx_drop, 1);
		   */
		return -EAGAIN;
	}

	len = ntohs(__ip->tot_len);	

	IP_PROTO_STAT_ADD(tx_ok, 1);

	DCC_LOG3(LOG_INFO, "IP %I > %I (%d)", __ip->saddr, __ip->daddr, len);

	return ifn_send(__ifn, dst_hwaddr, HTONS(ETH_P_IP), __ip, len);
}

struct iphdr * mk_iphdr(struct iphdr * iph, in_addr_t saddr, 
						in_addr_t daddr, int len)
{
	uint32_t tmp;
	
	iph->saddr = saddr;
	iph->daddr = daddr;
	len += IP_HEADER_LEN;
	/* endianness - the most significant bytes of len are allways 0 */
	/* XXX: this code is to optimize de ARM operations ... */
	tmp = (uint32_t)len << 24;
	tmp = ((uint32_t)len >> 8) + (tmp >> 16);
	iph->tot_len = tmp;

	iph->chksum = ~in_chksum(0, (uint16_t *)iph, IP_HEADER_LEN);

	return iph;
}

/* version, len, tos */
#define IP_HLVT(VER, TOS) ((TOS << 8) +  (VER << 4) + (IP_HEADER_LEN >> 2))

/* 
 * NOTE: This is a helper structure to speed up the ip header building 
 *  in 32bits architectures. It's not expected to have a good performance in
 *  16 or 8 bits processors.
*/
struct ipht {
    uint32_t hlen_ver_tos_len;
    uint32_t id_frag;
    uint32_t ttl_proto_chksum;
    uint32_t saddr;
    uint32_t daddr;
};

/*
 * Create a IP header template. With some fields filled up. 
 */
struct iphdr * iph_template(struct iphdr * __ip, int proto, int ttl, int tos)
{
	struct ipht * p =  (struct ipht *)__ip;
	uint32_t x;
	uint32_t y;

	p->ttl_proto_chksum = (uint32_t)((proto << 8) + ttl);
	p->hlen_ver_tos_len = (IP_HEADER_LEN << 24) + IP_HLVT(4, tos);
	/* The strange sequence bellow is meant to produce good assembly
	   with poor compilers for 32bit processors. */
	x = ++ip_sequence << 16;
	y = x >> 24;
	x = x << 8;
	p->id_frag = (x >> 16) + y;

	return (struct iphdr *)__ip;
}

void ip_proto_getstat(struct proto_stat * __st, int __rst)
{
#if ENABLE_IP_PROTO_STAT
	proto_stat_copy(__st, &ip_stat, __rst);
#else
	memset(__st, 0, sizeof(struct proto_stat)); 
#endif
}

