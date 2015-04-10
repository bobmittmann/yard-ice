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
 * @file sys/net.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_NET_H__
#define __SYS_NET_H__

#include <thinkos.h>
#include <stdint.h>

/* Network Framework mutex - all functions accessing the network stack must
 lock this mutex to avoid race conditions...  */
extern int32_t net_mutex;

#ifdef __cplusplus
extern "C" {
#endif

static inline int tcpip_net_lock(void) {
	return thinkos_mutex_lock(net_mutex);
}

static inline int tcpip_net_unlock(void) {
	return thinkos_mutex_unlock(net_mutex);
}

#ifdef __cplusplus
}
#endif

#endif /* __SYS_NET_H__ */

