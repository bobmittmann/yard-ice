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
 * @file tcp_enum.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

#include <tcpip/tcp.h>


static inline int __enum(int (* __callback)(struct tcp_inf *, void *), 
						 void * __arg, struct tcp_pcb * __tp) {
	struct tcp_inf inf;
	inf.faddr = __tp->t_faddr;
	inf.laddr = __tp->t_laddr;
	inf.fport = __tp->t_fport;
	inf.lport = __tp->t_lport;
	inf.state = __tp->t_state;

	return __callback(&inf, __arg);
}

int tcp_enum(int (* __callback)(struct tcp_inf *, void *), void * __arg) 
{
	struct tcp_pcb * tp = NULL;
	int n = 0;
	int ret;

	tcpip_net_lock();

	while ((tp = (struct tcp_pcb *)pcb_getnext(&__tcp__.closed, 
											  (struct pcb *)tp)) != NULL) {
		if ((ret = __enum(__callback, __arg, tp)) < 0)
			return ret;
		n++;
	}

	while ((tp = (struct tcp_pcb *)pcb_getnext(&__tcp__.listen, 
											  (struct pcb *)tp)) != NULL) {
		if ((ret = __enum(__callback, __arg, tp)) < 0)
			return ret;
		n++;
	}

	while ((tp = (struct tcp_pcb *)pcb_getnext(&__tcp__.active, 
											  (struct pcb *)tp)) != NULL) {
		if ((ret = __enum(__callback, __arg, tp)) < 0)
			return ret;
		n++;
	}
	tcpip_net_unlock();

	return n;
}


