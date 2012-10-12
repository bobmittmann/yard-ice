/* $Id: tcp_enum.c,v 2.4 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcp_enum.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <sys/types.h>
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

int tcp_enum(int (* __callback)(struct tcp_pcb *, void *), void * __parm)
{
	int n = 0;

	n = pcb_enum((int (*)(struct pcb *, void *))__callback,
				   __parm, &__tcp__.closed);

	n += pcb_enum((int (*)(struct pcb *, void *))__callback,
				   __parm, &__tcp__.listen);

	n += pcb_enum((int (*)(struct pcb *, void *))__callback,
					__parm, &__tcp__.active);

	return n;
}

