/* $Id: tcp_listen.c,v 2.8 2008/05/28 22:32:21 bob Exp $ 
 *
 * File:	tcp_listen.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE(www.boreste.com). All Rights Reserved.
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
#include <errno.h>
#include <netinet/tcp.h>

#include <sys/net.h>
#include <tcpip/tcp.h>
#include <tcpip/ip.h>

#include <sys/dcclog.h>

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
		errno = EADDRINUSE;
		tcpip_net_unlock();
		return -1;
	}

	if (tp->t_state == TCPS_CLOSED) {
		/* Move from the closed pcb list to
		the listen list */
		pcb_move((struct pcb *)tp, &__tcp__.closed, &__tcp__.listen);
	} else {
		/* XXX: check this condition */
		DCC_LOG1(LOG_ERROR, "<%04x> state != TCPS_CLOSED", (int)tp);
		/* insert into the listen list */
		pcb_insert((struct pcb *)tp, &__tcp__.listen);
	}

	/* For preinitialized PCB's we don't write into the PCB structure
	 because it may not be stored in RAM. We expect that this PCB
	 have its state already set to LISTEN, otherwise we change the state. */
	if (tp->t_state != TCPS_LISTEN) {
		tp->t_state = TCPS_LISTEN;
		/* backlog */
	 	tp->t_max = __backlog;
		tp->t_tail = 0;
		tp->t_head = 0;
		/* uses a global conditional variable for all listening sockets */
		tp->t_cond = __tcp__.accept_cond;
	}

	DCC_LOG2(LOG_TRACE, "<%04x> port:%d [LISTEN]", (int)tp, 
		ntohs(tp->t_lport));

	DCC_LOG2(LOG_TRACE, "<%04x> port:%d [LISTEN]", (int)tp, 
		ntohs(tp->t_lport));

	tcpip_net_unlock();

	return 0;
}

