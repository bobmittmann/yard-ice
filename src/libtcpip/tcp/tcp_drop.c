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
 * @file tcp_drop.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

/* Abort an active connection or refuse an incomming one; */
int tcp_drop(struct tcp_pcb * __tp)
{
	if (pcb_find((struct pcb *)__tp, &__tcp__.active) < 0) {
		DCC_LOG1(LOG_ERROR, "<%04x> pcb_find()", (int)__tp);
		return -1;
	}

	if (TCPS_HAVERCVDSYN(__tp->t_state)) {
		tcp_abort(__tp);
		/* TODO: tcpstat.tcps_conndrops++; */
	} else {
		/* TODO: tcpstat.tcps_drops++; */
		tcp_pcb_free(__tp);
	}

	return 0;
}

