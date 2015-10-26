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
 * @file udp_enum.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <tcpip/udp.h>

int udp_enum(int (* __callback)(struct udp_inf *, void *), void * __arg) 
{
	struct udp_pcb * lst[NET_TCP_PCB_ACTIVE_MAX];
	struct udp_pcb * up = NULL;
	int cnt = 0;
	int ret;
	int i;

	tcpip_net_lock();

	while ((up = (struct udp_pcb *)pcb_getnext(&__udp__.active, 
											   (struct pcb *)up)) != NULL) {
		lst[cnt++] = up;
	}

	tcpip_net_unlock();

	for (i = 0; i < cnt; ++i) {
		struct udp_inf inf;

		up = lst[i];
		inf.faddr = up->u_faddr;
		inf.laddr = up->u_laddr;
		inf.fport = up->u_fport;
		inf.lport = up->u_lport;

		if ((ret = __callback(&inf, __arg)) < 0) {
			DCC_LOG1(LOG_WARNING, "callback ret=%d!", ret);
			return ret;
		}
	}

	return cnt;
}

