/* $Id: tcp_send.c,v 2.13 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcp_send.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifdef TCP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>

#include <sys/mbuf.h>
#include <sys/in.h>
#include <sys/net.h>

#include <tcpip/ip.h>
#include <tcpip/tcp.h>
#include <tcpip/arp.h>

#include <sys/dcclog.h>
#include <debug.h>

extern const char * const __tcp_state[];

int tcp_send(struct tcp_pcb * __tp, const void * __buf, 
	int __len, int __flags)
{
	uint8_t * src;
	int rem;
	int n;
	int m;

	tcpip_net_lock();

#ifdef ENABLE_SANITY
	if (pcb_find((struct pcb *)__tp, &__tcp__.active) < 0) {
		DCC_LOG(LOG_ERROR, "<%04x> pcb_find()", (int)__tp);
		tcpip_net_unlock();
		return -1;
	}

	if (__len < 0) {
		DCC_LOG(LOG_WARNING, "<%04x> invalid length: %d", (int)__tp, __len);
		tcpip_net_unlock();
		return 0;
	}
#endif

	DCC_LOG3(LOG_TRACE, "<%05x> buf=%05x len=%d", (int)__tp, (int)__buf, __len);

	src = (uint8_t *)__buf;
	rem = __len;

again:
	if (__tp->t_state != TCPS_ESTABLISHED)  {
/*
	if ((__tp->t_state != TCPS_ESTABLISHED) &&
		(__tp->t_state != TCPS_CLOSE_WAIT)) {
*/
		DCC_LOG2(LOG_WARNING, "<%05x> [%s]", (int)__tp, 
				 __tcp_state[__tp->t_state]);

		if (__tp->t_state == TCPS_SYN_RCVD) {
			DCC_LOG1(LOG_TRACE, "<%05x> wait", (int)__tp);
			uthread_cond_wait(__tp->t_cond, net_mutex);
			DCC_LOG2(LOG_TRACE, "<%05x> again [%s]",
					 (int)__tp, __tcp_state[__tp->t_state]);
			goto again;
		} 
		

		DCC_LOG(LOG_TRACE, "done.");

		tcpip_net_unlock();
		return -1;
	}

	while (rem) {
		/* buffer limit ... */
		m = tcp_maxrcv - __tp->snd_q.len;
		if (m <= 0) {
			DCC_LOG1(LOG_WARNING, "<%05x> queue limit", (int)__tp);
			__tp->t_flags |= TF_ACKNOW;

			
			DCC_LOG(LOG_INFO, "output request.");
			__tcp__.need_output = 1;
			uthread_cond_signal(__tcp__.output_cond);
		
			DCC_LOG(LOG_INFO, "waiting for buffer space.");
			uthread_cond_wait(__tp->t_cond, net_mutex);

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
			__tcp__.need_output = 1;
			uthread_cond_signal(__tcp__.output_cond);
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


