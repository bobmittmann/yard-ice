/* $Id: udp.c,v 2.16 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	udp_connect.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <stdlib.h>

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
		__os_cond_signal(__up->u_rcv_cond);
	} else {
		if (pcb_lookup(__addr, __port, __up->u_laddr, 
					   __up->u_lport, &__udp__.list)) {
			DCC_LOG3(LOG_WARNING, "<%05x> %I:%d in use", (int)__up,
					 __addr, ntohs(__port));
			ret = -EADDRINUSE;
		} else {
			__up->u_faddr = __addr;
			__up->u_fport = __port;
			__os_cond_signal(__up->u_rcv_cond);
		}
	}

	tcpip_net_unlock();

	return ret;
}

