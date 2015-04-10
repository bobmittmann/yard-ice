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
 * @file tcp_bind.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>
#include <errno.h>

static int can_bind(addr, port)
{
	/* Check to see if this address is not in use already. */
	if (pcb_lookup(INADDR_ANY, 0, addr, port, &__tcp__.listen) != NULL) {
		return 0;
	}

	if (pcb_lookup(INADDR_ANY, 0, addr, port, &__tcp__.closed) != NULL) {
		return 0;
	}

	return 1;
}

int tcp_bind(struct tcp_pcb * __tp, in_addr_t __addr, uint16_t __port) 
{
	if (__tp == NULL) {
		DCC_LOG(LOG_WARNING, "NULL pointer");
		/* FIXME: not a socket? The semantic here is not exactly the same
		   as the sockets API. */
		return -ENOTSOCK;
	}

	if (pcb_find((struct pcb *)__tp, &__tcp__.closed) < 0) {
		DCC_LOG1(LOG_ERROR, "<%04x> pcb_find()", (int)__tp);
		return -ENOTSOCK;
	}

	tcpip_net_lock();

	if (__port == 0) {
		do {
			/* generate an ephemeral port number from 1024 to 33791 */
			__port = ntohs(((__tcp__.port_seq++) & 0x7fff) + 1024);
		} while (!can_bind(__addr, __port));
	} else {
		if (!can_bind(__addr, __port)) {
		DCC_LOG3(LOG_WARNING, "<%04x> %I:%d in use", 
				 (int)__tp, __addr, ntohs(__port));
			return -EADDRINUSE;
		}
	}

	DCC_LOG3(LOG_TRACE, "<%05x> %I:%d", (int)__tp, __addr, ntohs(__port));

	__tp->t_lport = __port;
	__tp->t_laddr = __addr;

	tcpip_net_unlock();

	return 0;
}

