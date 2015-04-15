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
 * @file tcp_send.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>
#include <tcpip/tcp.h>

int tcp_send(struct tcp_pcb * __tp, const void * __buf, 
	int __len, int __flags)
{
	uint8_t * src;
	int rem;
	int n;
	int m;

	if (__tp == NULL) {
		DCC_LOG(LOG_WARNING, "NULL pointer");
		return -1;
	}

#ifdef ENABLE_SANITY
	if (__buf == NULL) {
		DCC_LOG1(LOG_WARNING, "<%04x> NULL pointer:", (int)__tp);
		return -1;
	}

	if (__len < 0) {
		DCC_LOG2(LOG_WARNING, "<%04x> invalid length: %d", (int)__tp, __len);
		return -1;
	}
#endif

	tcpip_net_lock();

#ifdef ENABLE_SANITY
	if (pcb_find((struct pcb *)__tp, &__tcp__.active) < 0) {
		DCC_LOG(LOG_ERROR, "<%04x> pcb_find()", (int)__tp);
		tcpip_net_unlock();
		return -1;
	}
#endif

	DCC_LOG3(LOG_INFO, "<%05x> buf=%05x len=%d", (int)__tp, (int)__buf, __len);

	src = (uint8_t *)__buf;
	rem = __len;

again:
	if (__tp->t_state != TCPS_ESTABLISHED)  {
/*
	if ((__tp->t_state != TCPS_ESTABLISHED) &&
		(__tp->t_state != TCPS_CLOSE_WAIT)) {
*/
		DCC_LOG2(LOG_INFO, "<%05x> [%s]", (int)__tp, 
				 __tcp_state[__tp->t_state]);

		if (__tp->t_state == TCPS_SYN_RCVD) {
			DCC_LOG1(LOG_INFO, "<%05x> wait", (int)__tp);
			thinkos_cond_wait(__tp->t_cond, net_mutex);
			DCC_LOG2(LOG_INFO, "<%05x> again [%s]",
					 (int)__tp, __tcp_state[__tp->t_state]);
			goto again;
		} 
		
		tcpip_net_unlock();
		return -1;
	}

	while (rem) {
		/* buffer limit ... */
		m = tcp_maxsnd - __tp->snd_q.len;
		if (m <= 0) {
			DCC_LOG1(LOG_INFO, "<%05x> queue limit", (int)__tp);
			__tp->t_flags |= TF_ACKNOW;
			
			DCC_LOG(LOG_INFO, "output request.");
			tcp_output_sched(__tp);
		
			DCC_LOG(LOG_INFO, "waiting for buffer space.");
			thinkos_cond_wait(__tp->t_cond, net_mutex);

			goto again;
		}

		m = MIN(m, rem);

		if ((n = mbuf_queue_add(&__tp->snd_q, src, m)) == 0) {
			DCC_LOG(LOG_TRACE, "mbuf_wait...");
			mbuf_wait(net_mutex);
			goto again;
		}
		rem -= n;
		src += n;
	}

#if 0
	/* FIXME: Set retransmit timer if not currently set,
	   and not doing an ack or a keepalive probe.
	   Initial value for retransmit is smoothed
	   round-trip time + 2 * round-trip time variance.
	   Initialize counter which is used for backoff
	   :of retransmit time. */
	if ((__tp->t_rxmt_tmr == 0) && (__tp->snd_una != 0)) {
		__tp->t_rxmt_tmr = tcp_rxmtintvl[0];
		__tp->t_rxmt_cnt = 0;
		/* tp->t_flags &= ~TF_IDLE; */
	}
#endif

	if (__len > 0) {
		/* TCP_SEND_NOWAIT flag set or one maximum segment size pending for
		   send then send now */
		if ((__flags & TCP_SEND_NOWAIT) || 
			((__tp->snd_q.len - (int)__tp->snd_q.offs) >= __tp->t_maxseg)) {
			DCC_LOG(LOG_INFO, "output request.");
			tcp_output_sched(__tp);
//			if (tcp_output(__tp) < 0) {
				/* if the reason to fail was an arp failure
				   try query an address pending for resolution ... */
//				arp_query_pending();
//			}
		} else  {
			__tp->t_flags |= TF_DELACK;
		}
	}

	DCC_LOG(LOG_INFO, "done.");

	tcpip_net_unlock();

	return __len;
}


