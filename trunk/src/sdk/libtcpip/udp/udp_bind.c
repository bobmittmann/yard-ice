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
 * @file udp_bind.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <errno.h>
#include <stdlib.h>

int udp_bind(struct udp_pcb * __up, in_addr_t __addr, uint16_t __port) 
{
	if (__up == NULL) {
		DCC_LOG1(LOG_ERROR, "<%06x> NULL", (int)__up);
		/* FIXME: not a socket? The semantic here is not exactly the same
		   as the sockets API. */
		return -ENOTSOCK;
	}

	DCC_LOG3(LOG_TRACE, "<%05x> %I:%d", (int)__up, __addr, ntohs(__port));

	tcpip_net_lock();

	__up->u_lport = __port;
	__up->u_laddr = __addr;

	DCC_LOG2(LOG_TRACE, "<%05x> cond=%d", (int)__up, __up->u_rcv_cond);

	tcpip_net_unlock();

	return 0;
}

