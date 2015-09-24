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
#include "simrpc.h"
#include "simrpc_svc.h"

#undef DEBUG
#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_WARN
#include <trace.h>

#define SIMLNK_STACK_SIZE 1024
#define SIMLNK_MAX 5

/* ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 */

void __attribute__((noreturn)) simlnk_loop(struct simlnk * lnk);

void __attribute__((noreturn)) simlnk_recv_task(struct simlnk * lnk)
{
	INF("<%d> started...", thinkos_thread_self());

	simlnk_loop(lnk);
}

uint32_t simlnk_stack[SIMLNK_MAX][SIMLNK_STACK_SIZE / 4];
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


#define SIMRPC_PCB_POOL_SIZE 4

struct simrpc_pcb_entry {
	union {
		struct simrpc_pcb_entry * next;
		struct simrpc_pcb pcb;
    };
};

struct {
	int mutex;
	struct simrpc_pcb_entry * free;
	struct simrpc_pcb_entry buf[SIMRPC_PCB_POOL_SIZE];
} simrpc_pcb_pool;

struct simrpc_pcb * simrpc_pcb_alloc(void)
{
	struct simrpc_pcb_entry * entry;

	thinkos_mutex_lock(simrpc_pcb_pool.mutex);
	if ((entry = simrpc_pcb_pool.free) != NULL)
		simrpc_pcb_pool.free = entry->next;

	thinkos_mutex_unlock(simrpc_pcb_pool.mutex);
	return &entry->pcb;
}

void simrpc_pcb_free(struct simrpc_pcb * pcb)
{
	struct simrpc_pcb_entry * entry = (struct simrpc_pcb_entry *)pcb;

	thinkos_mutex_lock(simrpc_pcb_pool.mutex);
	entry->next = simrpc_pcb_pool.free;
	simrpc_pcb_pool.free = entry;
	thinkos_mutex_unlock(simrpc_pcb_pool.mutex);
}

void simrpc_pcb_pool_init(void)
{
	int i;

	simrpc_pcb_pool.mutex = thinkos_mutex_alloc();
	simrpc_pcb_pool.free = &simrpc_pcb_pool.buf[0];

	for (i = 0; i < SIMRPC_PCB_POOL_SIZE - 1; ++i)
		simrpc_pcb_pool.buf[i].next = &simrpc_pcb_pool.buf[i + 1];

	simrpc_pcb_pool.buf[i].next = NULL;
}

void simrpc_init(void)
{
	struct serial_dev * ser;
	struct simlnk * lnk;

	simrpc_pcb_pool_init();

	lnk = simlnk_alloc();
	ser = stm32f_uart2_serial_dma_init(SIMLNK_BAUDRATE, 
									   SERIAL_8N1 | SERIAL_EOT_BREAK);
	simlnk_init(lnk, "SIM1", 1, ser);
	simlnk[0] = lnk;
	thinkos_thread_create_inf((void *)simlnk_recv_task, (void *)lnk,
							  &simlnk_recv_inf[0]);

	lnk = simlnk_alloc();
	ser = stm32f_uart3_serial_dma_init(SIMLNK_BAUDRATE, 
									   SERIAL_8N1 | SERIAL_EOT_BREAK);
	simlnk_init(lnk, "SIM2", 2, ser);
	simlnk[1] = lnk;
	thinkos_thread_create_inf((void *)simlnk_recv_task, (void *)lnk,
							  &simlnk_recv_inf[1]);

	lnk = simlnk_alloc();
	ser = stm32f_uart4_serial_dma_init(SIMLNK_BAUDRATE, 
									   SERIAL_8N1 | SERIAL_EOT_BREAK);
	simlnk_init(lnk, "SIM3", 3, ser);
	simlnk[2] = lnk;
	thinkos_thread_create_inf((void *)simlnk_recv_task, (void *)lnk,
							  &simlnk_recv_inf[2]);

	lnk = simlnk_alloc();
	ser = stm32f_uart5_serial_dma_init(SIMLNK_BAUDRATE, 
									   SERIAL_8N1 | SERIAL_EOT_BREAK);
	simlnk_init(lnk, "SIM4", 4, ser);
	simlnk[3] = lnk;
	thinkos_thread_create_inf((void *)simlnk_recv_task, (void *)lnk,
							  &simlnk_recv_inf[3]);

	lnk = simlnk_alloc();
	ser = stm32f_uart6_serial_dma_init(SIMLNK_BAUDRATE,
									   SERIAL_8N1 | SERIAL_EOT_BREAK);
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

struct simrpc_pcb * simrpc_open(unsigned int daddr)
{
	struct simrpc_pcb * sp;
	struct simlnk * lnk; 

	if ((sp = simrpc_pcb_alloc()) == NULL) {
		WARN("simrpc_pcb_alloc() failed!");
		return NULL;
	}

	if ((lnk = simrpc_route(daddr)) == NULL)
		return NULL;

	sp->lnk = lnk;
	sp->seq = 0;
	sp->daddr = daddr;
	sp->saddr = io_addr_get();
	sp->tmo = SIMRPC_DEF_TMO_MS;

	return sp;
}

int simrpc_set_timeout(struct simrpc_pcb * sp, unsigned int tmo_ms)
{
	sp->tmo = tmo_ms;
	return 0;
}

int simrpc_close(struct simrpc_pcb * sp)
{
	simrpc_pcb_free(sp);

	return 0;
}

