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
 * @file icmp.c
 * @brief minimal ICMP
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef ICMP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>


#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/param.h>

#include <netinet/in.h>
#include <netinet/ip.h>

#define __USE_SYS_ARP__
//#include <sys/arp.h>
#define __USE_SYS_UDP__
#include <sys/udp.h>
#define __USE_SYS_IFNET__
#include <sys/ifnet.h>
#define __USE_SYS_ETHARP__
#include <sys/etharp.h>
#define __USE_SYS_ROUTE__
#include <sys/route.h>

#include <sys/ip.h>

#include <tcpip/icmp.h>

#define ICMP_MAXTYPE NR_ICMP_TYPES

#ifndef ENABLE_ICMP_PROTO_STAT
#define ENABLE_ICMP_PROTO_STAT 0
#endif

#if ENABLE_ICMP_PROTO_STAT
struct proto_stat icmp_stat;
#define ICMP_PROTO_STAT_ADD(STAT, VAL) icmp_stat.STAT += (VAL)
#else
#define ICMP_PROTO_STAT_ADD(STAT, VAL)
#endif

int icmp_send(struct iphdr * __ip, struct icmp * __icp, int __len)
{
	struct route * rt;
	uint32_t saddr;
	uint32_t daddr;
	int ret;

	ICMP_PROTO_STAT_ADD(tx_ok, 1);

	__len += ICMP_MINLEN;
	saddr = __ip->daddr;
	daddr = __ip->saddr;

	/* build the ip header (swap addresses) */
	iph_template(__ip, IPPROTO_ICMP, ip_defttl, ip_deftos);
	mk_iphdr(__ip, saddr, daddr, __len);

	__icp->icmp_chksum = 0;
	__icp->icmp_chksum = ~in_chksum(0, (uint8_t *)__icp, __len);

	DCC_LOG3(LOG_INFO, "ICMP %I > %I (%d)", 
			 __ip->saddr, __ip->daddr, __len); 

	if ((rt = __route_lookup(daddr)) == NULL) {
		ICMP_PROTO_STAT_ADD(tx_drop, 1);
		DCC_LOG1(LOG_WARNING, "no route to host: %I", daddr);
		return -1;
	}

	if ((ret = ip_output(rt->rt_ifn, rt, __ip)) < 0) {
		DCC_LOG(LOG_ERROR, "ip_output() fail!");
		/* XXX:  */
		ICMP_PROTO_STAT_ADD(tx_drop, 1);

		if (ret == -EAGAIN) {
			/* FIXME: non ethernet type interfaces */
			etharp_query_pending();
		}
		return -1;
	}

	return 0;
}

/*
  return value:
    < 0 : error not processed.
    = 0 : ok processed, packet can be released.
    > 0 : ok processed, packet reused, don't release.
*/
int icmp_echoreplay(struct ifnet * __if, struct iphdr * __ip, 
					struct icmphdr * __icmp, int __len)
{
	struct ifnet * ifn;
	struct route * rt;
	unsigned int tmp;
//	int pkt_len;
	int ret;

	ICMP_PROTO_STAT_ADD(tx_ok, 1);

	__icmp->type = ICMP_ECHOREPLY;

	/* adjust the checksum */
    if (__icmp->chksum >= HTONS((0xffff - (ICMP_ECHO << 8)))) {
		__icmp->chksum += HTONS(ICMP_ECHO << 8) + 1;
	} else {
		 __icmp->chksum += + HTONS(ICMP_ECHO << 8);
	}

	if ((rt = __route_lookup(__ip->daddr)) == NULL) {
		DCC_LOG1(LOG_WARNING, "no route to host: %I", __ip->daddr);
		ICMP_PROTO_STAT_ADD(tx_drop, 1);
		return -1;
	}
	ifn = (struct ifnet *)rt->rt_ifn;

	/* swap addresses in the ip header */
	tmp = __ip->saddr;
	__ip->saddr = __ip->daddr;
	__ip->daddr = tmp;

#if 0
	struct iphdr * ip;
	pkt_len = sizeof(struct iphdr) + sizeof(struct icmphdr) + __len;
	ip = (struct iphdr *)ifn_mmap(ifn, pkt_len);
	if (ip == NULL) {
		return -1;
	}

	memcpy(ip, __ip, pkt_len);
#endif

	if ((ret = ip_output(ifn, rt, __ip)) < 0) {
//		ifn_munmap(ifn, ip);
		DCC_LOG(LOG_ERROR, "ip_output() fail!");
		ICMP_PROTO_STAT_ADD(tx_drop, 1);
		if (ret == -EAGAIN) {
			/* FIXME: non ethernet type interfaces */
			etharp_query_pending();
		}
		return -1;
	}
	
	return 1;
}

#if (ENABLE_NET_UDP)
static void * icmp_skip_ip_hdr(struct iphdr * ip, int frag_len)
{
	int hdr_len;

	if (ip->ver != 4) {
		/* unsupported version: not ipv4 */
		return NULL;
	}

	/* header length  */
	hdr_len = (ip->hlen) << 2;
	if (hdr_len > frag_len) {
		/* fragment too short */
		return NULL;
	}

	return (void *)&ip->opt[hdr_len - sizeof(struct iphdr)];
}
#endif

/*
  return value:
    -1 : error not processed.
     0 : ok processed, packet can be released.
     1 : ok processed, packet reused, don't release.
*/
int icmp_input(struct ifnet * __if, struct iphdr * __ip, 
			   struct icmphdr * __icmp, int __len)
{
	int ret;

	__len -= ICMP_MINLEN;

	ICMP_PROTO_STAT_ADD(rx_ok, 1);

	switch (__icmp->type) {
	case ICMP_ECHO: 
		DCC_LOG(LOG_TRACE, "ICMP: echo request");
		ret = icmp_echoreplay(__if, __ip, __icmp, __len);
		break;

	case ICMP_DEST_UNREACH:
		DCC_LOG(LOG_TRACE, "ICMP: dest unreach");
#if (ENABLE_NET_UDP)
		struct icmp * icp = (struct icmp *)__icmp;
		struct udphdr * udp;
		udp = (struct udphdr *)icmp_skip_ip_hdr(&icp->icmp_ip, __len);
		if ((icp->icmp_ip.proto == IPPROTO_UDP) && (udp != NULL) && 
			(icp->icmp_code == ICMP_PORT_UNREACH)) {
			DCC_LOG1(LOG_WARNING, "UDP port unreach: %d", ntohs(udp->dport));
			udp_port_unreach(icp->icmp_ip.daddr, udp->dport, 
							 icp->icmp_ip.saddr, udp->sport);
		}
#endif
		ret = 0;
		break;

	default:
		ICMP_PROTO_STAT_ADD(rx_drop, 1);
		DCC_LOG2(LOG_INFO, "ICMP: %d (%d)", __icmp->type, __len);
		ret = -1;
	}

	return ret;
} 

int icmp_error(struct iphdr * __ip, int __type, 
			   int __code, struct ifnet * __if) 
{
//	uint8_t icmp_buf[sizeof(struct icmp) + IP_MAXOPTLEN + 8];
	struct icmp * icp;
	int len;

	/* TODO: 
	if (type != ICMP_REDIRECT)
		icmpstat.icps_error++; */

	/* Don't send error if not the first fragment 
	   Ref.: TCP/IP Illustrated Volume 2, pg. 325 */
	icp = (struct icmp *)__ip->opt;

	if (__ip->ip_off & ~(IP_MF | IP_DF)) {
		DCC_LOG1(LOG_WARNING, "not first fragment!; ip_off = %04x", 
				 __ip->ip_off);
		return -1;
	}

	len = MIN((__ip->hlen << 2) + 8 , ntohs(__ip->tot_len));
	memcpy((void *)&(icp->icmp_ip), __ip, len);

	/*
	if ((ip->proto == IPPROTO_ICMP) && (type != ICMP_REDIRECT) &&
	    (totlen >= iplen + ICMP_MINLEN) ) {
	 TODO: 
		icmpstat.icps_oldicmp++; 
		return -1;
	} */

	/* Don't send error in response to a multicast or broadcast packet */
	if (in_broadcast(__ip->daddr, __if) || IN_MULTICAST(__ip->daddr)) {
		DCC_LOG(LOG_INFO, "broadcast/multicast!");
		return -1;
	}

	if ((unsigned int)__type > ICMP_MAXTYPE) {
		DCC_LOG(LOG_PANIC, "icmp_error");
		return -1;
	}

	/* TODO: 
		icmpstat.icps_outhist[type]++; */

	icp->icmp_type = __type;
	icp->icmp_void = 0;
	if (__type == ICMP_PARAMETERPROB)  {
		icp->icmp_pptr = __code;
		__code = 0;
	} else {
		if ((__type == ICMP_DEST_UNREACH) && 
		    (__code == ICMP_FRAG_NEEDED) && (__if)) {
			icp->icmp_nextmtu = htons(__if->if_mtu);
		}
	}
	icp->icmp_code = __code;

	return icmp_send(__ip, icp, len);
}


void icmp_proto_getstat(struct proto_stat * __st, int __rst)
{
#if ENABLE_ICMP_PROTO_STAT
	proto_stat_copy(__st, &icmp_stat, __rst);
#else
	memset(__st, 0, sizeof(struct proto_stat)); 
#endif
}

