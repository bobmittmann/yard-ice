/* 
 * File:	tcp_bind.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
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

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>

#include <sys/mbuf.h>
#include <sys/in.h>
#include <tcpip/ip.h>

#include <sys/net.h>
#include <uthreads.h>

#define __LIB_TCPIP__
#include <tcpip/tcp.h>

#include <sys/dcclog.h>

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
		DCC_LOG1(LOG_ERROR, "<%05x> NULL", (int)__tp);
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

