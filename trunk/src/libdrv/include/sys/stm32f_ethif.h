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
#include <sys/dcclog.h>

#include <sys/stm32f.h>

#include <sys/ethernet.h>

#include <stdint.h>
#include <stdlib.h>

#include <netinet/ip.h>
#include <netinet/in.h>

#include <tcpip/ifnet.h>
#include <sys/os.h>

#ifndef STM32F_ETH_PAYLOAD_MAX
#define STM32F_ETH_PAYLOAD_MAX 1500
#endif

#ifndef STM32F_ETH_RX_BUF_SIZE
#define STM32F_ETH_RX_BUF_SIZE (STM32F_ETH_PAYLOAD_MAX + 16)
#endif

#ifndef STM32F_ETH_TX_BUF_SIZE
#define STM32F_ETH_TX_BUF_SIZE (STM32F_ETH_PAYLOAD_MAX + 16)
#endif

#ifndef STM32F_ETH_INPUT_STACK_SIZE
#define STM32F_ETH_INPUT_STACK_SIZE 512
#endif

struct stm32f_eth_drv {
	struct ifnet * ifn;
	struct stm32f_eth * eth;
	struct {
		uint32_t buf[STM32F_ETH_RX_BUF_SIZE / sizeof(uint32_t)];
		struct rxdma_enh_desc desc;
		int ev;
	} rx;
	struct {
		struct eth_hdr hdr;
		uint32_t buf[STM32F_ETH_TX_BUF_SIZE / sizeof(uint32_t)];
		struct txdma_enh_desc desc;
		int ev;
	} tx;
	uint32_t stack[STM32F_ETH_INPUT_STACK_SIZE / sizeof(uint32_t)];
} __attribute__ ((aligned (16)));

extern const struct ifnet_operations stm32f_eth_ifnet_op;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F_ETHIF_H__ */

