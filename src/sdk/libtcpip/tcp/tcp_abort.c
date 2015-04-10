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
 * @file tcp_abort.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

/* 
 * Sends a RST to the peer and close the connection
 * 
 * This function shuld be called after the upper layer released
 * its resources.
 *
 */
void tcp_abort(struct tcp_pcb * tp)
{
	int state;

	state = tp->t_state;

	if (state != TCPS_CLOSED) {

		tp->t_state = TCPS_CLOSED;

		DCC_LOG1(LOG_TRACE, "<%04x> [CLOSED]", (int)tp);

		/* discard the buffers */
		mbuf_queue_free(&tp->rcv_q);
		mbuf_queue_free(&tp->snd_q);
		thinkos_cond_free(tp->t_cond);

		/* move from the active pcb list to the closed list */
		if (pcb_move((struct pcb *)tp, &__tcp__.active, 
			&__tcp__.closed) < 0) {
			DCC_LOG1(LOG_ERROR, "<%04x> pcb_move()", (int)tp);
		}

	}

	if (TCPS_HAVERCVDSYN(state)) {
		/* Calling the tcp_output in the close state, cause
		   the TCP send a RST to the peer. */
		tcp_output(tp);
	}

	tcp_pcb_free(tp);
}

