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
 * @file tcp_out.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>
#include <sys/mbuf.h>
#include <tcpip/tcp.h>
#include <string.h>

#ifndef ENABLE_TCPCHKSUMCHECK
#define ENABLE_TCPCHKSUMCHECK 0
#endif

/* the values here are in (miliseconds / TCP_SLOW_MS_TICK) */
extern const uint8_t tcp_rxmtintvl[];

const uint8_t tcp_outflags[11] = {
	/* CLOSED */
	TH_RST | TH_ACK,
	/* LISTEN */
	0,
	/* SYN_SENT */
	TH_SYN,
	/* SYN_RCVD */
	TH_SYN | TH_ACK,
	/* ESTABLISHED */
	TH_ACK,
	/* CLOSE_WAIT */
	TH_ACK,
	/* FIN_WAIT_1 */
	TH_FIN | TH_ACK,
	/* FIN_CLOSING */
	TH_FIN | TH_ACK,
	/* LAST_ACK */
	TH_FIN | TH_ACK,
	/* FIN_WAIT_2 */
	TH_ACK,
	/* TIME_WAIT */
	TH_ACK
};

int tcp_respond(struct iphdr * iph, struct tcphdr * th, 
	tcp_seq ack, tcp_seq seq, int flags)
{
	struct route * rt;
	unsigned int sum;
	uint32_t daddr;
	uint32_t saddr;
	int sport;
	int dport;
	int ret;

	/* exchange source and destination addresses */
	daddr = iph->saddr;
	saddr = iph->daddr;
	dport = th->th_sport;
	sport = th->th_dport;

	iph_template(iph, IPPROTO_TCP, ip_defttl, ip_deftos);
	mk_iphdr(iph, saddr, daddr, sizeof(struct tcphdr));

	th = (struct tcphdr *)iph->opt;

	/* Fill in TCP fields */
	th->th_sport = sport;
	th->th_dport = dport;
	th->th_seq = htonl(seq);		
	th->th_ack = htonl(ack);
	th->th_off = sizeof(struct tcphdr) >> 2;
	th->th_x2 = 0;
	th->th_flags = flags;

	/* TODO: Calculate receive window. Don't shrink window,
	   but avoid silly window syndrome. */
	th->th_win = 0;
	th->th_sum = 0;
	th->th_urp = 0;

	sum = ntohs(sizeof(struct tcphdr)) + (IPPROTO_TCP << 8);
	sum += (iph->saddr & 0xffff) + (iph->saddr >> 16);
	sum += (iph->daddr & 0xffff) + (iph->daddr >> 16);
	th->th_sum = ~in_chksum(sum, (uint16_t *)th, sizeof(struct tcphdr));

	if ((rt = __route_lookup(daddr)) == NULL) {
		DCC_LOG1(LOG_WARNING, "<____> no route to host: %I", daddr);
		TCP_PROTO_STAT_ADD(tx_err, 1);
		return -1;
	}

#if (ENABLE_TCPDUMP)
	tcp_dump(iph, th, TCPDUMP_TX);
#endif

	if ((ret = ip_output(rt->rt_ifn, rt, iph)) < 0) {
		DCC_LOG(LOG_ERROR, "<____> ip_output() fail!");
		TCP_PROTO_STAT_ADD(tx_drop, 1);
		return ret;
	}

	TCP_PROTO_STAT_ADD(tx_ok, 1);

	return 1;
}

static int tcp_mss(struct tcp_pcb * tp, unsigned int offer)
{
	struct route * rt;
	int mss;

	if ((rt = tp->t_route) != NULL) {
		mss = rt->rt_ifn->if_mtu - (sizeof(struct tcphdr) + 
									sizeof(struct iphdr));
		if (mss > tcp_maxmss)
			mss = tcp_maxmss;
	} else
		mss = tcp_defmss;

	return mss;
}

#define _HTONL_(TO, FROM) ({ uint8_t * t = ((uint8_t *)(TO)); \
	uint8_t * f = ((uint8_t *)(FROM)); t[0] = f[3]; t[3] = f[0]; \
	t[1] = f[2];  t[2] = f[1];  })

#if (ENABLE_TCPCHKSUMCHECK)
void checksum_check(struct tcp_pcb * tp, struct iphdr * iph,
					struct mlink * m)
{
	uint8_t buf[1024];
	struct mlink * p;
	uint8_t * cp;
	int n;
	int cnt;
	int sum;
	int len;
	uint16_t * ptr;

	len = ntohs(iph->tot_len) - IP_HEADER_LEN;

	sum = htons(len) + (IPPROTO_TCP << 8);
	sum += ((uint16_t *)&(iph->saddr))[0];
	sum += ((uint16_t *)&(iph->saddr))[1];
	sum += ((uint16_t *)&(iph->daddr))[0];
	sum += ((uint16_t *)&(iph->daddr))[1];

	n = 0;
	cp = buf;
	cnt = len;
	p = m;
	/* copy into linear buffer */
	do {
		n = MIN((int)(p->len), cnt);
		memcpy(cp, p->ptr, n);
		cp += n;
		cnt -= n;
		p = MLINK_NEXT(p);
	} while (cnt);

	/* padding */
	if (len & 1) {
		*cp = 0;
		n = (len + 1) >> 1;
	} else
		n = len >> 1;

	/* summing */
	ptr = (uint16_t *)buf;
	while (n--)
		sum += *ptr++;

//	sum = in_chksum(sum, buf, len);
	/* folding */
	while (sum > 0xffff)
		sum = (sum & 0xffff) + (sum >> 16);

	if (sum != 0xffff)
		DCC_LOG1(LOG_WARNING, "<%05x> checksum ERROR!", (int)tp);
	else
		DCC_LOG1(LOG_INFO, "<%05x> checksum OK.", (int)tp);
}
#endif

int tcp_output(struct tcp_pcb * tp)
{
	struct iphdr * iph;
	struct tcphdr * th;
	int flags;
	unsigned int optlen;
	uint8_t opt[16];
	unsigned int hdrlen;
	uint32_t daddr;
	uint32_t saddr;
	struct route * rt;
	unsigned int sum;
	uint32_t snd_una;
	uint32_t snd_nxt;
	uint32_t snd_max;
	int off;
	int win;
	int w_rem;
	int q_rem;
	int len;
	uint8_t * data;
	int sendalot;
	int idle;
	int ret;

	/* TODO: slow start algorithm */
	/* TODO: congestion avoidance */
	/* TODO: silly window avoidance */

	if (tp->t_state != TCPS_ESTABLISHED) {
		DCC_LOG3(LOG_INFO, "<%05x> %I:%d", (int)tp,
				 tp->t_faddr, ntohs(tp->t_fport));
	}

	snd_una = tp->snd_seq;
	snd_nxt = tp->snd_seq + tp->snd_off;
	snd_max = tp->snd_seq + tp->snd_max;

	DCC_LOG3(LOG_INFO, "snd_nxt=%u snd_una=%u snd_len=%d", 
			 snd_nxt, snd_una, tp->snd_q.len);

	/*  Ref.: TCP/IP Illustrated Volume 2, pg. 853 */
	idle = (snd_max == snd_una);

again:
	sendalot = 0;

	/* Ref.: TCP/IP Illustrated Volume 2, pg. 854 */
	off = snd_nxt - snd_una;
	w_rem = tp->snd_wnd - off;
	q_rem = tp->snd_q.len - off;

	flags = tcp_outflags[tp->t_state];

	/*
	 * TODO:
	 * If in persist timeout with window of 0, send 1 byte. ...
	 *
	 */

	/* Ref.: TCP/IP Illustrated Volume 2, pg. 855 */
	len = MIN(w_rem, q_rem);

	DCC_LOG2(LOG_INFO, "queue=%d window=%d", q_rem, w_rem);

	if (len < 0) {
		/*
		 * If FIN has been sent but not acked,
		 * but we haven't been called to retransmit,
		 * len will be -1. ...
		 */
		len = 0;
		if (w_rem == 0) {
			DCC_LOG(LOG_INFO, "window closed, stop rxmt tmr");
			tp->t_rxmt_tmr = 0;
			snd_nxt = snd_una;
			tp->snd_off = 0;
		}
	}

	if (len > tp->t_maxseg) {
		len = tp->t_maxseg;
		sendalot = 1;
	}

	if (SEQ_LT(snd_nxt + len, snd_una + tp->snd_q.len)) {
		DCC_LOG(LOG_INFO, "removing FIN"); 
		flags &= ~TH_FIN;
	}

	/* space left in the input queue */
	win = tcp_maxrcv - tp->rcv_q.len;

	/* 
	 * Sender silly window avoidance. ...
	   Ref.: TCP/IP Illustrated Volume 2, pg. 859 */
	if (len) {
		if (len == tp->t_maxseg) {
			DCC_LOG1(LOG_INFO, "len == maxseg (%d), send...", len); 
			goto send;
		}

/*		
		XXX: the TF_NODELAY flag was never set and was
		removed 
		if ((idle || (tp->t_flags & TF_NODELAY)) && 
			(len + off >= tp->snd_q.len)) {
*/
		if ((idle) && (len >= q_rem)) {
			if (idle) {
				DCC_LOG(LOG_INFO, "idle, send..."); 
			} else  {
				DCC_LOG(LOG_INFO, "no delay, send..."); 
			}
			goto send;
		} 

/*		if (tp->t_force)
			goto send; */

/*		if (len >= tp->max_sndwnd / 2) {
			goto send;
		} */

		if (len >= tp->snd_wnd / 2) {
			goto send;
		}

#if 0
		/* small output buffer */
		if (len >= (tp->snd_q.len / 2)) {
			DCC_LOG(LOG_INFO,"small buffer"); 
			goto send;
		}
#endif

		if (SEQ_LT(snd_nxt, snd_max)) {
			DCC_LOG(LOG_INFO, "retransmission, send..."); 
			goto send;
		}
	}

	DCC_LOG2(LOG_INFO, "<%05x> win=%d", (int)tp, win);

	/*  Check if a window update should be sent
	   Ref.: TCP/IP Illustrated Volume 2, pg. 860 */
	if (win > 0) {
		int adv;

		adv = MIN(win, tcp_maxwin - tp->rcv_adv_wnd);

		if (adv >= (2 * tp->t_maxseg)) {
			DCC_LOG2(LOG_INFO, "<%05x> adv=%d >= 2 * maxseg", (int)tp, adv);
			DCC_LOG(LOG_INFO,"adv >= 2 * maxseg, send..."); 
			goto send;
		}

		if (2 * adv >= tcp_maxrcv) {
			DCC_LOG2(LOG_INFO, "<%05x> adv=%d >= maxrcv/2", (int)tp, adv);
			DCC_LOG(LOG_INFO, "adv >= maxrcv/2, send...");
			goto send;
		}
	}

	/*
	if ((flags & TH_SYN) && (!(flags & TH_ACK))) {
		len = 0;
		goto send;
	} 
	*/

	/* Send if we owe peer an ACK. */
	if (tp->t_flags & TF_ACKNOW) {
		DCC_LOG(LOG_INFO, "ack now, send...");
		goto send;
	}

	if (flags & TH_SYN) {
		DCC_LOG1(LOG_INFO, "<%05x> SYN send...", tp);
		goto send;
	}

	if (flags & TH_RST) {
		DCC_LOG1(LOG_INFO, "<%05x> RST send...", tp);
		goto send;
	}

	/* TODO: urgent data */

	/* if our state indicates that FIN should be sent 
	   and we have not yet done so, or we are retransmitting the FIN,
	   then we need to send. */
	if ((flags & TH_FIN) && 
		(((tp->t_flags & TF_SENTFIN) == 0) || (snd_nxt == snd_una))) {
		if (len) {
			DCC_LOG(LOG_INFO, "data, FIN, send...");
		} else {
			DCC_LOG(LOG_INFO, "FIN, send...");
		}
		goto send;
	}

	/* TODO: persist state ?? */

	DCC_LOG(LOG_INFO, "no reason to send");

	/*
	 * No reason to send a segment, just return. 
	 */
	return 0;

send:
	DCC_LOG5(LOG_INFO, "una=%u nxt=%u max=%u win=%d (%d)", 
			 snd_una, snd_nxt, snd_max, win, len);

	daddr = tp->t_faddr;
	saddr = tp->t_laddr;

	if (!(tp->t_route)) {
		if ((rt = __route_lookup(daddr)) == NULL) {
			DCC_LOG2(LOG_WARNING, "<%05x> no route to host: %I", 
					 (int)tp, daddr);
			/* XXX: ???? */
			TCP_PROTO_STAT_ADD(tx_err, 1);
			return 0;
		}
		tp->t_route = rt;
		DCC_LOG3(LOG_INFO, "<%05x> route to %I: %08x", 
				 (int)tp, daddr, (int)rt);
	} else {
		rt = tp->t_route;
		DCC_LOG2(LOG_INFO, "<%05x> pcb route: %08x", (int)tp, (int)rt);
	}

	/*
     * XXX: we have to request a memory map and compute the
	 * len but we don't know in advance 
	 * the size of TCP options field...
	 */

	if ((sizeof(struct iphdr) + sizeof(struct tcphdr) + len) > 
		rt->rt_ifn->if_mtu) {
		len = rt->rt_ifn->if_mtu - (sizeof(struct iphdr) + 
									sizeof(struct tcphdr));
	}

	/*  Ref.: TCP/IP Illustrated Volume 2, pg. 872 */
	optlen = 0;
	hdrlen = sizeof(struct tcphdr);
	if (flags & TH_SYN) {
		uint16_t * mss;
		/* fill the tcp options */
		opt[0] = TCPOPT_MSS;
		opt[1] = TCPOLEN_MSS;
		mss = (uint16_t *)&opt[2];
		*mss = htons(tcp_mss(tp, 0));
		optlen = 4;
	}
	
	/*  Ref.: TCP/IP Illustrated Volume 2, pg. 873 */
	hdrlen += optlen;

	/*
	 * Adjust data length if insertion of options will
	 * bump the packet length beyound the t_maxseg length.
	 */
	if (len > (tp->t_maxseg - optlen)) {
		len = tp->t_maxseg - optlen;
		sendalot = 1;
	}

	if (len) {
		/* TODO: statistics */
		/*  Ref.: TCP/IP Illustrated Volume 2, pg. 875 */
		/*
		 * If we're sending everithing we've got, set PUSH.
		 */
		if ((off + len) == tp->snd_q.len)
			flags |= TH_PSH;

	} else {
		/* TODO: statistics */
		/*  Ref.: TCP/IP Illustrated Volume 2, pg. 876 */
	}

	/*  Ref.: TCP/IP Illustrated Volume 2, pg. 877 */
	/*
	 * Fill in fields ...
	 */

	if ((flags & TH_FIN) && (tp->t_flags & TF_SENTFIN) && 
		(snd_nxt == snd_max)) {
		snd_nxt--;
	}

	/* TODO: Calculate receive window. Don't shrink window,
	   but avoid silly window syndrome. */
	/*  Ref.: TCP/IP Illustrated Volume 2, pg. 879 */
	if ((win < (tcp_maxrcv / 4)) && (win < tp->t_maxseg))
		win = 0;

	if (win > tcp_maxwin)
		win = tcp_maxwin;

//	if (win < tp->rcv_adv_wnd)
//		win = tp->rcv_adv_wnd;


	iph = ifn_mmap(rt->rt_ifn, sizeof(struct iphdr) + 
				   sizeof(struct tcphdr) + len);

	if (iph == NULL) {
		DCC_LOG1(LOG_ERROR, "<%05x> ifn_mmap() failed!", (int)tp);
		return 0;
	}

	iph_template(iph, IPPROTO_TCP, ip_defttl, ip_deftos);
	th = (struct tcphdr *)iph->opt;

	mk_iphdr(iph, saddr, daddr, hdrlen + len);

	memcpy(th->th_opt, opt, optlen);

	if ((len) || (flags & (TH_SYN | TH_FIN)))
		th->th_seq = htonl(snd_nxt);
	else
		th->th_seq = htonl(snd_max);

	th->th_ack = htonl(tp->rcv_nxt);

	th->th_flags = flags;

	th->th_win = htons(win);

	th->th_sport = tp->t_lport;
	th->th_dport = tp->t_fport;

	th->th_off = hdrlen >> 2;
	th->th_x2 = 0;

	th->th_sum = 0;
	th->th_urp = 0;

	/* initialize the checksum */
	sum = ntohs(hdrlen + len) + (IPPROTO_TCP << 8);
	sum += (iph->saddr & 0xffff) + (iph->saddr >> 16);
	sum += (iph->daddr & 0xffff) + (iph->daddr >> 16);

	sum = in_chksum(sum, (uint16_t *)th, hdrlen);

	data = (uint8_t *)&th->th_opt[optlen];

	if (len > 0) {
		int n;
		n = mbuf_queue_get(&tp->snd_q, data, off, len);
		sum = in_chksum(sum, data, n);
	}

	th->th_sum = ~sum;

#if (ENABLE_TCPCHKSUMCHECK)
	checksum_check(tp, iph, m);
#endif

#if (ENABLE_TCPDUMP)
	tcp_dump(iph, th, TCPDUMP_TX);
#endif

	TCP_PROTO_STAT_ADD(tx_ok, 1);

	DCC_LOG7(LOG_INFO, "%I:%d > %I:%d %s win=%d (%d)", 
			 iph->saddr, ntohs(th->th_sport), 
			 iph->daddr, ntohs(th->th_dport), 
			 tcp_all_flags[th->th_flags], win, len);

	if ((ret = ip_output(rt->rt_ifn, rt, iph)) < 0) {
		DCC_LOG4(LOG_ERROR, "ip_output(): > %I:%d %s (%d)", 
				 iph->daddr, ntohs(th->th_dport), 
				 tcp_all_flags[th->th_flags], len);
		ifn_munmap(rt->rt_ifn, iph);
		/* FIXME: if the reason to fail was an arp failure
		   try to query an address pending for resolution ... */
		TCP_PROTO_STAT_ADD(tx_drop, 1);
		return ret;
	}

	/* In transmit state, time the transmission and arrange for
	   the retransmit.  In persist state, just set snd_max.
	   Ref.: TCP/IP Illustrated Volume 2, pg. 881 */

	/* TODO: perist state ?? */
	if (flags & (TH_SYN | TH_FIN)) {
		if (flags & TH_SYN) {
			snd_nxt++;
		}
		
		if (flags & TH_FIN) {
			snd_nxt++;
			tp->t_flags |= TF_SENTFIN;
		}
	}
	snd_nxt += len;

	/* TODO: perist state ?? */
	if (SEQ_GT(snd_nxt, snd_max)) {
		snd_max = snd_nxt;
		/* TODO: time this transmission if not a retransmission and
		   not currently timing anything */
	}

	/* Set retransmit timer if not currently set,
	   and not doing an ack or a keepalive probe.
	   Initial value for retransmit is smoothed
	   round-trip time + 2 * round-trip time variance.
	   Initialize counter which is used for backoff
	   of retransmit time. */
	/*  Ref.: TCP/IP Illustrated Volume 2, pg. 881 */
	if ((tp->t_rxmt_tmr == 0) && 
		(snd_nxt != snd_una)) {
		DCC_LOG(LOG_INFO, "not ACK or keepalive prob, start rxmt tmr");
		tp->t_rxmt_tmr = tcp_rxmtintvl[0];
		tp->t_rxmt_cnt = 0;
		/* tp->t_flags &= ~TF_IDLE; */
	}

	tp->snd_seq = snd_una;
	tp->snd_off = snd_nxt - tp->snd_seq;
	tp->snd_max = snd_max - tp->snd_seq;

	if (tp->t_state != TCPS_ESTABLISHED) {
		DCC_LOG6(LOG_INFO, "%5u %d->%I:%d %s (%d)", 
				 tcp_rel_timestamp(), ntohs(th->th_sport), 
				 iph->daddr, ntohs(th->th_dport), 
				 tcp_all_flags[th->th_flags], len);
	}

	/* Data sent (as far as we can tell)
	   Ref.: TCP/IP Illustrated Volume 2, pg. 883 */
#if 0
	if ((win > 0) && SEQ_GT(tp->rcv_nxt + win, tp->rcv_adv))
		tp->rcv_adv = tp->rcv_nxt + win;
#endif
//	if ((win > 0) && (win > tp->rcv_adv_wnd))
		tp->rcv_adv_wnd = win;

	tp->t_flags &= ~(TF_ACKNOW | TF_DELACK);

	DCC_LOG5(LOG_INFO, "seq=%u off=%d max=%d snd_q.len=%d rcv_nxt=%u", 
		tp->snd_seq, tp->snd_off, tp->snd_max, tp->snd_q.len, tp->rcv_nxt);

	if (sendalot) {
		DCC_LOG(LOG_INFO, "again...........................................");
		goto again;
	}

	return 0;
}

