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
 * @file tcp_recv.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

int tcp_recv(struct tcp_pcb * __tp, void * __buf, int __len)
{
	int n;

	if (__tp == NULL) {
		DCC_LOG(LOG_WARNING, "NULL pointer");
		return -1;
	}

	if (__len == 0) {
		/* invalid argument */
		DCC_LOG(LOG_WARNING, "invalid argument");
		return -1;
	}

	tcpip_net_lock();

#ifdef ENABLE_SANITY_CHECK
	if (pcb_find((struct pcb *)__tp, &__tcp__.active) < 0) {
		DCC_LOG1(LOG_ERROR, "<%05x> pcb_find()", (int)__tp);
		tcpip_net_unlock();
		return -1;
	}
#endif

	for (;;) {
		if ((__tp->t_state == TCPS_CLOSED)) {
			DCC_LOG(LOG_WARNING, "closed!");
			tcpip_net_unlock();
			return -1;
		}

		if ((__tp->t_state == TCPS_TIME_WAIT) ||
			(__tp->t_state == TCPS_CLOSING) || 
			(__tp->t_state == TCPS_LAST_ACK)) {
			tcpip_net_unlock();
			return 0;
		}

		if (__tp->rcv_q.len)
			break;

		if (__tp->t_state == TCPS_CLOSE_WAIT) {
			tcpip_net_unlock();
			return 0;
		}

		DCC_LOG2(LOG_MSG, "<%05x> wait [%d]", (int)__tp, __tp->t_cond);

		thinkos_cond_wait(__tp->t_cond, net_mutex); 
	}
			 
	n = mbuf_queue_remove(&__tp->rcv_q, __buf, __len);

	DCC_LOG1(LOG_INFO, "len=%d", n);

	/* Half close: don't reopen the receiver window, i'm not sure 
	   whether it is a rule break or not, but it may prevent 
	   resources been consumed by an about to die connection! */
	if ((__tp->t_state == TCPS_FIN_WAIT_1) ||
	    (__tp->t_state == TCPS_FIN_WAIT_2)) {
		DCC_LOG1(LOG_TRACE, "<%05x> FIN_WAIT", (int)__tp);
		tcpip_net_unlock();
		return n;
	}

	/* XXX: revisit this ... */
//	if ((__tp->rcv_q.len == 0) || (__tp->t_flags & TF_DELACK)) {
	if ((__tp->rcv_q.len == 0)) {
		if (__tp->t_flags & TF_DELACK) {
			__tp->t_flags |= TF_ACKNOW;
		}

		DCC_LOG(LOG_INFO, "empty queue, call tcp_out.");

		tcp_output_sched(__tp);
	}

	tcpip_net_unlock();

	return n;
}

