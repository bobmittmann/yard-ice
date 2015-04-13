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
 * @file raw_sendto.c
 * @brief IP layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_RAW__
#include <sys/raw.h>

#include <sys/etharp.h>
#include <sys/ip.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>

int raw_sendto(struct raw_pcb * __raw, void * __buf, int __len, 
			   const struct sockaddr_in * __sin)
{
	struct ifnet * ifn;
	struct iphdr * ip;
	in_addr_t daddr;
	in_addr_t saddr;
	struct route * rt;
	uint8_t * ptr;
	int ret;
	int mtu;

	DCC_LOG2(LOG_TRACE, "<%05x> len=%d", (int)__raw, __len);
	
	tcpip_net_lock();

	DCC_LOG2(LOG_INFO, "<%05x> lock [%d]", (int)__raw, net_mutex);
	
	if (__raw == NULL) {
		DCC_LOG(LOG_WARNING, "invalid pcb");
		tcpip_net_unlock();
		return -EFAULT;
	}

	if (__buf == NULL) {
		DCC_LOG(LOG_WARNING, "invalid buffer");
		tcpip_net_unlock();
		return -EFAULT;
	}

	daddr = __sin->sin_addr.s_addr;

	if ((rt = __route_lookup(daddr)) == NULL) {
		DCC_LOG1(LOG_WARNING, "no route to host: %I", daddr);
		tcpip_net_unlock();
		return -1;
	}

	ifn = (struct ifnet *)rt->rt_ifn;
	mtu = ifn->if_mtu - sizeof(struct iphdr);

	if ((__len <= 0) || (__len > mtu)) {
		DCC_LOG3(LOG_WARNING, "<%05x> invalid length %d (max: %d)", (int)__raw, 
			__len, __raw->r_mtu);
		tcpip_net_unlock();
		return 0;
	}


	/* get the source address */
	if ((saddr = __raw->r_laddr) == INADDR_ANY) {
		saddr = ifn->if_ipv4_addr;
	}

	ip = (struct iphdr *)ifn_mmap(ifn, sizeof(struct iphdr) + __len);
	iph_template(ip, __raw->r_protocol, ip_defttl, __raw->r_tos);
	ptr = (uint8_t *)ip->opt;
	
	/* build the ip header */
	mk_iphdr(ip, saddr, daddr, __len);

	memcpy(ptr, __buf, __len);

	DCC_LOG3(LOG_TRACE, "IP %I > %I (%d)", ip->saddr, ip->daddr, __len); 

	if ((ret = ip_output(ifn, rt, ip)) < 0) {
		ifn_munmap(ifn, ip);
		DCC_LOG1(LOG_ERROR, "<%05x> ip_output() fail!", (int)__raw);
		/* if the reason to fail was an arp failure
		   try query an address pending for resolution ... */

		if (ret == -EAGAIN) {
			/* FIXME: non ethernet type interfaces */
			etharp_query_pending();
		}

		tcpip_net_unlock();
		return -1;
	}

	tcpip_net_unlock();

	return __len;
}

