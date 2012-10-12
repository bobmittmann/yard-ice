/* 
 * File:	tcp_accept_tmo.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifdef TCP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>

#include <sys/mbuf.h>
#include <sys/in.h>
#include <sys/net.h>
#include <tcpip/ip.h>
#include <tcpip/tcp.h>
#include <tcpip/arp.h>

#include <sys/dcclog.h>

struct tcp_pcb * tcp_accept_tmo(const struct tcp_pcb * __mux, int __tmo)
{
	struct tcp_listen_pcb * mux = (struct tcp_listen_pcb *)__mux;
	struct tcp_pcb * tp;
	int ret;

	tcpip_net_lock();
	
	while (mux->t_head == mux->t_tail) {

		DCC_LOG1(LOG_TRACE, "<%04x> waiting...", (int)mux);

		if ((ret = uthread_cond_timedwait(__mux->t_cond, net_mutex, __tmo)) < 0) {
			if (ret == -ETIMEDOUT)
				return NULL;
		}
	}

	tp = (struct tcp_pcb *)mux->t_backlog[mux->t_head++];

	/* wrap */
	if (mux->t_head == mux->t_max)
		mux->t_head = 0;

	if (tp == NULL) {
		DCC_LOG(LOG_PANIC, "NULL pointer");
	} else {
		DCC_LOG3(LOG_TRACE, "<%04x> %I:%d", (int)tp, 
				 tp->t_faddr, ntohs(tp->t_fport));
	}

	tcpip_net_unlock();

	return tp;
}

