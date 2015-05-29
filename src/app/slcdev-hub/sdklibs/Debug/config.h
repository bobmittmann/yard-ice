/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file config.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* -------------------------------------------------------------------------
 * TCP/IP
 * ------------------------------------------------------------------------- */

#define ENABLE_NETWORK 1
#define ENABLE_NET_TCP 1
#define ENABLE_NET_UDP 1
#define ENABLE_NET_RAW 1

#define ENABLE_TCPDUMP 0

#define IFNET_INTERFACES_MAX         2

#define MBUF_POOL_SIZE               128

#define TCP_FAST_TMR_MS              100
#define TCP_ENABLE_TIMEWAIT          1
#define TCP_ENABLE_HEADER_PREDICTION 1
#define NET_TCP_PCB_ACTIVE_MAX       64
#define NET_UDP_PCB_ACTIVE_MAX       1

#define TCP_DEFAULT_MSL_SEC          1
#define TCP_IDLE_DET_SEC             10
#define TCP_KEEP_ALIVE_PROBE_SEC     20
#define TCP_MAX_IDLE_SEC             60
#define TCP_MAX_SND_QUEUE            (1460 * 4)

/* use hardware checksum */
#define ENABLE_NET_TCP_CHECKSUM 0
#define ENABLE_NET_UDP_CHECKSUM 0

/* -------------------------------------------------------------------------
 * Ethernet Driver
 * ------------------------------------------------------------------------- */

#define ETH_PHY_RST_GPIO         STM32_GPIOE, 5

/* Number of reception DMA descriptors for the Ethernet driver */
#define STM32F_ETH_RX_NDESC      4
/* Number of transmision DMA descriptors for the Ethernet driver */
#define STM32F_ETH_TX_NDESC      4
/* Number of packet in the buffer pool */
#define PKTBUF_POOL_SIZE        10 

#endif /* __CONFIG_H__ */


