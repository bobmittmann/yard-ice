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
 * @file tcp_accept.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

struct tcp_pcb * tcp_accept(const struct tcp_pcb * __mux)
{
	struct tcp_listen_pcb * mux = (struct tcp_listen_pcb *)__mux;
	struct tcp_pcb * tp;
	unsigned int tail;

	if (__mux == NULL)
		DCC_LOG(LOG_WARNING, "NULL pointer");

	/* FIXME: this call is not reentrant for the same tcp_pcb.
	   Only one accept point should exist for each listening PCB. */
		DCC_LOG1(LOG_TRACE, "<%04x> waiting...", (int)mux);

	if (__os_sem_wait(mux->t_sem) < 0) {
		DCC_LOG2(LOG_ERROR, "<%04x> __os_sem_wait(%d) failed!", 
				 (int)mux, mux->t_sem);
		return NULL;
	}

	tail = mux->t_tail;

	tp = (struct tcp_pcb *)mux->t_backlog[tail++];

	/* wrap */
	if (tail == mux->t_max)
		tail = 0;

	mux->t_tail = tail;

#ifdef ENABLE_SANITY
	if (tp == NULL)
		DCC_LOG(LOG_PANIC, "NULL pointer");
#endif

	DCC_LOG4(LOG_TRACE, "<%04x> --> <%04x> %I:%d", (int)mux, 
			 (int)tp, tp->t_faddr, ntohs(tp->t_fport));


	return tp;
}

