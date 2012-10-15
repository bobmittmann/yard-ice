/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
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
 * @file netinet/udp.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __NETINET_UDP_H__
#define __NETINET_UDP_H__

#include <stdint.h>

struct udphdr {
	uint16_t sport;
	uint16_t dport;
	uint16_t len;
	uint16_t chksum;
};

#define UDP_HEADER_LEN 8
#define UDP_MAX_DATA_LEN (576 - (20 + UDP_HEADER_LEN))

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __NETINET_UDP_H__ */

