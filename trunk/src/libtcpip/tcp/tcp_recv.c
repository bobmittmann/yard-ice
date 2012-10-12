/* $Id: tcp_recv.c,v 2.14 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcp_recv.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2009 BORESTE (www.boreste.com). All Rights Reserved.
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
#include <tcpip/tcp.h>
#include <tcpip/arp.h>

#include <debug.h>
#include <sys/dcclog.h>

#if (defined(DEBUG) || defined(ENABLE_LOG))
extern const char * const __tcp_state[];
#endif

int tcp_recv(struct tcp_pcb * __tp, void * __buf, int __len)
{
	int n;

	if (__len == 0) {
		/* invalid argument */
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

		DCC_LOG2(LOG_INFO, "<%05x> wait [%d]", (int)__tp, __tp->t_cond);

		if (__tp->rcv_q.len)
			break;

		if (__tp->t_state == TCPS_CLOSE_WAIT) {
			tcpip_net_unlock();
			return 0;
		}

		uthread_cond_wait(__tp->t_cond, net_mutex); 
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

//	if ((__tp->rcv_q.len == 0) || (__tp->t_flags & TF_DELACK)) {
	if ((__tp->rcv_q.len == 0)) {
		if (__tp->t_flags & TF_DELACK) {
			__tp->t_flags |= TF_ACKNOW;
		}

		DCC_LOG(LOG_INFO, "empty queue, call tcp_out.");

		__tcp__.need_output = 1;
		uthread_cond_signal(__tcp__.output_cond);
	}

	tcpip_net_unlock();

	return n;
}

