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
 * @file tcpip/ethif.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TCPIP_ETHIF_H__
#define __TCPIP_ETHIF_H__

#include <tcpip/ifnet.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ifnet * ethif_init(const uint8_t ethaddr[], in_addr_t ip_addr, in_addr_t netmask);

void ethif_stats_show(FILE * f, int clear);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_ETHIF_H__ */

