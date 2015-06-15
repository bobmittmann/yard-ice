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
 * @file tcp_in.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>
#include <tcpip/tcp.h>

inline struct tcp_pcb * tcp_active_lookup(in_addr_t __faddr, uint16_t __fport, 
										  in_addr_t __laddr, uint16_t __lport)
{

	return (struct tcp_pcb *)pcb_lookup(__faddr, __fport, 
										__laddr, __lport, 
										&__tcp__.active);
}

inline struct tcp_listen_pcb * tcp_listen_lookup(in_addr_t __faddr, 
												 uint16_t __fport, 
												 in_addr_t __laddr, 
												 uint16_t __lport)
{
	return (struct tcp_listen_pcb *)pcb_wildlookup(__faddr, __fport, 
												  __laddr, __lport, 
												  &__tcp__.listen);
}

static void tcp_parse_options(struct tcp_pcb * tp, struct tcphdr * th, 
							  uint8_t * p, int len)
{
	int n = len;
	int opt;
	int val;

	while (len) {
		opt = p[0];
		if (opt == TCPOPT_END) {
			/* end of options */
			DCC_LOG(LOG_MSG, "option END");
			break;
		}

		if (opt == TCPOPT_NOP) {
			DCC_LOG(LOG_MSG, "option NOP");
			n = 1;
		} else {
			n = p[1];
			if (n <= 0) {
				DCC_LOG1(LOG_INFO, "invalid option len: %d", n);
				break;
			}
			switch (opt) {
			case TCPOPT_MSS:
				if ((n == TCPOLEN_MSS) && ((th->th_flags & TH_SYN))) {
					/* FIXME: endianess */
					val = ((int)p[2] << 8) + (int)p[3];
					/* Only set the maximum segment size if lower than 
					   the initialy configured one. */
					if (val < tp->t_maxseg)
						tp->t_maxseg = val;
					DCC_LOG1(LOG_INFO, "option MSS: %d", val);
				}
				break;
			case TCPOPT_WINDOW:
				DCC_LOG(LOG_INFO, "unsuported option WINDOW");
				break;
			case TCPOPT_SACK_PERMITTED:
				DCC_LOG(LOG_INFO, "unsuported option SACK_PERMITTED");
				break;
			case TCPOPT_SACK:
				DCC_LOG(LOG_INFO, "unsuported option SACK");
				break;
			case TCPOPT_TIMESTAMP:
				DCC_LOG(LOG_INFO, "unsuported option TIMESTAMP");
				break;
			default:
				DCC_LOG1(LOG_INFO, "unsuported option: %d", opt);
			}
		}
	
		len -= n;
		p += n;
	};
}

struct tcp_pcb * tcp_passive_open(struct tcp_listen_pcb * mux, 
								  struct iphdr * iph,
								  struct tcphdr * th, 
								  unsigned int optlen)
{
	struct tcp_pcb * tp;
	struct route * rt;
	int new_head;
	int cond;

	new_head = mux->t_head + 1;
	if (new_head == mux->t_max)
		new_head = 0;

	if (mux->t_tail == new_head) {
		DCC_LOG(LOG_WARNING, "backlog limit");
		return NULL;
	}

	if ((cond = thinkos_cond_alloc()) < 0) {
		DCC_LOG(LOG_WARNING, "thinkos_cond_alloc()");
		return NULL;
	}

	if ((tp = tcp_pcb_new(&__tcp__.active)) == NULL) {
		DCC_LOG(LOG_WARNING, "tcp_pcb_new() failed!");
		return NULL;
	}

	/* Set up the new PCB. */
	tp->t_lport = th->th_dport;
	tp->t_laddr = iph->daddr;
	tp->t_fport = th->th_sport; 
	tp->t_faddr = iph->saddr;
	tp->t_cond = cond;
	/*
	 * TODO: max queue size...
	 */
	mbuf_queue_init(&tp->rcv_q);
	mbuf_queue_init(&tp->snd_q);

	if ((rt = __route_lookup(tp->t_faddr)) == NULL) {
		DCC_LOG(LOG_WARNING, "no route to host");			
		tp->t_maxseg = tcp_defmss;
	} else {
		/* default mss to the network interface mtu. */
		tp->t_route = rt;
		tp->t_maxseg = rt->rt_ifn->if_mtu - (sizeof(struct tcphdr) + 
											 sizeof(struct iphdr));
	}

	if (tp->t_maxseg > tcp_maxmss)
		tp->t_maxseg = tcp_maxmss;

	/* TODO: calculate the amount of space in receive window */
//	tp->rcv_wnd = MIN(tcp_maxrcv, tcp_maxwin);
	/* advertised window */
//	tp->t_adv_wnd = 0;

	if (optlen)
		tcp_parse_options(tp, th, th->th_opt, optlen);

	/* update the sequence numbers */
	tp->rcv_nxt = ntohl(th->th_seq) + 1;

	tp->snd_seq = (++__tcp__.iss << 20);
	tp->snd_off = 0;
	tp->snd_max = 0;

	/* set the connection-establishment timer to 75 seconds  */
	tp->t_conn_tmr = tcp_conn_est_tmo;
	tp->snd_wnd = ntohs(th->th_win);

	DCC_LOG2(LOG_INFO, "maxseg=%d snd_wnd=%d", 
			 tp->t_maxseg, tp->snd_wnd);			

	/* TODO: initialization of receive urgent pointer */

	tp->t_state = TCPS_SYN_RCVD;

	DCC_LOG3(LOG_INFO, "<%05x> %I:%d [SYN_RCVD]", 
			 (int)tp, tp->t_faddr, ntohs(tp->t_fport));

	/* XXX: don't respond now - the upper layer must call the tcp_accept()
	   function to send back the SYNC and finish handshaking. */
	tp->t_flags = TF_ACKNOW;

	/* insert into backlog */
	mux->t_backlog[mux->t_head] = tp;
	mux->t_head = new_head;
	thinkos_sem_post(mux->t_sem);

	return tp;
}

/*
  return value:
    -1 : error not processed.
     0 : ok processed, packet can be released.
     1 : ok processed, packet reused, don't release.
*/

int tcp_input(struct ifnet * __if, struct iphdr * iph, 
			   struct tcphdr * th, int len)
{
	struct tcp_listen_pcb * mux;
	struct tcp_pcb * tp;
#if (ENABLE_NET_TCP_CHECKSUM)
	unsigned int sum;
#endif
	int ti_len;
	int acked = 0;
	int ourfinisacked = 0;
	int needoutput = 0;
	unsigned int optlen;
	int tiflags;
	int todrop;
	uint32_t snd_una;
	uint32_t snd_nxt;
	uint32_t snd_max;
	uint32_t ti_seq;
	uint32_t ti_ack;
	int rcv_wnd;
	int tiwin;
	int hdrlen;
	uint8_t * data;
	int ret;

#if (ENABLE_TCPDUMP)
	tcp_dump(iph, th, TCPDUMP_RX);
#endif

	/* get TCP options, if any */
	optlen = ((th->th_off << 2) - sizeof(struct tcphdr));
	hdrlen = sizeof(struct tcphdr) + optlen;

	data = (uint8_t *)&th->th_opt[optlen];
	ti_len = len - hdrlen;
	
#if (ENABLE_NET_TCP_CHECKSUM)
	/* initialize checksum */
	sum = htons(len) + (IPPROTO_TCP << 8);
	sum = in_chksum(sum, &iph->saddr,  8);
	sum = in_chksum(sum, th,  hdrlen);

	if (ti_len) {
		sum = in_chksum(sum, data, ti_len);
	}

	if (sum != 0x0000ffff) {
		DCC_LOG3(LOG_WARNING, "checksum error: 0x%04x hdrlen=%d, len=%d", 
				 sum, hdrlen, len);
		TCP_PROTO_STAT_ADD(rx_err, 1);
		goto drop;
	}
#endif

	tiflags = th->th_flags;
	/* convert TCP protocol specific fields to host format */
	tiwin = ntohs(th->th_win);
	ti_seq = ntohl(th->th_seq);
	ti_ack = ntohl(th->th_ack);

	TCP_PROTO_STAT_ADD(rx_ok, 1);

	/* Serch in active list first */
	if ((tp = tcp_active_lookup(iph->saddr, th->th_sport, 
								iph->daddr, th->th_dport)) == NULL) {
		/* lookup into listening pcb list */
		if ((mux = tcp_listen_lookup(iph->saddr, th->th_sport, 
									 iph->daddr, th->th_dport)) == NULL) {
			DCC_LOG(LOG_WARNING, "invalid peer ???");
			goto dropwithreset;
		}

		if ((tiflags & TH_ACK)) {
			DCC_LOG(LOG_WARNING, "listen ACK ?");
			goto dropwithreset;
		}

		if (ti_len != 0) {
			DCC_LOG(LOG_WARNING, "ti_len != 0");
			goto dropwithreset;
		}

		/* Completion of Passive Open
		   Ref.: TCP/IP Illustrated Volume 2, pg. 942 */
		if (!(tiflags & TH_SYN)) {
			DCC_LOG(LOG_WARNING, "listen !SYN ?");
			goto drop;
		}
	
		/* In the LISTEN state, we check for incoming SYN segments,
		   creates a new PCB, and responds with a SYN|ACK. */
		if ((tiflags & TH_RST)) {
			DCC_LOG(LOG_WARNING, "listen RST?");
			goto drop;
		}

		if ((tp = tcp_passive_open(mux, iph, th, optlen)) == NULL) {
			DCC_LOG(LOG_WARNING, "tcp_passive_open()");
			goto dropwithreset;
		}

		/* schedule output */
		tcp_output_sched(tp);

		/* packet handled */
		return 0;
	}

	DCC_LOG1(LOG_MSG, "<%05x> active", (int)tp);

	snd_una = tp->snd_seq;
	snd_nxt = tp->snd_seq + tp->snd_off;
	snd_max = tp->snd_seq + tp->snd_max;

 	/* Remove acknowledged bytes from the send buffer */
	/* Wakeup processes waiting on send buffer */

	/* Segment received on a connection.
	   Reset the idle detection timer 
	   Ref.: TCP/IP Illustrated Volume 2, pg. 932  */
	tp->t_conn_tmr = tcp_idle_det_tmo;
	if (tp->t_flags & TF_IDLE) {
		/* exits from the idle state */
		tp->t_flags &= ~TF_IDLE;
		DCC_LOG1(LOG_INFO, "<%05x> IDLE exit", (int)tp);		
	}

#if 0
	/* Process options, we don't need to check if the socket is 
	   in the LISTEN state, because only active (non LISTENING) sockets
	   will actually fall into this code. 
	   XXX: options after connection stablished ??? 
	 */
	if (optlen)
		tcp_parse_options(tp, th, th->th_opt, optlen);
#endif

	/* Ref.: TCP/IP Illustrated Volume 2, pg. 934  */
#if (TCP_ENABLE_HEADER_PREDICTION)
	if ((tp->t_state == TCPS_ESTABLISHED) &&
		(tiflags & (TH_SYN | TH_FIN | TH_RST | TH_URG | TH_ACK)) == TH_ACK &&
		(ti_seq == tp->rcv_nxt) && 
		(tiwin) && 
		(tiwin == tp->snd_wnd) && 
		(snd_nxt == snd_max)) {

		if (ti_len == 0) {

			if (SEQ_GT(ti_ack, snd_una) &&
				SEQ_LEQ(ti_ack, snd_max)) {
				acked = ti_ack - snd_una;
			
				DCC_LOG(LOG_INFO, "header prediction, ACK ...");

				mbuf_queue_trim(&tp->snd_q, acked);
				snd_una = ti_ack;

				tp->snd_seq = snd_una;
				tp->snd_off = snd_nxt - tp->snd_seq;
				tp->snd_max = snd_max - tp->snd_seq;

				if (snd_una == snd_max) {
					tp->t_rxmt_tmr = 0;
					tp->t_rxmt_cnt = 0;
					DCC_LOG(LOG_INFO, "acked all data, rxmt tmr stopped");
				} else {
					if (tp->t_rxmt_tmr == 0) {
						DCC_LOG(LOG_INFO, 
								"not all data acked restart rxmt tmr");
						tp->t_rxmt_tmr = tcp_rxmtintvl[tp->t_rxmt_cnt / 2];
					}
				}

				thinkos_cond_broadcast(tp->t_cond);

				if (tp->snd_q.len) {
					/* schedule output */
					tcp_output_sched(tp);
				}

				return 0;
			}
		} else {
			if ((ti_ack == snd_una) && 
				ti_len <= (tcp_maxrcv - tp->rcv_q.len)) {
				int len;

				DCC_LOG1(LOG_INFO, "header prediction, data (%d)", ti_len);

				/* append data */
				len = mbuf_queue_add(&tp->rcv_q, data, ti_len);
				tp->rcv_nxt += len;
				thinkos_cond_broadcast(tp->t_cond);

				if (len != ti_len) {
					DCC_LOG1(LOG_WARNING, "<%05x> no more mbufs", (int)tp);
					tp->t_flags |= TF_ACKNOW;
					/* schedule output */
					tcp_output_sched(tp);
				} else {
					tp->t_flags |= TF_DELACK;
				}

				return 0;
			 }
		}
	}

#endif /* TCP_ENABLE_HEADER_PREDICTION */

	/* Slow path input processing
	   Ref.: TCP/IP Illustrated Volume 2, pg. 941  */

	/* TODO: Drop TCP, IP headers and TCP options. 
		Well, only if these structures were dynamic allocated... */
	
	if (ti_len == 0) {
		DCC_LOG(LOG_INFO, "slow path ACK");
	} else {
		DCC_LOG1(LOG_INFO, "slow path (%d)", ti_len);
	}

	/* Calculate the amount of space in receive window,
	   and then do TCP input processing.
	   Receive window is amount of space in rcv queue,
	   but not less than advertise window.
	   Ref.: TCP/IP Illustrated Volume 2, pg. 941  */
	{
		int win;
		
		/* space left in the input queue */
		win = tcp_maxrcv - tp->rcv_q.len;
		
		if (win <= 0) {
			win = 0;
			DCC_LOG(LOG_INFO, "receive buffer full!");
		}


//		rcv_wnd = MAX(win, tp->rcv_adv_wnd);
		rcv_wnd = win;

		DCC_LOG3(LOG_INFO, "adv_wnd=%d rcv_wnd=%d win=%d", 
				tp->rcv_adv_wnd, rcv_wnd, win);
	} 

	if (tp->t_state == TCPS_SYN_SENT) {
		/* response to an active open. 
		   Ref.: TCP/IP Illustrated Volume 2, pg. 947  */

		/* Common proccessing for receipt of SYN. 
		   Ref.: TCP/IP Illustrated Volume 2, pg. 950 */
		if ((tiflags & TH_RST)) {
			goto close;
		}

		if (!(tiflags & TH_SYN)) {
			DCC_LOG(LOG_WARNING, "SYN_SENT SYN ?");
			/* TODO: reset */
			goto close_and_reset;
		}

		if (!(tiflags & TH_ACK)) {
			DCC_LOG(LOG_WARNING, "SYN_SENT ACK ?");
			/* TODO: reset */
			goto close_and_reset;
		}

		if (ti_len != 0) {
			DCC_LOG(LOG_WARNING, "ti_len != 0");
			/* TODO: reset */
			goto close_and_reset;
		}

		/* update the send sequence */
		tp->snd_seq++;
		if (tp->snd_seq != ti_ack) {
			DCC_LOG3(LOG_WARNING, "<%05x> tp->snd_seq(%d) != ti_ack(%d)",
					 (int)tp, tp->snd_seq, ti_ack);
			/* TODO: reset */
			goto close_and_reset;
		}
		tp->snd_off--;
		tp->snd_max--;
//		tp->snd_off = 0;
//		tp->snd_max = 0;

		if (optlen)
			tcp_parse_options(tp, th, th->th_opt, optlen);

		/* Advance tp->ti_seq to correspond to first data byte. */
		ti_seq++;
		if (ti_len > rcv_wnd) {
			DCC_LOG3(LOG_WARNING, "<%05x> ti_len(%d) > rcv_wnd(%d)", 
				(int)tp, ti_len, rcv_wnd);
		/* TODO: if data, trim to stay within window. */
			ti_len = rcv_wnd;
		}

		/* update the sequence number */
		tp->rcv_nxt = ti_seq;

		/* update the window size */
		tp->snd_wnd = ntohs(th->th_win);

		tp->t_state = TCPS_ESTABLISHED;
		DCC_LOG1(LOG_INFO, "<%05x> [ESTABLISHED]", (int)tp);
		/* TODO: initialization of receive urgent pointer
		tcp->rcv_up = ti_seq; */
		/* XXX: */ 
		tp->t_flags |= TF_ACKNOW;
		thinkos_cond_broadcast(tp->t_cond);

		goto step6;

close_and_reset:
		tp->t_state = TCPS_CLOSED;
		pcb_move((struct pcb *)tp, &__tcp__.active, &__tcp__.closed);
		DCC_LOG1(LOG_INFO, "<%05x> [CLOSED]", (int)tp);

		/* XXX: discard the data */
		mbuf_queue_free(&tp->snd_q);
		mbuf_queue_free(&tp->rcv_q);

		/* notify the upper layer */
		thinkos_cond_broadcast(tp->t_cond);

		goto dropwithreset;	
	}

/* States other than LISTEN or SYN_SENT 
   First check timestamp, if present.
   Then check that at least some bytes of segment are within
   receive window.  If segment begins before rcv_nxt,
   drop leading data (and SYN); if nothing left, just ti_ack. */

	/* Trim Segment so Data is Within Window
	   Ref.: TCP/IP Illustrated Volume 2, pg. 954 */
	todrop = tp->rcv_nxt - ti_seq;
	if (todrop > 0) {
		if (tiflags & TH_SYN) {
			DCC_LOG(LOG_INFO, "SYN");
			tiflags &= ~TH_SYN;
			ti_seq++;
			todrop--;
		}
		if ((todrop > ti_len) || 
		   ((todrop == ti_len) && ((tiflags & TH_FIN) == 0))) {
			tiflags &= ~TH_FIN;
			tp->t_flags |= TF_ACKNOW;
			todrop = ti_len;		
		}

		DCC_LOG4(LOG_WARNING, "<%05x> drop: len=%d drop=%d rem=%d!", 
			(int)tp, ti_len, todrop, ti_len - todrop);

		/* adjust the data pointer */
		data += todrop;

		ti_seq += todrop;
		ti_len -= todrop;

		/* TODO: adjust the urgent pointer */
	} 

	/* FIXME: only reset the connection if there are no more 
		application to handle the incomming data, half-close */
	if ((tp->t_state > TCPS_FIN_WAIT_1) && (ti_len)) { 
		DCC_LOG1(LOG_INFO, "<%05x> segment received after FIN", (int)tp);
		/* TODO: stat */
		goto dropwithreset;	
	}

	/* If segment ends after window, drop trailing data
	   and (PUSH and FIN); if nothing left, just ACK.
	   Ref.: TCP/IP Illustrated Volume 2, pg. 958 */
	todrop = (ti_seq + ti_len) - (tp->rcv_nxt + rcv_wnd);

	DCC_LOG4(LOG_INFO, "ti_seq=%u ti_len=%d rcv_nxt=%u rcv_wnd=%d", 
			ti_seq,  ti_len, tp->rcv_nxt, rcv_wnd);
	/* */

	if (todrop > 0) {
//		TCP_LOG(tp, "tcp_input: trailing data drop");
		if (todrop >= ti_len) {

	   		/* 
			 * If a new connection request is received 
			 * while in TIME_WAIT, drop the old connection ...
			 * Ref.: TCP/IP Illustrated Volume 2, pg. 958 
			if ((tiflags & TH_SYN) && (tp->t_state == TCPS_TIMEWAIT) &&
			   (SEQ_GT(ti_seq, tp->rcv_nxt))) {
				__tcp__.iss += tcp_issincr;
				tcp_rst(tp);
				goto findpcb;
			} */

			if ((rcv_wnd == 0) && (ti_seq == tp->rcv_nxt)) {
				tp->t_flags |= TF_ACKNOW;
			} else
				goto dropafterack;
		}

		DCC_LOG2(LOG_WARNING, "<%05x> data drop: %d!", (int)tp, todrop);
		ti_len -= todrop;
		tiflags &= ~(TH_PSH | TH_FIN);
	}

	/* If the RST bit is set eximine the state: ...
	   Ref.: TCP/IP Illustrated Volume 2, pg. 964 */
	if ((tiflags & TH_RST)) {
		DCC_LOG1(LOG_WARNING, "<%05x> RST received", (int)tp);
		switch(tp->t_state) {
		case TCPS_SYN_RCVD:
//			tp->errno = ECONNREFUSED;
			goto close;
		case TCPS_ESTABLISHED:
		case TCPS_CLOSE_WAIT:
//			tp->errno = ECONNRESET;
close:
			/* discard the data */
			mbuf_queue_free(&tp->snd_q);
			mbuf_queue_free(&tp->rcv_q);

			tp->t_state = TCPS_CLOSED;
			pcb_move((struct pcb *)tp, &__tcp__.active, &__tcp__.closed);
			DCC_LOG1(LOG_INFO, "<%05x> [CLOSED]", (int)tp);

			/* notify the upper layer */
			thinkos_cond_broadcast(tp->t_cond);
			/* PCBs in the close state should be cleared by the application */
			goto drop;

		case TCPS_FIN_WAIT_1:
		case TCPS_FIN_WAIT_2:
		case TCPS_CLOSING:
		case TCPS_LAST_ACK:
		case TCPS_TIME_WAIT:
			/* Our side was already closed */
			tcp_pcb_free(tp);
			goto drop;
		}
	}

	/* If a SYN is in the window, then this is an 
	   error and we send an RST and drop the connection.
	   Ref.: TCP/IP Illustrated Volume 2, pg. 965 */
	if ((tiflags & TH_SYN)) {
		DCC_LOG1(LOG_WARNING, "<%05x> the SYN bit is set inside the window", 
			(int)tp);
		goto dropwithreset;
	}

	/* If the ACK bit is off we drop the segment and return. */
	if ((!(tiflags & TH_ACK))) {
		DCC_LOG1(LOG_WARNING, "<%05x> the ACK bit is off", (int)tp);
		goto drop;
	}
	
/*
 * ACK processing.
 * Ref.: TCP/IP Illustrated Volume 2, pg. 969 
 *
 */

	DCC_LOG4(LOG_INFO, "ack=%u una=%u nxt=%u max=%u", 
			 ti_ack, snd_una, snd_nxt, snd_max);

	switch(tp->t_state) {
	case TCPS_SYN_RCVD:
		if (SEQ_GT(snd_una, ti_ack) || 
			SEQ_GT(ti_ack, snd_max)) {
			DCC_LOG1(LOG_WARNING, 
					 "<%05x> ti_ack < snd_una || snd_max < ti_ack", 
					 (int)tp);
			goto dropwithreset;
		}
		tp->t_state = TCPS_ESTABLISHED;
		tp->snd_off--;
		tp->snd_max--;
		DCC_LOG1(LOG_INFO, "<%05x> SYN ackd [ESTABLISHED]", (int)tp);
		/* notify the upper layer*/
//		thinkos_cond_signal(tp->t_cond);

		/* TODO: tcp reassembly
		tcp_reass(tp); */
	case TCPS_ESTABLISHED:
	case TCPS_FIN_WAIT_1:
	case TCPS_FIN_WAIT_2:
	case TCPS_CLOSE_WAIT:
	case TCPS_CLOSING:
	case TCPS_LAST_ACK:
	case TCPS_TIME_WAIT:
		/* TODO: tcp reassembly
		   tcp_reass(tp); */
		if (SEQ_LEQ(ti_ack, snd_una)) {
			/* TODO: check for completly duplicated ACKs.
			   Ref.: TCP/IP Illustrated Volume 2, pg. 971 */
			if ((ti_len == 0) && (tiwin == tp->snd_wnd)) {
				if ((tp->t_rxmt_tmr == 0) || ti_ack != snd_una) {
//					dupacks = 0;
				} else {
					DCC_LOG2(LOG_INFO, "duplicated ACK. ti_ack=%u snd_una=%u", 
							 ti_ack, snd_una);
				}
			} else {
//				dupacks = 0;
			}
			break;
		}

		/* Check out of range ACK */
		/*  Ref.: TCP/IP Illustrated Volume 2, pg. 974 */
		if (SEQ_GT(ti_ack, snd_max)) {
			/* TODO:
			   tcpstat.tcps_rcvacktoomuch++;
			 */
			DCC_LOG3(LOG_WARNING, "(%04x) out of range ACK. "
				"th_ack=%u > snd_max=%u !", 
				(int)tp, ti_ack, snd_max);
			goto dropafterack;	
		}

		acked = ti_ack - snd_una;

		/* TODO:
		   tcpstat.tcps_rcvackpack++;
		   tcpstat.tcps_rcvackbyte += acked;		
		 */

		DCC_LOG1(LOG_INFO, "acked=%d", acked);

		/* If all outstanding data is acked, stop retransmit timer else
		   restarts it ....
		   Ref.: TCP/IP Illustrated Volume 2, pg. 976 */
		if (ti_ack == snd_max) {
			tp->t_rxmt_tmr = 0;
			tp->t_rxmt_cnt = 0;
			needoutput = 1;
			DCC_LOG(LOG_INFO, "acked all data, rxmt tmr stopped");
		} else {
			/* TODO: peristent timer */
//			if (tp->t_persist_tmr == 0) {
				DCC_LOG(LOG_INFO, "not all data acked restart rxmt tmr");
				tp->t_rxmt_tmr = tcp_rxmtintvl[tp->t_rxmt_cnt / 2];
//			}
		}

		/* TODO:
		   tcpstat.tcps_rcvackpack++;
		   tcpstat.tcps_rcvackbyte += acked;		
		 */

		/* TODO: remove acknowledged data from send buffer 
		   Ref.: TCP/IP Illustrated Volume 2, pg. 978 */
		/* FIXME: send buffer bytes count */
		if (acked > tp->snd_q.len) {
			mbuf_queue_trim(&tp->snd_q, tp->snd_q.len);
			ourfinisacked = 1;
		} else {
			/* TODO: estimate the send window */
			mbuf_queue_trim(&tp->snd_q, acked);
			ourfinisacked = 0;
		}

		/* awaken a thread waiting on the send buffer ... */
		thinkos_cond_broadcast(tp->t_cond);

		snd_una = ti_ack;

		if (SEQ_LT(snd_nxt, snd_una)) {
			snd_nxt = snd_una;
		}

		tp->snd_seq = snd_una;
		tp->snd_off = snd_nxt - tp->snd_seq;
		tp->snd_max = snd_max - tp->snd_seq;

		DCC_LOG4(LOG_INFO, "<%05x> snd_seq=%u snd_max=%u snd_q.len=%d", 
			(int)tp, tp->snd_seq, snd_max, tp->snd_q.len); 

		switch(tp->t_state) {
		case TCPS_FIN_WAIT_1:
			if (ourfinisacked) {
				/* FIXME: If we can't receive any more data..
				   Ref.: TCP/IP Illustrated Volume 2, pg. 979 */
				tp->t_conn_tmr = 4 * tcp_msl;
				tp->t_state = TCPS_FIN_WAIT_2;
				DCC_LOG1(LOG_INFO, "<%05x> [FIN_WAIT_2]", (int)tp);
			}
			break;
		case TCPS_CLOSING:
			if (ourfinisacked) {
				mbuf_queue_free(&tp->snd_q);
				mbuf_queue_free(&tp->rcv_q);
				tp->t_state = TCPS_TIME_WAIT;
				DCC_LOG1(LOG_INFO, "<%05x> [TIME_WAIT]", (int)tp);
				tp->t_rxmt_tmr = 0;
				tp->t_conn_tmr = 2 * tcp_msl;
				DCC_LOG1(LOG_INFO, "stop rxmt tmr, start 2MSL tmr: %d",
						 tp->t_conn_tmr);
			}
			break;
		case TCPS_LAST_ACK:
			if (ourfinisacked) {
				tcp_pcb_free(tp);
				goto drop;
			}
			break;

		case TCPS_TIME_WAIT:
			/* restart the finack timer 
			   Ref.: TCP/IP Illustrated Volume 2, pg. 981 */
			tp->t_conn_tmr = 2 * tcp_msl;
			goto dropafterack;
		}
		break;
	}

	DCC_LOG4(LOG_INFO, "<%05x> recvd=%d acked=%d rcv_q.len=%d", (int)tp, 
		ti_len, acked, tp->rcv_q.len);
step6:
	/* Update window information 
	   Ref.: TCP/IP Illustrated Volume 2, pg. 982 */
	DCC_LOG(LOG_MSG, "setp6");
	
//	if ((tiflags & TH_ACK) && (tiwin > tp->snd_wnd)) {
	if ((tiflags & TH_ACK) && (tiwin != tp->snd_wnd)) {
		/* Keep track of pure window updates */
		/* TODO: TCP Statistics */
		/* TODO: Update window information */
		DCC_LOG1(LOG_INFO, "window update, win=%d", tiwin);
		tp->snd_wnd = tiwin;
		needoutput = 1;
	}

	/* TODO: Urgent mode processing */
	/* Process the segment text, 
	   merging it into the TCP sequencing queue,
dodata:
	   ...
	   Ref.: TCP/IP Illustrated Volume 2, pg. 988 */
	if ((ti_len || (tiflags & TH_FIN)) && 
		TCPS_HAVERCVDFIN(tp->t_state) == 0) {

		if ((ti_seq == tp->rcv_nxt) && (tp->t_state == TCPS_ESTABLISHED)) {

			/* append data */
			int n;

			tp->t_flags |= TF_DELACK;

			n = mbuf_queue_add(&tp->rcv_q, data, ti_len);
			if (n != ti_len) {
				DCC_LOG2(LOG_WARNING, "no more mbufs, %d != %d", n, ti_len);
			}
			ti_len = n;

			tp->rcv_nxt += ti_len;
			/* TODO: statistics */

			tiflags &= TH_FIN;

//			if (tp->rcv_q.len == ti_len) {
//				DCC_LOG3(LOG_INFO, "<%05x> rcvd %d, signaling %d ...", 
//					(int)tp, ti_len, tp->t_cond);
			/* 
			 * notify the upper layer of the data arrival...
			 */
			thinkos_cond_signal(tp->t_cond);
//			} else {
//				DCC_LOG2(LOG_INFO, "<%05x> rcvd %d", (int)tp, ti_len);
//			}

		} else {
			/* TODO: half-close */
			/* TODO: reassembly */
//			m = mlink_free(m);
			if (tp->t_state == TCPS_ESTABLISHED) {
//				DCC_LOG(LOG_WARNING, "out of order, drop!");
				DCC_LOG(LOG_WARNING, "out of order, drop");
				TCP_PROTO_STAT_ADD(rx_drop, 1);
			}
			tp->t_flags |= TF_ACKNOW;
		}
	} else {
		DCC_LOG(LOG_INFO, "!!!!!!!!!");
		tiflags &= ~TH_FIN;
	}

	/* FIN Processing */
	if (tiflags & TH_FIN) {
		if (TCPS_HAVERCVDFIN(tp->t_state) == 0) {
			tp->t_flags |= TF_ACKNOW;
			tp->rcv_nxt++;
		}
		switch(tp->t_state) {
		case TCPS_SYN_RCVD:
		case TCPS_ESTABLISHED:
			tp->t_state = TCPS_CLOSE_WAIT;
			DCC_LOG1(LOG_INFO, "<%05x> [CLOSE_WAIT]", (int)tp);
			/* notify the application that our peer 
			   has closed its side. Sockets: marks 
			   the socket as write-only */
			if (tp->rcv_q.len == 0) {
				thinkos_cond_broadcast(tp->t_cond);
			}
			break;
		case TCPS_FIN_WAIT_1:
			tp->t_state = TCPS_CLOSING;
			DCC_LOG1(LOG_INFO, "<%05x> [CLOSING]", (int)tp);
			break;
		case TCPS_FIN_WAIT_2:
			mbuf_queue_free(&tp->rcv_q);
			mbuf_queue_free(&tp->snd_q);
			tp->t_state = TCPS_TIME_WAIT;
			DCC_LOG1(LOG_INFO, "<%05x> [TIME_WAIT]", (int)tp);
			tp->t_rxmt_tmr = 0;
			tp->t_conn_tmr = 2 * tcp_msl;
			DCC_LOG1(LOG_INFO, "stop rxmt tmr, start 2MSL tmr: %d",
					 tp->t_conn_tmr);
			break;
		case TCPS_TIME_WAIT:
			/* restart the counter */
			tp->t_conn_tmr = 2 * tcp_msl;
			break;
		}
	}

	/* Final Processing */
	if (needoutput || (tp->t_flags & TF_ACKNOW)) {
		if (needoutput) {
			DCC_LOG(LOG_INFO, "needoutput, call tcp_out.");
		}
		if (tp->t_flags & TF_ACKNOW) {
			DCC_LOG(LOG_INFO, "ACKNOW set, call tcp_out.");
		}
		/* schedule output */
		tcp_output_sched(tp);
	}
	return 0;

dropafterack:
	DCC_LOG1(LOG_INFO, "<%05x> drop and ACK", (int)tp);

	if (tiflags & TH_RST)
		goto drop;

	tp->t_flags |= TF_ACKNOW;
	/* schedule output */
	tcp_output_sched(tp);
	return 0;

dropwithreset:
	DCC_LOG1(LOG_TRACE, "<%05x> drop and RST", (int)tp);

	ret = 0;
	/* TODO: check for a broadcast/multicast */
	if (!(tiflags & TH_RST)) {
		if (tiflags & TH_ACK) {
			ret = tcp_respond(iph, th, 0, ti_ack, TH_RST);
		} else if (tiflags & TH_SYN) {
				ti_len++;
			ret = tcp_respond(iph, th, ti_seq + ti_len, 0, TH_ACK | TH_RST);
		}
	}
	TCP_PROTO_STAT_ADD(rx_drop, 1);
	return ret;

drop:
	DCC_LOG(LOG_TRACE, "drop");
	TCP_PROTO_STAT_ADD(rx_drop, 1);

	return 0;
}

