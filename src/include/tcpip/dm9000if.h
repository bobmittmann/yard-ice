/*
 * File:	dm9000if.h
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment:	
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __DM9000IF_H__
#define __DM9000IF_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <stdlib.h>
#include <uthreads.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>

#include <tcpip/ifnet.h>
#include <sys/dm9000a.h>

#ifndef DM9000_MAX_PKT_SIZE
#define DM9000_MAX_PKT_SIZE (1024 + 16)
#endif

#ifndef DM9000_RX_BUF_SIZE
#define DM9000_RX_BUF_SIZE DM9000_MAX_PKT_SIZE
#endif

#ifndef DM9000_TX_BUF_SIZE
#define DM9000_TX_BUF_SIZE DM9000_MAX_PKT_SIZE
#endif

#ifndef ENABLE_DM9000_STATS
#define ENABLE_DM9000_STATS 0
#endif

#ifndef DM9000_INPUT_STACK_SIZE
#define DM9000_INPUT_STACK_SIZE 1024
#endif

#ifndef DM9000_ISR_STACK_SIZE
#define DM9000_ISR_STACK_SIZE 256
#endif

struct dm9000_drv {
	/* input thread id */
	int8_t input_thread;
	/* ISR (interrupt service routine) thread id */
	int8_t isr_thread;
	/* TX semaphore */
	int8_t tx_sem;
	/* RX semaphore */
	int8_t rx_sem;
	/* receive buffer */
	uint16_t rx_buf[DM9000_RX_BUF_SIZE / sizeof(uint16_t)];
	/* transmit buffer */
	uint16_t tx_buf[DM9000_TX_BUF_SIZE / sizeof(uint16_t)];
	/* input thread stack */
	uint32_t input_stack[DM9000_INPUT_STACK_SIZE / sizeof(uint32_t)];
	/* interrupt thread stack */
	uint32_t isr_stack[DM9000_ISR_STACK_SIZE / sizeof(uint32_t)];
};

extern const struct ifnet_operations dm9000a_ifnet_op;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DM9000IF_H__ */

