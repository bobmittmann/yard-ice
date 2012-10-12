/* $Id: net.c,v 1.2 2008/05/28 22:32:21 bob Exp $ 
 *
 * File:	net.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2008-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_NET__
#include <sys/net.h>

#include <sys/dcclog.h>

#ifndef ENABLE_NET_UDP
#define ENABLE_NET_UDP 1
#endif

#ifndef ENABLE_NET_TCP
#define ENABLE_NET_TCP 0
#endif

int8_t net_mutex;

void tcp_init(void);
void udp_init(void);

void __attribute__ ((constructor)) net_init(void)
{
	net_mutex = __os_mutex_alloc();

	DCC_LOG1(LOG_TRACE, "net_mutex=%d", net_mutex);

#if (ENABLE_NET_UDP)
	udp_init();
#endif

#if (ENABLE_NET_TCP)
	tcp_init();
#endif
}

