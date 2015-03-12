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
 * @file tcp_dump.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

#include <tcpip/tcp.h>

#ifndef TCPDUMP_SEQ_TAB_MAX
#define TCPDUMP_SEQ_TAB_MAX 16
#endif

struct {
	in_addr_t addr;
	unsigned int port;
	uint32_t seq;
} offtab[TCPDUMP_SEQ_TAB_MAX];

static uint32_t offset_seq(in_addr_t addr, unsigned int port, uint32_t seq)
{
	int i;

	for (i = 0; i < TCPDUMP_SEQ_TAB_MAX; i++) {
		if ((offtab[i].addr == addr) && (offtab[i].port == port)) {
			return seq - offtab[i].seq;
		}
	}

	return seq;
}

static void offset_set(in_addr_t addr, unsigned int port, uint32_t seq)
{
	int i;

	for (i = 0; i < TCPDUMP_SEQ_TAB_MAX; i++) {
		if ((offtab[i].addr == addr) && (offtab[i].port == port)) {
			offtab[i].seq = seq;
			return;
		}
	}

	for (i = 0; i < TCPDUMP_SEQ_TAB_MAX; i++) {
		if (offtab[i].addr == INADDR_ANY) {
			offtab[i].addr = addr;
			offtab[i].port = port;
			offtab[i].seq = seq;
			return;
		}
	}
}

static void rx(struct iphdr * iph, struct tcphdr * th)
{
	int ip_hlen;
	int ip_tlen;
	unsigned int optlen;
	unsigned int datalen;
	unsigned int seq;
	unsigned int ack;

	/* ip header length  */
	ip_hlen = (iph->hlen) << 2;
	/* ip transport len */
	ip_tlen = ntohs(iph->tot_len) - ip_hlen;
	optlen = ((th->th_off << 2) - sizeof(struct tcphdr));
	/* tcp data len */
	datalen = ip_tlen - (sizeof(struct tcphdr) + optlen);

	if (th->th_flags & TH_SYN) {
		offset_set(iph->saddr, th->th_sport, ntohl(th->th_seq));
	}
	seq = offset_seq(iph->saddr, th->th_sport, ntohl(th->th_seq)); 

	if (th->th_flags & TH_ACK) {
		ack = offset_seq(iph->daddr, th->th_dport, ntohl(th->th_ack));
		if (datalen){ 
			DCC_LOG10(LOG_TRACE, 
					  "TCP %I:%d > %I:%d %s %u:%u(%d) ack %u win %d", 
					  iph->saddr, ntohs(th->th_sport),
					  iph->daddr, ntohs(th->th_dport),
					  tcp_basic_flags[th->th_flags & 0x0f], 
					  seq, seq + datalen, datalen, 
					  ack, ntohs(th->th_win));
		} else {
			DCC_LOG8(LOG_TRACE, 
					 "TCP %I:%d > %I:%d %s %u(0) ack %u win %d", 
					 iph->saddr, ntohs(th->th_sport),
					 iph->daddr, ntohs(th->th_dport),
					 tcp_basic_flags[th->th_flags & 0x0f], 
					 seq, ack, ntohs(th->th_win));
		}
	} else {
		DCC_LOG8(LOG_TRACE, 
				 "TCP %I:%d > %I:%d %s %u(%d) win %d", 
				 iph->saddr, ntohs(th->th_sport),
				 iph->daddr, ntohs(th->th_dport),
				 tcp_basic_flags[th->th_flags & 0x0f], 
				 seq, datalen, ntohs(th->th_win));
	}

	(void)ack;
	(void)seq;
}

static void tx(struct iphdr * iph, struct tcphdr * th)
{

	int ip_hlen;
	int ip_tlen;
	unsigned int optlen;
	unsigned int datalen;
	unsigned int seq;
	unsigned int ack;

	/* ip header length  */
	ip_hlen = (iph->hlen) << 2;
	/* ip transport len */
	ip_tlen = ntohs(iph->tot_len) - ip_hlen;
	optlen = ((th->th_off << 2) - sizeof(struct tcphdr));
	/* tcp data len */
	datalen = ip_tlen - (sizeof(struct tcphdr) + optlen);

	if (th->th_flags & TH_SYN) {
		offset_set(iph->saddr, th->th_sport, ntohl(th->th_seq));
	}
	seq = offset_seq(iph->saddr, th->th_sport, ntohl(th->th_seq)); 

	if (th->th_flags & TH_ACK) {
		ack = offset_seq(iph->daddr, th->th_dport, ntohl(th->th_ack));
		if (datalen){ 
			DCC_LOG10(LOG_TRACE, 
					  "TCP %I:%d > %I:%d %s %u:%u(%d) ack %u win %d", 
					  iph->saddr, ntohs(th->th_sport),
					  iph->daddr, ntohs(th->th_dport),
					  tcp_basic_flags[th->th_flags & 0x0f], 
					  seq, seq + datalen, datalen, 
					  ack, ntohs(th->th_win));
		} else {
			DCC_LOG8(LOG_TRACE, 
					 "TCP %I:%d > %I:%d %s %u(0) ack %u win %d", 
					 iph->saddr, ntohs(th->th_sport),
					 iph->daddr, ntohs(th->th_dport),
					 tcp_basic_flags[th->th_flags & 0x0f], 
					 seq, ack, ntohs(th->th_win));
		}
	} else {
		DCC_LOG8(LOG_TRACE, 
				 "TCP %I:%d > %I:%d %s %u(%d) win %d", 
				 iph->saddr, ntohs(th->th_sport),
				 iph->daddr, ntohs(th->th_dport),
				 tcp_basic_flags[th->th_flags & 0x0f], 
				 seq, datalen, ntohs(th->th_win));
	}
	(void)ack;
	(void)seq;
}

int tcp_dump(struct iphdr * iph, struct tcphdr * th, int dir)
{
	if (iph->ver != 4)
		return -1;

	if (dir == TCPDUMP_RX)
		rx(iph, th);
	else
		tx(iph, th);

	return 0;
}

