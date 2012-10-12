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

#include <sys/cs8900.h>
#include <uthreads.h>

#include <net/ethernet.h>
#include <tcpip/etharp.h>

#include <tcpip/ifnet.h>
#include <tcpip/ip.h>

#include <sys/net.h>

#ifndef CS8900IF_INPUT_STACK_SIZE
#define CS8900IF_INPUT_STACK_SIZE 320
#endif

#ifndef CS8900IF_BUF_SIZE
#define CS8900IF_BUF_SIZE (512 + 40)
#endif

#ifndef ENABLE_CS8900_STATS
#define ENABLE_CS8900_STATS 0
#endif

struct cs8900if_drv {
#if (ENABLE_CS8900_STATS)
	struct {
		uint32_t rx_event = 0;
		uint32_t rx_count = 0;
		uint32_t isq_count = 0;
		uint32_t tx_event = 0;
		uint32_t buf_event = 0;
		uint32_t rx_miss_event = 0;
		uint32_t tx_col_event = 0;
	} stat;
#endif
	int8_t thread;
	int8_t res;
	int16_t addr[3];
	uint32_t stack[(CS8900IF_INPUT_STACK_SIZE) / sizeof(uint32_t)];
	uint16_t buf[(CS8900IF_BUF_SIZE) / sizeof(uint16_t)];
};

extern const struct ifnet_operations cs8900if_op;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CS8900IF_H__ */

