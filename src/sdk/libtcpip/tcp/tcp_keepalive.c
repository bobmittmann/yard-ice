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
 * @file tcp_keepalive.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>
#include <tcpip/ip.h>
#include <tcpip/tcp.h>

/*
 * Send a TCP keepalive probe. 
 * This function is called from the tcp_keep_tmr() timer. 
 */
int tcp_keepalive(struct tcp_pcb * tp)
{
	struct iphdr * iph;
	struct tcphdr * th;
	uint32_t daddr;
	uint32_t saddr;
	struct route * rt;
	unsigned int sum;
	struct ifnet * ifn;
	int ret;

	daddr = tp->t_faddr;
	saddr = tp->t_laddr;

	if ((rt = __route_lookup(daddr)) == NULL) {
		DCC_LOG2(LOG_WARNING, "<%05x> no route to host: %I", (int)tp, daddr);
		TCP_PROTO_STAT_ADD(tx_err, 1);
		return -1;
	}

	ifn = (struct ifnet *)rt->rt_ifn;
	iph = (struct iphdr *)ifn_mmap(ifn, sizeof(struct iphdr) + 
								   sizeof(struct tcphdr));

	iph_template(iph, IPPROTO_TCP, ip_defttl, ip_deftos);
	mk_iphdr(iph, saddr, daddr, sizeof(struct tcphdr));
	th = (struct tcphdr *)iph->opt;

	/* Fill in TCP fields */
	th->th_sport = tp->t_lport;
	th->th_dport = tp->t_fport;
	th->th_seq = htonl(tp->snd_seq - + 1);
	th->th_ack = htonl(tp->rcv_nxt);		
	th->th_off = sizeof(struct tcphdr) >> 2;
	th->th_x2 = 0;
	th->th_flags = TH_ACK;
	th->th_win = htons(tcp_maxrcv - tp->rcv_q.len);
	th->th_urp = 0;
	th->th_sum = 0;

	sum = ntohs(sizeof(struct tcphdr)) + (IPPROTO_TCP << 8);
/*	sum += ((uint16_t *)&(iph->saddr))[0];
	sum	+= ((uint16_t *)&(iph->saddr))[1];
	sum += ((uint16_t *)&(iph->daddr))[0];
	sum += ((uint16_t *)&(iph->daddr))[1]; */

	sum += (iph->saddr & 0xffff) + (iph->saddr >> 16);
	sum += (iph->daddr & 0xffff) + (iph->daddr >> 16);

	th->th_sum = ~in_chksum(sum, (uint16_t *)th, sizeof(struct tcphdr));

//	if (!(tp->t_route)) {
//		tp->t_route = rt;
//	} else
//		rt = tp->t_route;

	TCP_PROTO_STAT_ADD(tx_ok, 1);

#if (ENABLE_TCPDUMP)
	tcp_dump(iph, th, TCPDUMP_TX);
#endif

	if ((ret = (ip_output(ifn, rt, iph)) < 0)) {
		DCC_LOG3(LOG_ERROR, "ip_output(): > %I:%d %s (0)", 
				 iph->daddr, ntohs(th->th_dport), 
				 tcp_all_flags[th->th_flags]);
		TCP_PROTO_STAT_ADD(tx_drop, 1);
		return ret;
	}

	return 0;
}

