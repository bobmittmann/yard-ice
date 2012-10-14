/*
 * File:	cs8900if.h
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment:	
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __STM32F_ETHIFIF_H__
#define __STM32F_ETHIFIF_H__

#ifdef CONFIG_H
#include "config.h"
#endif
#include <sys/dcclog.h>

#include <sys/stm32f.h>

#include <stdint.h>
#include <stdlib.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>

#include <tcpip/ifnet.h>
#include <sys/os.h>


/* 
 * EMAC io buffers
 *
 * This structure defines the transmit and receive queues
 *
 * The receive queue comprises of 24 receive buffers of 128 bytes each
 * The transmitter queue has only one buffer of 1500 bytes.
 *
 */

#ifndef STM32F_ETH_PAYLOAD_MAX
#define STM32F_ETH_PAYLOAD_MAX 1500
#endif

#ifndef STM32F_ETH_RX_BUF_COUNT
#define STM32F_ETH_RX_BUF_COUNT 2
#endif

#ifndef STM32F_ETH_RX_BUF_SIZE
#define STM32F_ETH_RX_BUF_SIZE (STM32F_ETH_PAYLOAD_MAX + 16)
#endif

#ifndef STM32F_ETH_TX_BUF_COUNT
#define STM32F_ETH_TX_BUF_COUNT 2
#endif

#ifndef STM32F_ETH_TX_BUF_SIZE
#define STM32F_ETH_TX_BUF_SIZE (STM32F_ETH_PAYLOAD_MAX + 16)
#endif

#ifndef ENABLE_STM32F_ETH_STATS
#define ENABLE_STM32F_ETH_STATS 0
#endif

#ifndef STM32F_ETH_INPUT_STACK_SIZE
#define STM32F_ETH_INPUT_STACK_SIZE 512
#endif

struct stm32f_eth_drv {
	struct ifnet * ifn;
	struct stm32f_eth * eth;
	struct {
		uint32_t buf[STM32F_ETH_RX_BUF_SIZE / sizeof(uint32_t)];
		struct rxdma_enh_desc desc[1];
		int ev;
	} rx;
	struct {
		struct eth_hdr hdr;
		uint32_t buf[STM32F_ETH_TX_BUF_SIZE / sizeof(uint32_t)];
		struct txdma_enh_desc desc[1];
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

#endif /* __STM32F_ETHIFIF_H__ */

