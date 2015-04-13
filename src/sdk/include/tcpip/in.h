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
 * @file tcpip/in.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef	__TCPIP_IN_H__
#define	__TCPIP_IN_H__

#include <stdint.h>

#include <netinet/in.h> /* in_addr_t */

#ifndef __BYTE_ORDER 
#error "Undefined __BYTE_ORDER"
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define IP4_ADDR1(A) ((int)(((in_addr_t)(A)) & 0xff))
#define IP4_ADDR2(A) ((int)((((in_addr_t)(A)) >> 8) & 0xff))
#define IP4_ADDR3(A) ((int)((((in_addr_t)(A)) >> 16) & 0xff))
#define IP4_ADDR4(A) ((int)((((in_addr_t)(A)) >> 24) & 0xff))

#define IP4_ADDR(A, B, C, D) ((((A) & 0xff) << 0) | \
		(((B) & 0xff) << 8) | (((C) & 0xff) << 16) | (((D) & 0xff) << 24))

#define IPV4_ADDR(A, B, C, D) IP4_ADDR((A), (B), (C), (D))

#define NTOHL(x) \
	((uint32_t)((((uint32_t)(x) & 0x000000ffU) << 24) | \
	(((uint32_t)(x) & 0x0000ff00U) <<  8) | \
	(((uint32_t)(x) & 0x00ff0000U) >>  8) | \
	(((uint32_t)(x) & 0xff000000U) >> 24)))

#define HTONL NTOHL

#define NTOHS(x) \
	((uint16_t)((((uint16_t)(x) & 0x00ff) << 8) | \
	(((uint16_t)(x) & 0xff00) >> 8)))

#define HTONS NTOHS

#elif __BYTE_ORDER == __BIG_ENDIAN

#else
#error "Undefined __BYTE_ORDER"
#endif

#ifdef __cplusplus
extern "C" {
#endif

uint16_t in_chksum(uint32_t, const void * __data, int __len);

#ifdef __cplusplus
}
#endif

#endif	/* __TCPIP_IN_H__ */

