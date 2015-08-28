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
 * @file tcp_listen.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>
#include <errno.h>

int tcp_listen(struct tcp_pcb * __mux, int __backlog)
{
	struct tcp_listen_pcb * tp = (struct tcp_listen_pcb *)__mux;

	if (!tp) {
		DCC_LOG1(LOG_ERROR, "<%04x> NULL", (int)tp);
		/* FIXME: not a socket? The semantic here is not exactly the same
		   as the sockets API. */
		return -ENOTSOCK;
	} 

	if (__backlog == 0) {
		DCC_LOG1(LOG_WARNING, "<%04x> backlog=0", (int)tp);
	}
		
	if (__backlog > (TCP_BACKLOG_MAX - 1)) {
		DCC_LOG2(LOG_WARNING, "<%04x> backlog=%d", (int)tp, __backlog);
		__backlog = TCP_BACKLOG_MAX;
	} else {
		__backlog += 1;
	}

	DCC_LOG3(LOG_TRACE, "<%04x> %I:%d", (int)tp, tp->t_laddr, 
			 ntohs(tp->t_lport));

	tcpip_net_lock();

	/* Check to see if this address is not in use already. */
	if (pcb_wildlookup(INADDR_ANY, 0, tp->t_laddr, 
	    tp->t_lport, &__tcp__.listen) != NULL) {

		DCC_LOG3(LOG_WARNING, "<%04x> %I:%d in use", (int)tp, tp->t_laddr, 
				 ntohs(tp->t_lport));
		tcpip_net_unlock();
		return -EADDRINUSE;
	}

	/* For preinitialized PCB's we don't write into the PCB structure
	 because it may not be stored in RAM. We expect that this PCB
	 have its state already set to LISTEN, otherwise we change the state. */
	if (tp->t_state == TCPS_CLOSED) {
		int sem;

		if ((sem = thinkos_sem_alloc(0)) < 0) {
			DCC_LOG1(LOG_WARNING, 
					 "<%04x> thinkos_sem_alloc() failed!", (int)tp);
			tcpip_net_unlock();
			return -ENOMEM;
		}

		tp->t_state = TCPS_LISTEN;
		/* backlog */
	 	tp->t_max = __backlog;
		tp->t_tail = 0;
		tp->t_head = 0;
		tp->t_sem = sem;
		/* Move from the closed pcb list to
		the listen list */
		pcb_move((struct pcb *)tp, &__tcp__.closed, &__tcp__.listen);
	} else if (tp->t_state == TCPS_LISTEN) {
		/* XXX: check this condition */
		DCC_LOG1(LOG_WARNING, "<%04x> state == TCPS_LISTEN", (int)tp);
		/* insert into the listen list */
		pcb_insert((struct pcb *)tp, &__tcp__.listen);
	} else {
		tcpip_net_unlock();
	}

	DCC_LOG3(LOG_TRACE, "<%04x> semaphore=%d port=%d [LISTEN]", (int)tp, 
		tp->t_sem, ntohs(tp->t_lport));

	tcpip_net_unlock();

	return 0;
}

