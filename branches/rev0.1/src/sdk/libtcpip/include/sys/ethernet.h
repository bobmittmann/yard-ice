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
 * @file sys/ethernet.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_ETHERNET_H__
#define __SYS_ETHERNET_H__

#include <stdint.h>

#define	ETH_ADDR_LEN    6
#define	ETH_PROTO_LEN   2
#define ETH_HEADER_LEN  14
#define ETH_ZERO_LEN    60
#define	ETH_CRC_LEN     4
#define ETH_DATA_LEN    1500		
#define ETH_FRAME_LEN   1514		
#define	ETH_MIN_LEN     (ETH_ZERO_LEN + ETH_CRC_LEN) 
#define	ETH_MAX_LEN     (ETH_FRAME_LEN + ETH_CRC_LEN)

/* Ethernet Protocol ID's. */
#define ETH_P_IP	0x0800		/* IPv4 */
#define ETH_P_ARP	0x0806		/* Address Resolution */
#define ETH_P_IPV6	0x86dd		/* IPv6 */

struct eth_hdr {
	uint8_t eth_dst[ETH_ADDR_LEN];
	uint8_t eth_src[ETH_ADDR_LEN];
	uint16_t eth_type;
};

typedef struct eth_hdr eth_hdr_t;

#define	ETH_MTU ETH_DATA_LEN

#endif /* __SYS_ETHERNET_H__ */

