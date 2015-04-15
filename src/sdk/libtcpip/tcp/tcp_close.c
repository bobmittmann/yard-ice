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
 * @file tcp_close.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

int tcp_close(struct tcp_pcb * __tp)
{
	int ret;

	if (__tp == NULL) {
		DCC_LOG(LOG_WARNING, "NULL pointer");
		return -1;
	}

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
		case TCPS_LISTEN: {
			ret = tcp_pcb_free(__tp);
			tcpip_net_unlock();
			return ret;
		}

		case TCPS_TIME_WAIT:
		case TCPS_CLOSED:  
		case TCPS_SYN_SENT:
			DCC_LOG2(LOG_INFO, "<%05x> [%s]", (int)__tp, 
					 __tcp_state[__tp->t_state]);
			if (__tp->t_cond >= 0) {
				thinkos_cond_free(__tp->t_cond);
				__tp->t_cond = -1;
			}
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
			DCC_LOG1(LOG_INFO, "<%05x> [FIN_WAIT_1]", (int)__tp);
			break;

		/* passive close */
		case TCPS_CLOSE_WAIT:
			__tp->t_state = TCPS_LAST_ACK;
			DCC_LOG1(LOG_INFO, "<%05x> [LAST_ACK]", (int)__tp);
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
	tcp_output_sched(__tp);

	tcpip_net_unlock();
	return 0;
}

