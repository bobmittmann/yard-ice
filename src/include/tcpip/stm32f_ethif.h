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
#define STM32F_ETH_RX_BUF_SIZE (STM32F_ETH_PAYLOAD_MAX + 14)
#endif

#ifndef STM32F_ETH_TX_BUF_COUNT
#define STM32F_ETH_TX_BUF_COUNT 2
#endif

#ifndef STM32F_ETH_TX_BUF_SIZE
#define STM32F_ETH_TX_BUF_SIZE STM32F_ETH_PAYLOAD_MAX
#endif

#ifndef ENABLE_STM32F_ETH_STATS
#define ENABLE_STM32F_ETH_STATS 0
#endif

#ifndef STM32F_ETH_INPUT_STACK_SIZE
#define STM32F_ETH_INPUT_STACK_SIZE 512
#endif

struct rx_dma_desc {
	uint32_t status: 16;
	uint32_t fl: 14; /* Frame length */
	uint32_t afm: 1; /* Destination address filter fail */
	uint32_t own: 1; /* Own bit */

	uint32_t rbs1: 13; /* Receive buffer 1 size */
	uint32_t res1: 1;
	uint32_t rch: 1; /* Second address chained */
	uint32_t rer: 1; /* Receive end of ring */
	uint32_t rbs2: 15; /* Receive buffer 2 size */
	uint32_t dic: 1; /* Disable interrupt on completion */

	union {
		uint32_t * rbap1; /* Receive buffer 1 address pointer */
		uint32_t rtsl; /* Receive frame time stamp low */
	};
	union {
		uint32_t * rbap2; /* Receive buffer 2 address pointer */
		uint32_t rtsh; /* Receive frame time stamp high */
	};
};

/* FIXME: */
struct tx_dma_desc {
	uint32_t ptr;
	uint32_t opt;
};

struct stm32f_eth_drv {
	struct ifnet * ifn;
	struct stm32f_eth * eth;
	struct {
		uint32_t buf[STM32F_ETH_RX_BUF_COUNT] \
		            [STM32F_ETH_RX_BUF_SIZE / sizeof(uint32_t)];
		struct rx_dma_desc desc[STM32F_ETH_RX_BUF_COUNT];
		uint32_t dma;
	} rx;
	struct {
		struct eth_hdr hdr;
		uint32_t buf[STM32F_ETH_TX_BUF_COUNT] \
		            [STM32F_ETH_TX_BUF_SIZE / sizeof(uint32_t)];
		struct tx_dma_desc desc[STM32F_ETH_TX_BUF_COUNT];
		uint32_t dma;
	} tx;
	/* input thread id */
	int8_t input_thread;
	/* ISR (interrupt service routine) thread id */
	int8_t isr_thread;
	/* TX semaphore */
	int8_t tx_sem;
	/* RX semaphore */
	int8_t rx_sem;
	/* Mgm semaphore */
	int8_t phy_sem;
	/* Mgm phy address semaphore */
	int8_t phy_addr;
	uint32_t input_stack[STM32F_ETH_INPUT_STACK_SIZE / sizeof(uint32_t)];
} __attribute__ ((aligned (16)));

extern const struct ifnet_operations stm32f_eth_ifnet_op;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F_ETHIFIF_H__ */

