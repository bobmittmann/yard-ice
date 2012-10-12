/* $Id: raw.c,v 2.1 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	raw_sendto.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_RAW__
#include <sys/raw.h>

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#define __USE_SYS_ARP__
#include <sys/arp.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>

int raw_sendto(struct raw_pcb * __raw, void * __buf, int __len, 
			   const struct sockaddr_in * __sin)
{
	struct iphdr * ip;
	in_addr_t daddr;
	in_addr_t saddr;
	struct route * rt;
	uint8_t * ptr;
	int mtu;
	struct ifnet * ifn;

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

	if ((rt = route_lookup(daddr)) == NULL) {
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

	if (ip_output(rt, ip) < 0) {
		DCC_LOG1(LOG_ERROR, "<%05x> ip_output() fail!", (int)__raw);
		/* if the reason to fail was an arp failure
		   try query an address pending for resolution ... */
		arp_query_pending();
		tcpip_net_unlock();
		return -1;
	}

	tcpip_net_unlock();

	return __len;
}

