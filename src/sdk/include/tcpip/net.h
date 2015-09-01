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
 * @file tcpip/net.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TCPIP_NET_H__
#define __TCPIP_NET_H__

#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <tcpip/in.h>
#include <tcpip/tcp.h>
#include <tcpip/udp.h>
#include <tcpip/route.h>

#include <tcpip/ethif.h>
#include <tcpip/loopif.h>

#include <tcpip/dhcp.h>
#include <tcpip/httpd.h>

#ifdef __cplusplus
extern "C" {
#endif

void tcpip_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_NET_H__ */

