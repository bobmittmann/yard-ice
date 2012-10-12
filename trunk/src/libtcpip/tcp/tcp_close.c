/* $Id: tcp_close.c,v 2.11 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcp_close.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
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

#include <sys/mbuf.h>
#include <sys/in.h>
#include <sys/net.h>

#include <tcpip/ip.h>
#include <tcpip/tcp.h>
#include <tcpip/arp.h>
#include <errno.h>

#include <sys/dcclog.h>
#include <debug.h>

#if defined(DEBUG) || defined(ENABLE_LOG)
extern const char * const __tcp_state[];
#endif

int tcp_close(struct tcp_pcb * __tp)
{
	int ret;

	tcpip_net_lock();

#ifdef ENABLE_SANITY
	if ((pcb_find((struct pcb *)__tp, &__tcp__.active) < 0) && 
		(pcb_find((struct pcb *)__tp, &__tcp__.listen) < 0) &&
		pcb_find((struct pcb *)__tp, &__tcp__.closed) < 0) {
		DBG(DBG_ERROR, "<%05x> pcb_find()", (int)__tp);
		tcpip_net_unlock();
		return -1;
	}
#endif

	switch(__tp->t_state) {
		case TCPS_LISTEN:
			DCC_LOG1(LOG_TRACE, "<%05x> [LISTEN]", (int)__tp);
		case TCPS_TIME_WAIT:
		case TCPS_CLOSED:  
		case TCPS_SYN_SENT:
			ret = tcp_pcb_free(__tp);
			tcpip_net_unlock();
			return ret;

		/* active close */
		case TCPS_SYN_RCVD:
		case TCPS_ESTABLISHED:
			/* Close the receive window */
			/*
			 * XXX: if we close the receive window we may stuck at
			 * FIN_WAIT_2 state...
			 */
//			__tp->rcv_wnd = 0;
			__tp->t_state = TCPS_FIN_WAIT_1;
			DCC_LOG1(LOG_TRACE, "<%05x> [FIN_WAIT_1]", (int)__tp);
			break;

		/* passive close */
		case TCPS_CLOSE_WAIT:
			__tp->t_state = TCPS_LAST_ACK;
			DCC_LOG1(LOG_TRACE, "<%05x> [LAST_ACK]", (int)__tp);
			/* discard the data 
			 *  TODO: check whether both buffers must be 
			 * released or not. Probably they where released already.
			 */
			/* discards unsent data */
			__tp->snd_off -= __tp->snd_q.len;
			__tp->snd_max -= __tp->snd_q.len;
			mbuf_queue_free(&__tp->snd_q);
			mbuf_queue_free(&__tp->rcv_q);
			/*  notify the upper layer that we are closed */
			break;

		default: {
			DCC_LOG2(LOG_ERROR, "<%05x> state=[%s]", (int)__tp, 
				__tcp_state[__tp->t_state]);
			tcpip_net_unlock();
			return -1;
		}
	}

	/* ACK now */
	__tp->t_flags |= TF_ACKNOW;
	/* schedule output */
	__tcp__.need_output = 1;
	uthread_cond_signal(__tcp__.output_cond);

	tcpip_net_unlock();
	return 0;
}

