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
 * @file tcpip/raw.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TCPIP_RAW_H__
#define __TCPIP_RAW_H__

struct raw_pcb;

#ifdef __cplusplus
extern "C" {
#endif

struct raw_pcb * raw_pcb_new(int __protocol);

int raw_close(struct raw_pcb * __raw);

int raw_sendto(struct raw_pcb * __raw, void * __buf, int __len, 
			   const struct sockaddr_in * __sin);

int raw_recvfrom(struct raw_pcb * __raw, void * __buf, int __len, 
			 struct sockaddr_in * __sin);

int raw_recvfrom_tmo(struct raw_pcb * __raw, void * __buf, 
				 int __len, struct sockaddr_in * __sin, 
				 unsigned int msec);

int raw_ioctl(struct raw_pcb * __raw, int __cmd, void * __data);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_RAW_H__ */

