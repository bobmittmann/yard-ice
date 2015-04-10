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
 * @file udp_connect.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <stdlib.h>
#include <errno.h>

int udp_connect(struct udp_pcb * __up, in_addr_t __addr, uint16_t __port) 
{
	int ret = 0;

	if (__up == NULL) {
		DCC_LOG1(LOG_ERROR, "<%05x> NULL", (int)__up);
		/* FIXME: not a socket? The semantic here is not exactly the same
		   as the sockets API. */
		return -ENOTSOCK;
	}

	DCC_LOG3(LOG_TRACE, "<%05x> %I:%d", (int)__up, __addr, ntohs(__port));

	if ((__port == 0) && (__addr != INADDR_ANY)) {
		DCC_LOG1(LOG_WARNING, "<%05x> invalid port.", (int)__up);
		return -EADDRNOTAVAIL;
	}

	tcpip_net_lock();

	if ((__addr == INADDR_ANY) && (__port == 0)) {
		__up->u_faddr = __addr;
		__up->u_fport = __port;
		thinkos_cond_signal(__up->u_rcv_cond);
	} else {
		if (pcb_lookup(__addr, __port, __up->u_laddr, 
					   __up->u_lport, &__udp__.active)) {
			DCC_LOG3(LOG_WARNING, "<%05x> %I:%d in use", (int)__up,
					 __addr, ntohs(__port));
			ret = -EADDRINUSE;
		} else {
			__up->u_faddr = __addr;
			__up->u_fport = __port;
			thinkos_cond_signal(__up->u_rcv_cond);
		}
	}

	tcpip_net_unlock();

	return ret;
}

