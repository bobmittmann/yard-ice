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
 * @file udp_recv.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <string.h>
#include <sys/param.h>
#include <errno.h>
#include <stdlib.h>

int udp_recv(struct udp_pcb * __up, void * __buf, int __len, 
			 struct sockaddr_in * __addr)
{
	struct udp_dgram * dgram;
	int head;
	int tail;
	int n;

	DCC_LOG2(LOG_INFO, "pb=%p len=%d", __up, __len);

	if (__up == NULL) {
		DCC_LOG1(LOG_WARNING, "<%05x> invalid pcb", (int)__up);
		return -EFAULT;
	}

	if (__buf == NULL) {
		DCC_LOG1(LOG_WARNING, "<%05x> invalid buffer", (int)__up);
		return -EFAULT;
	}

	tcpip_net_lock();

#if (ENABLE_NET_SANITY_CHECK)
	if (pcb_find((struct pcb *)__up, &__udp__.active) < 0) {
		DCC_LOG1(LOG_ERROR, "<%05x> pcb_find()", (int)__up);
		tcpip_net_unlock();
		/* TODO: errno */
		return -1;
	}
#endif

	for (;;) {
		/*  check for binding */
		if (__up->u_lport == 0) {
			DCC_LOG(LOG_WARNING, "not bound (closed)!");
			tcpip_net_unlock();
			/* TODO: correct errno ?? */
			return -ENOTCONN;
		}

		if (__up->u_icmp_err) {
			/* there was an ICMP error, clear the flag and exit  */
			__up->u_icmp_err = 0;
			DCC_LOG(LOG_WARNING, "ICMP error!");
			tcpip_net_unlock();
			return -ECONNREFUSED;
		}

		head = __up->u_rcv_head;
		tail = __up->u_rcv_tail;
		if (head != tail)
			break;

		DCC_LOG2(LOG_INFO, "<%05x> wait [%d]", (int)__up, __up->u_rcv_cond);
		thinkos_cond_wait(__up->u_rcv_cond, net_mutex);
	}

	dgram = &__up->u_rcv_buf[head % NET_UDP_RECV_QUEUE_LEN];
	__up->u_rcv_head = head + 1;

	n = MIN(dgram->len, __len);

	n = mbuf_list_read(dgram->q, __buf, n);

	mbuf_list_free(dgram->q);

	if (__addr != NULL) {
		__addr->sin_family = AF_INET;
		__addr->sin_port = dgram->port;
		__addr->sin_addr.s_addr = dgram->addr;
	}

	DCC_LOG5(LOG_TRACE, "%I:%d > %I:%d (%d)", 
		__up->u_laddr, ntohs(__up->u_lport), dgram->addr, 
		ntohs(dgram->port), dgram->len); 

	tcpip_net_unlock();
	
#if (LOG_LEVEL < LOG_INFO)
	DCC_LOG(LOG_TRACE, "received.");
#endif

	return n;
}

