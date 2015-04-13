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
 * @file udp_sendto.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <sys/etharp.h>

#include <tcpip/udp.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>

int udp_bcast(struct udp_pcb * __up, void * __buf, int __len, 
			  struct ifnet * __ifn, unsigned int __dport)
{
	struct iphdr * ip;
	struct udphdr * uh;
	in_addr_t daddr;
	int dport;
	in_addr_t saddr;
#if (ENABLE_NET_UDP_CHECKSUM)
	unsigned int sum;
#endif
	uint8_t * ptr;
	int mtu;
	int ret;

	DCC_LOG2(LOG_INFO, "<%05x> len=%d", (int)__up, __len);
	
	if (__up == NULL) {
		DCC_LOG1(LOG_WARNING, "<%05x> invalid pcb", (int)__up);
		return -EFAULT;
	}

	if (__buf == NULL) {
		DCC_LOG1(LOG_WARNING, "<%05x> invalid buffer", (int)__up);
		return -EFAULT;
	}

	tcpip_net_lock();

#if (ENABLE_NET_SANITY_CHECK)
	if (pcb_find((struct pcb *)__up, &__udp__.pcb) < 0) {
		DCC_LOG1(LOG_ERROR, "<%05x> pcb_find()", (int)__up);
		tcpip_net_unlock();
		/* TODO: errno */
		return -1;
	}
#endif

	if ((__up->u_lport) == 0) {
		DCC_LOG1(LOG_WARNING, "<%05x> not bound", (int)__up);
		tcpip_net_unlock();
		/* TODO: errno */
		return -3;
	}

	if ((dport = __dport) == 0) {
		DCC_LOG1(LOG_WARNING, "<%05x> connection refused", (int)__up);
		return -ECONNREFUSED;
	}

	daddr = INADDR_BROADCAST;
	mtu = __ifn->if_mtu - sizeof(struct iphdr);

	if ((__len <= 0) || (__len > mtu)) {
		DCC_LOG3(LOG_WARNING, "<%04x> invalid length %d (max: %d)", 
				 (int)__up, __len, mtu);
		tcpip_net_unlock();
		/* TODO: errno */
		return -7;
	}

	/* get the source address */
	if ((saddr = __up->u_laddr) == INADDR_ANY) {
		saddr = __ifn->if_ipv4_addr;
	}

	ip = (struct iphdr *)ifn_mmap(__ifn, sizeof(struct iphdr) + 
								  sizeof(struct udphdr) + __len);

	if (ip == NULL) {
		DCC_LOG1(LOG_WARNING, "<%04x> ifn_mmap() fail", (int)__up);
		tcpip_net_unlock();
		/* TODO: errno */
		return -1;
	}

	iph_template(ip, IPPROTO_UDP, udp_def_ttl, udp_def_tos);
	uh = (struct udphdr *)ip->opt;
	
	/* build the ip header */
	ip = mk_iphdr(ip, saddr, daddr, sizeof(struct udphdr) + __len);

	/* fill the udp header fields */
	uh = (struct udphdr *)ip->opt;
	uh->dport = dport;
	uh->sport = __up->u_lport;
	uh->len = htons(__len + sizeof(struct udphdr));
#if (ENABLE_NET_UDP_CHECKSUM)
	/* initialize the udp checksum */
	sum = uh->len << 1;
	sum += uh->dport;
	sum += uh->sport;
	sum += (IPPROTO_UDP << 8);
	sum += ((uint16_t *)(void *)&(ip->saddr))[0] + 
		((uint16_t *)(void *)&(ip->saddr))[1];
	sum += ((uint16_t *)(void *)&(ip->daddr))[0] + 
		((uint16_t *)(void *)&(ip->daddr))[1];
#endif
	
	ptr = (uint8_t *)uh + sizeof(struct udphdr);
	memcpy(ptr, __buf, __len);

#if (ENABLE_NET_UDP_CHECKSUM)
	if (__len) {
		sum = in_chksum(sum, ptr, __len);
	}
	uh->chksum = ~sum;
#else
	uh->chksum = 0;
#endif

#if 0
	DCC_LOG(LOG_INFO, "IP %d.%d.%d.%d:%d > %d.%d.%d.%d:%d: %d", 
		IP4_ADDR1(ip->saddr), IP4_ADDR2(ip->saddr), IP4_ADDR3(ip->saddr), 
		IP4_ADDR4(ip->saddr), ntohs(uh->sport), IP4_ADDR1(ip->daddr), 
		IP4_ADDR2(ip->daddr), IP4_ADDR3(ip->daddr), IP4_ADDR4(ip->daddr), 
		ntohs(uh->dport), ntohs(uh->len)); 
#endif

	if ((ret = ip_output(__ifn, NULL, ip)) < 0) {
		UDP_PROTO_STAT_ADD(tx_drop, 1);
		ifn_munmap(__ifn, ip);
		DCC_LOG1(LOG_ERROR, "<%05x> ip_output() fail!", (int)__up);
	} else {
		UDP_PROTO_STAT_ADD(tx_ok, 1);
		DCC_LOG5(LOG_INFO, "IP %I:%d > %I:%d: %d", 
				 ip->saddr, ntohs(uh->sport), ip->daddr, 
				 ntohs(uh->dport), ntohs(uh->len)); 
#if (LOG_LEVEL < LOG_INFO)
		DCC_LOG(LOG_INFO, "sent.");
#endif
		ret = __len;
	}

	tcpip_net_unlock();

	return ret;
}


