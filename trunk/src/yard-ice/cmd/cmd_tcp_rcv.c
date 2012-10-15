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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tcpip/tcp.h>
#include <netinet/in.h>
#include <sys/clock.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "eval.h"

#include <debug.h>

#ifdef MEM_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

//#define TCP_RCV_BUF_LEN 512
#define TCP_RCV_BUF_LEN 4096

#if 0
struct rcv_info {
	FILE * console;
	struct tcp_pcb * volatile tp;
	uint32_t addr;
	uint32_t size;
	volatile int ret;
};

static void tcp_rcv_task(struct rcv_info * info, uthread_id_t id)
{
	uint8_t buf[TCP_RCV_BUF_LEN];
	struct tcp_pcb * svc;
	struct tcp_pcb * tp;
	FILE * f;
	uint32_t addr;
	uint32_t size;
	struct timespec start;
	struct timespec end;
	unsigned int ms;
	int rem;
	uint8_t * ptr;
	int n;

	f = info->console;
	svc = info->tp;

	if ((tp = tcp_accept(svc)) == NULL) {
		DBG(DBG_ERROR, "tcp_accept().");
		tcp_close(svc);
		info->ret = -1;		
		return;
	}

	info->tp = tp;

	/* close listenning socket */
	tcp_close(svc);

	fprintf(f, " - Connected to %d.%d.%d.%d:%d\n", 
			IP4_ADDR1(tp->t_faddr), IP4_ADDR2(tp->t_faddr), 
			IP4_ADDR3(tp->t_faddr), IP4_ADDR4(tp->t_faddr), 
			ntohs(tp->t_fport));


	addr = info->addr;
	size = 0;
	rem = TCP_RCV_BUF_LEN;
	ptr = buf;

	clock_gettime(CLOCK_REALTIME, &start);

	for (;;) {

		n = tcp_recv(tp, ptr, rem);
		if (n <= 0)
			break;

		rem -= n;
		ptr += n;

		if (rem == 0) {
			if ((n = target_mem_write(addr, buf, TCP_RCV_BUF_LEN)) <= 0) {
				fprintf(f, "!");
				break;
			}
			fprintf(f, ".");
			rem = TCP_RCV_BUF_LEN;
			ptr = buf;
			size += n;
			addr += n;
		}
	}

	if (rem < TCP_RCV_BUF_LEN) {
		n = TCP_RCV_BUF_LEN - rem;
		if ((n = target_mem_write(addr, buf, n)) <= 0) {
			fprintf(f, "!");
		} else {
			fprintf(f, ".");
			size += n;
			addr += n;
		}
	}

	if (size) {
		clock_gettime(CLOCK_REALTIME, &end);

		ms = ((end.tv_sec - start.tv_sec) * 1000) + 
			((end.tv_nsec - start.tv_nsec) / 1000000);

		fprintf(f, "\n - EOT: size=%d tm=%d(ms) speed=%d(KiB/s)\n", 
				size, ms, size / ms);
	}

	info->size = size;
	info->ret = 0;
}

static uint32_t stack[256 + 128];
#endif

int cmd_tcp_recv(FILE * f, int argc, char ** argv)
{
	struct debugger * dbg = &debugger;
	uint8_t buf[TCP_RCV_BUF_LEN];
	value_t val;
	struct tcp_pcb * svc;
	struct tcp_pcb * tp;
	uint32_t addr;
	uint32_t port;
	uint32_t size;
	struct timespec start;
	struct timespec end;
	unsigned int ms;
	int rem;
	uint8_t * ptr;
	int n;

//	int c;
	
	argc--;
	argv++;

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DBG(DBG_WARNING, "target_eval(), addr");
			return n;
		}
		addr = val.uint32;
		argc -= n;
		argv += n;
		DBG(DBG_TRACE, "eval: addr=%08x n=%d", addr, n);
	} else {
		addr = (uint32_t)dbg->transf.base;
	}

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DBG(DBG_WARNING, "target_eval(), port");
			return n;
		}
		port = val.uint32;
		argc -= n;
		argv += n;
		DBG(DBG_TRACE, "eval: port=%08x n=%d", port, n);
	} else {
		port = dbg->tcp_port;
		DBG(DBG_TRACE, "port=%d", port);
	}

	if (argc) {
		fprintf(f, "Too many arguments...\n");
		return -1;
	}

	dbg->transf.base = addr & ~0x03;
	dbg->tcp_port = port;

	svc = tcp_alloc();
	tcp_bind(svc, INADDR_ANY, htons(port));

	if (tcp_listen(svc, 1) != 0) {
		fprintf(f, "Can't register the TCP listner!\n");
		return -1;
	}

	fprintf(f, " - Write address: 0x%08x\n", addr);
	fprintf(f, " - Listening on port %d\n", port);


	if ((tp = tcp_accept(svc)) == NULL) {
		DBG(DBG_ERROR, "tcp_accept().");
		tcp_close(svc);
		return -1;
	}


	/* close listenning socket */
	tcp_close(svc);

	fprintf(f, " - Connected to %d.%d.%d.%d:%d\n", 
			IP4_ADDR1(tp->t_faddr), IP4_ADDR2(tp->t_faddr), 
			IP4_ADDR3(tp->t_faddr), IP4_ADDR4(tp->t_faddr), 
			ntohs(tp->t_fport));

	size = 0;
	rem = TCP_RCV_BUF_LEN;
	ptr = buf;

	clock_gettime(CLOCK_REALTIME, &start);

	for (;;) {

		n = tcp_recv(tp, ptr, rem);
		if (n <= 0)
			break;

		rem -= n;
		ptr += n;

		if (rem == 0) {
			if ((n = target_mem_write(addr, buf, TCP_RCV_BUF_LEN)) <= 0) {
				fprintf(f, "!");
				break;
			}
			fprintf(f, ".");
			rem = TCP_RCV_BUF_LEN;
			ptr = buf;
			size += n;
			addr += n;
		}
	}

	if (rem < TCP_RCV_BUF_LEN) {
		n = TCP_RCV_BUF_LEN - rem;
		if ((n = target_mem_write(addr, buf, n)) <= 0) {
			fprintf(f, "!");
		} else {
			fprintf(f, ".");
			size += n;
			addr += n;
		}
	}

	if (size) {
		clock_gettime(CLOCK_REALTIME, &end);

		ms = ((end.tv_sec - start.tv_sec) * 1000) + 
			((end.tv_nsec - start.tv_nsec) / 1000000);

		fprintf(f, "\n - EOT: size=%d tm=%d[ms] speed=%d[bytes/sec]\n", 
				size, ms, (size * 1000) / ms);
	}

	tcp_close(tp);
	dbg->transf.size = size;

	return 0;
}

