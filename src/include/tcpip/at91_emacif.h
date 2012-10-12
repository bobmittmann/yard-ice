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

#ifndef __CS8900IF_H__
#define __CS8900IF_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/at91.h>

#include <stdint.h>
#include <stdlib.h>
#include <uthreads.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>

#include <tcpip/ifnet.h>

/* 
 * EMAC io buffers
 *
 * This structure defines the transmit and receive queues
 *
 * The receive queue comprises of 24 receive buffers of 128 bytes each
 * The transmitter queue has only one buffer of 1500 bytes.
 *
 */

#ifndef AT91_EMAC_MAX_PKT_SIZE
#define AT91_EMAC_MAX_PKT_SIZE 1024
#endif

#ifndef AT91_EMAC_RX_BUF_COUNT
#define AT91_EMAC_RX_BUF_COUNT 16
#endif

#ifndef AT91_EMAC_RX_BUF_SIZE
#define AT91_EMAC_RX_BUF_SIZE 128
#endif

#ifndef AT91_EMAC_TX_BUF_COUNT
#define AT91_EMAC_TX_BUF_COUNT 2
#endif

#ifndef AT91_EMAC_TX_BUF_SIZE
#define AT91_EMAC_TX_BUF_SIZE AT91_EMAC_MAX_PKT_SIZE
#endif

#ifndef ENABLE_AT91_EMAC_STATS
#define ENABLE_AT91_EMAC_STATS 0
#endif

#ifndef AT91_EMAC_INPUT_STACK_SIZE
#define AT91_EMAC_INPUT_STACK_SIZE 512
#endif

#ifndef AT91_EMAC_ISR_STACK_SIZE
#define AT91_EMAC_ISR_STACK_SIZE 128
#endif

struct emac_buf_desc {
	uint32_t ptr;
	uint32_t opt;
};

struct at91_emac_drv {
	struct {
		uint32_t buf[AT91_EMAC_RX_BUF_COUNT] \
		            [AT91_EMAC_RX_BUF_SIZE / sizeof(uint32_t)];
		struct emac_buf_desc desc[AT91_EMAC_RX_BUF_COUNT];
		uint32_t dma;
		uint32_t pkt[AT91_EMAC_MAX_PKT_SIZE / sizeof(uint32_t)];
	} rx;
	struct {
		struct eth_hdr hdr;
		uint32_t buf[AT91_EMAC_TX_BUF_COUNT] \
		            [AT91_EMAC_TX_BUF_SIZE / sizeof(uint32_t)];
		struct emac_buf_desc desc[AT91_EMAC_TX_BUF_COUNT];
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
	uint32_t input_stack[AT91_EMAC_INPUT_STACK_SIZE / sizeof(uint32_t)];
	uint32_t isr_stack[AT91_EMAC_ISR_STACK_SIZE / sizeof(uint32_t)];
} __attribute__ ((aligned (16)));

extern const struct ifnet_operations at91_emac_ifnet_op;

#ifdef __cplusplus
extern "C" {
#endif

int at91_emac_phy_probe(struct at91_emac_drv * drv);

int at91_emac_phy_init(struct at91_emac_drv * drv);

void at91_emac_phy_restart(struct at91_emac_drv * drv);

int at91_emac_phy_reset(struct at91_emac_drv * drv);

#ifdef __cplusplus
}
#endif

#endif /* __CS8900IF_H__ */

