/* $Id: raw.c,v 2.1 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	raw_recvfrom.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */


#define __USE_SYS_RAW__
#include <sys/raw.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>

int raw_recvfrom(struct raw_pcb * __raw, void * __buf, 
				 int __len, struct sockaddr_in * __sin)
{
	int n;

	DCC_LOG2(LOG_TRACE, "<%05x> len=%d", (int)__raw, __len);
	
	tcpip_net_lock();

	DCC_LOG2(LOG_INFO, "<%05x> lock [%d]", (int)__raw, net_mutex);

	while (__raw->r_len == 0) {

		if (__raw->r_flags & RF_NONBLOCK) {
			tcpip_net_unlock();
			return -EAGAIN;
		}

		DCC_LOG3(LOG_INFO, "<%05x> wait [%d, %d]", (int)__raw, 
			__raw->r_cond, net_mutex);

		__os_cond_wait(__raw->r_cond, net_mutex);

		DCC_LOG2(LOG_INFO, "<%05x> lock [%d]", (int)__raw, net_mutex);
	}

	if (__sin != NULL) {
		__sin->sin_family = AF_INET;
		__sin->sin_port = 0;
		__sin->sin_addr.s_addr = __raw->r_faddr;
	}

	n = (__raw->r_len > __len) ? __len : __raw->r_len;

	memcpy(__buf, __raw->r_buf, n);

	__raw->r_len = 0;

	DCC_LOG2(LOG_TRACE, "<%05x> len=%d", (int)__raw, n);

	__os_cond_signal(__raw->r_cond);

	tcpip_net_unlock();

	return n;
}

