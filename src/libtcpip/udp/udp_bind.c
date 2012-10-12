/* $Id: udp.c,v 2.16 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	udp_bind.c
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

