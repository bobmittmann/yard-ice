/* $Id: tcp_abort.c,v 2.6 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcp_abort.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment: API function.
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdint.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>

#include <sys/mbuf.h>
#include <sys/in.h>
#include <tcpip/ip.h>
#include <tcpip/tcp.h>

#ifdef TCP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

/* 
 * Sends a RST to the PEER and call the close
 * 
 * This function shuld be called after the upper layer be released
 * its resources.
 *
 *
 */
void tcp_abort(struct tcp_pcb * tp)
{
	int state;

	state = tp->t_state;

	if (state != TCPS_CLOSED) {

		tp->t_state = TCPS_CLOSED;

		DBG(DBG_TRACE, "<%04x> [CLOSED]", (int)tp);

		/* discard the buffers */
		mbuf_queue_free(&tp->rcv_q);
		mbuf_queue_free(&tp->snd_q);

		/* move from the active pcb list to the closed list */
		if (pcb_move((struct pcb *)tp, &__tcp__.active, 
			&__tcp__.closed) < 0) {
			DBG(DBG_ERROR, "<%04x> pcb_move()", (int)tp);
		}

	}

	if (TCPS_HAVERCVDSYN(state)) {
		/* Calling the tcp_output in the close state, cause
		   the TCP send a RST to the peer. */
		tcp_output(tp);
	}

	tcp_pcb_free(tp);
}

