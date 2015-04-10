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
 * @file udp_close.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <stdlib.h>
#include <errno.h>

int udp_close(struct udp_pcb * __up)
{
	int ret;

	if (__up == NULL) {
		DCC_LOG1(LOG_WARNING, "<%05x> invalid pcb", (int)__up);
		return -EFAULT;
	}

	tcpip_net_lock();

#if (ENABLE_NET_SANITY_CHECK)
	if (pcb_find((struct pcb *)__up, &__udp__.pcb) < 0) {
		DCC_LOG1(LOG_ERROR, "<%05x> pcb_find()", (int)__up);
		tcpip_net_unlock();
		/* TODO: errno */
		return -1;
	}
#endif

	if ((ret = udp_release(__up)) < 0) {
		DCC_LOG1(LOG_ERROR, "<%05x> udp_release()", (int)__up);
		/* TODO: errno */
		ret = -1;
	}

	tcpip_net_unlock();

	return ret;
}

