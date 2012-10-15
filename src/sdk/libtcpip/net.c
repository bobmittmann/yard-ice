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
 * @file net.c
 * @brief TCP/IP library initialization
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_NET__
#include <sys/net.h>

#include <sys/mbuf.h>

#include <sys/dcclog.h>

#ifndef ENABLE_NET_UDP
#define ENABLE_NET_UDP 1
#endif

#ifndef ENABLE_NET_TCP
#define ENABLE_NET_TCP 0
#endif

int8_t net_mutex;

void tcp_init(void);
void udp_init(void);

void tcpip_init(void)
{
	net_mutex = __os_mutex_alloc();

	DCC_LOG1(LOG_TRACE, "net_mutex=%d", net_mutex);

	mbuf_init();

#if (ENABLE_NET_UDP)
	udp_init();
#endif

#if (ENABLE_NET_TCP)
	tcp_init();
#endif
}

