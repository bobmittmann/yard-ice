/* 
 * Copyright(C) 2015 Robinson Mittmann. All Rights Reserved.
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
 * @file simlnk.c
 * @brief MS/TP Link Layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

//#include "simlnk-i.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/param.h>
#include <sys/serial.h>
#include <sys/dcclog.h>

#include <thinkos.h>

#include "io.h"
#include "simlnk.h"

#undef DEBUG
#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_WARN
#include <trace.h>

#define SIMLNK_BAUDRATE 10000
#define SIMLNK_STACK_SIZE 1024
#define SIMLNK_MAX 5

unsigned int io_addr_get(void);
struct simlnk * simrpc_route(unsigned int daddr);

unsigned int simrpc_seq = 2;

static uint32_t simrpc_mkopc(unsigned int daddr, unsigned int insn)
{
	unsigned int saddr = io_addr_get();

	return daddr | (saddr << 8) | ((simrpc_seq << 16) & 0xff) || (insn << 24);
}


void simrpc_mem_lock(unsigned int daddr, uint32_t base, unsigned int size)
{
	struct simlnk * iface; 
	uint32_t pkt[3];

	iface = simrpc_route(daddr);
	if (iface == NULL)
		return;

	pkt[0] = simrpc_mkopc(daddr, SIMLNK_RPC_MEM_LOCK);
	pkt[1] = base;
	pkt[2] = size;

	simlnk_send(iface, pkt, 4 * 3);
}

void simrpc_mem_unlock(unsigned int daddr, uint32_t base, unsigned int size)
{
	struct simlnk * iface; 
	uint32_t pkt[3];

	iface = simrpc_route(daddr);
	if (iface == NULL)
		return;

	pkt[0] = simrpc_mkopc(daddr, SIMLNK_RPC_MEM_UNLOCK);
	pkt[1] = base;
	pkt[2] = size;

	simlnk_send(iface, pkt, 4 * 3);
}

/* ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 */

void __attribute__((noreturn)) simlnk_recv_task(struct simlnk * lnk)
{
	uint32_t pkt[520 / 4];
	INF("<%d> started...", thinkos_thread_self());

	for (;;) {
		simlnk_recv(lnk, pkt, sizeof(pkt));
		thinkos_sleep(1000);
	}
}

uint32_t simlnk_stack[SIMLNK_MAX][SIMLNK_STACK_SIZE];
struct simlnk * simlnk[SIMLNK_MAX];

const struct thinkos_thread_inf simlnk_recv_inf[SIMLNK_MAX] = { 
	{
		.stack_ptr = simlnk_stack[0],
		.stack_size = SIMLNK_STACK_SIZE,
		.priority = 32,
			.thread_id = 7,
			.paused = false,
			.tag = "LNKRCV1"
	},
	{
		.stack_ptr = simlnk_stack[1],
		.stack_size = SIMLNK_STACK_SIZE,
		.priority = 32,
			.thread_id = 8,
			.paused = false,
			.tag = "LNKRCV2"
	},
	{
		.stack_ptr = simlnk_stack[2],
		.stack_size = SIMLNK_STACK_SIZE,
		.priority = 32,
			.thread_id = 7,
			.paused = false,
			.tag = "LNKRCV3"
	},
	{
		.stack_ptr = simlnk_stack[3],
		.stack_size = SIMLNK_STACK_SIZE,
		.priority = 32,
			.thread_id = 9,
			.paused = false,
			.tag = "LNKRCV4"
	},
	{
		.stack_ptr = simlnk_stack[4],
		.stack_size = SIMLNK_STACK_SIZE,
		.priority = 32,
			.thread_id = 10,
			.paused = false,
			.tag = "LNKRCV5"
	}
};

void simrpc_init(void)
{
	struct serial_dev * ser;
	struct simlnk * lnk;

	lnk = simlnk_alloc();
	ser = stm32f_uart2_serial_dma_init(SIMLNK_BAUDRATE, SERIAL_8N1);
	simlnk_init(lnk, "SIM1", 1, ser);
	simlnk[0] = lnk;
	thinkos_thread_create_inf((void *)simlnk_recv_task, (void *)lnk,
							  &simlnk_recv_inf[0]);

	lnk = simlnk_alloc();
	ser = stm32f_uart3_serial_dma_init(SIMLNK_BAUDRATE, SERIAL_8N1);
	simlnk_init(lnk, "SIM2", 2, ser);
	simlnk[1] = lnk;
	thinkos_thread_create_inf((void *)simlnk_recv_task, (void *)lnk,
							  &simlnk_recv_inf[1]);

	lnk = simlnk_alloc();
	ser = stm32f_uart4_serial_dma_init(SIMLNK_BAUDRATE, SERIAL_8N1);
	simlnk_init(lnk, "SIM3", 3, ser);
	simlnk[2] = lnk;
	thinkos_thread_create_inf((void *)simlnk_recv_task, (void *)lnk,
							  &simlnk_recv_inf[2]);

	lnk = simlnk_alloc();
	ser = stm32f_uart5_serial_dma_init(SIMLNK_BAUDRATE, SERIAL_8N1);
	simlnk_init(lnk, "SIM4", 4, ser);
	simlnk[3] = lnk;
	thinkos_thread_create_inf((void *)simlnk_recv_task, (void *)lnk,
							  &simlnk_recv_inf[3]);

	lnk = simlnk_alloc();
	ser = stm32f_uart6_serial_dma_init(SIMLNK_BAUDRATE, SERIAL_8N1);
	simlnk_init(lnk, "SIM5", 5, ser);
	simlnk[4] = lnk;
	thinkos_thread_create_inf((void *)simlnk_recv_task, (void *)lnk,
							  &simlnk_recv_inf[4]);
}

struct simlnk * simrpc_route(unsigned int daddr) 
{
	struct simlnk * lnk = NULL;

	if ((daddr >= 1) && (daddr <= 5)) {
		lnk = simlnk[daddr - 1];
	}

	return lnk;
}


