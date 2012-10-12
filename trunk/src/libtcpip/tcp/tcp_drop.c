/* $Id: tcp_drop.c,v 2.4 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcp_drop.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment: API function.
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdint.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>

#include <sys/mbuf.h>
#include <sys/in.h>
#include <tcpip/ip.h>
#include <tcpip/tcp.h>

#ifdef TCP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

/* 
 Abort an active connection or refuse an incomming one; 
 API function;
*/
int tcp_drop(struct tcp_pcb * __tp)
{
	if (pcb_find((struct pcb *)__tp, &__tcp__.active) < 0) {
		DBG(DBG_ERROR, "<%04x> pcb_find()", (int)__tp);
		return -1;
	}

	if (TCPS_HAVERCVDSYN(__tp->t_state)) {
		tcp_abort(__tp);
		/* TODO: tcpstat.tcps_conndrops++; */
	} else {
		/* TODO: tcpstat.tcps_drops++; */
		tcp_pcb_free(__tp);
	}

	return 0;
}

