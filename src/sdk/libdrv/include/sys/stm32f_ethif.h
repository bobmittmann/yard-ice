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
 * @file sys/stm32f-ethif.h
 * @brief STM32F Ethernet driver
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_ETHIF_H__
#define __STM32F_ETHIF_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>
#include <sys/ethernet.h>

#include <stdint.h>
#include <stdlib.h>

#include <netinet/ip.h>
#include <netinet/in.h>

#include <tcpip/ifnet.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#ifndef STM32F_ETH_PAYLOAD_MAX
#define STM32F_ETH_PAYLOAD_MAX 1500
#endif

#ifndef STM32F_ETH_RX_BUF_SIZE
#define STM32F_ETH_RX_BUF_SIZE (STM32F_ETH_PAYLOAD_MAX + 16)
#endif

#ifndef STM32F_ETH_TX_BUF_SIZE
#define STM32F_ETH_TX_BUF_SIZE (STM32F_ETH_PAYLOAD_MAX + 16)
#endif

#ifndef STM32F_ETH_RX_NDESC
#define STM32F_ETH_RX_NDESC 4
#endif

#ifndef STM32F_ETH_TX_NDESC
#define STM32F_ETH_TX_NDESC 2
#endif

struct stm32f_eth_drv {
	struct ifnet * ifn;
	struct stm32f_eth * eth;
	int event;
	struct {
		uint32_t buf[STM32F_ETH_RX_NDESC][(STM32F_ETH_RX_BUF_SIZE + 3) / 4];
		struct rxdma_enh_desc desc[STM32F_ETH_RX_NDESC];
		int sem;
		unsigned int cnt;
	} rx;
	struct {
		struct eth_hdr hdr[STM32F_ETH_TX_NDESC];
		uint32_t buf[STM32F_ETH_TX_NDESC][(STM32F_ETH_TX_BUF_SIZE + 3) / 4];
		struct txdma_enh_desc desc[STM32F_ETH_TX_NDESC];
		int flag;
	} tx;
} __attribute__ ((aligned (8)));

extern const struct ifnet_operations stm32f_eth_ifnet_op;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F_ETHIF_H__ */

