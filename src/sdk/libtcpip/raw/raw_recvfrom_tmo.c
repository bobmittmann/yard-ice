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
 * @file raw_recvfrom_tmo.c
 * @brief IP layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_RAW__
#include <sys/raw.h>

#include <string.h>
#include <stdlib.h>
#include <errno.h>

int raw_recvfrom_tmo(struct raw_pcb * __raw, void * __buf, 
				 int __len, struct sockaddr_in * __sin, 
				 unsigned int msec)
{
	int n;
	int err;

	DCC_LOG2(LOG_INFO, "<%05x> len=%d", (int)__raw, __len);
	
	tcpip_net_lock();

	DCC_LOG2(LOG_INFO, "<%05x> lock [mtx=%d]", (int)__raw, net_mutex);

	while (__raw->r_len == 0) {

		if (__raw->r_flags & RF_NONBLOCK) {
			tcpip_net_unlock();
			return -EAGAIN;
		}

		DCC_LOG3(LOG_INFO, "<%05x> wait [cnd=%d mtx=%d]", (int)__raw, 
				 __raw->r_cond, net_mutex);

		if ((err = thinkos_cond_timedwait(__raw->r_cond, 
										  net_mutex, msec)) < 0) {
			tcpip_net_unlock();
			if (err == THINKOS_ETIMEDOUT) {
				err = -ETIMEDOUT;
				DCC_LOG(LOG_INFO, "timeout!");
			} else {
				DCC_LOG(LOG_ERROR, "thinkos_cond_timedwait()!");
			}
			return err;
		}

		DCC_LOG2(LOG_INFO, "<%05x> lock [mtx=%d]", (int)__raw, net_mutex);
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

	tcpip_net_unlock();

	return n;
}

