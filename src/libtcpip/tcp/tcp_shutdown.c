/* $Id: tcp_shutdown.c,v 2.4 2006/09/06 15:37:21 bob Exp $ 
 *
 * File:	tcp_shutdown.c
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

#include <sys/msg.h>
#include <sys/mbuf.h>
#include <sys/mlink.h>
#include <sys/in.h>

#include <tcpip/ip.h>
#include <tcpip/tcp.h>
#include <errno.h>

#ifdef TCP_LOG
#ifndef ENABLE_LOG
#define ENABLE_LOG
#endif
#endif
#include <sys/dcclog.h>

#ifdef TCP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

#if defined(DEBUG) || defined(ENABLE_LOG)
extern const char * const __tcp_state[];
#endif

/*
   FIXME: this implementation is almost a clone of close call 
   We possibly will change the close() call to the shutdown one
   in the future. This function must be able to half close a 
   connection, etc...
 */
int tcp_shutdown(struct tcp_pcb * __tp, int __how)
{
	return 0;
}

