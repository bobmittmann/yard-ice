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

#if ENABLE_TCP_SEND

#ifndef TCP_SND_BUF_LEN
#define TCP_SND_BUF_LEN 1024
#endif

int cmd_tcp_send(FILE * f, int argc, char ** argv)
{
	struct debugger * dbg = &debugger;
	uint8_t buf[TCP_SND_BUF_LEN];
	value_t val;
	struct tcp_pcb * svc;
	struct tcp_pcb * tp;
	uint32_t addr;
	uint32_t port;
	uint32_t size;
//	struct timespec start;
//	struct timespec end;
	uint32_t start;
	unsigned int ms;
	int rem;
	uint8_t * ptr;
	int n;

	argc--;
	argv++;

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			return n;
		}
		addr = val.uint32;
		argc -= n;
		argv += n;
	} else {
		addr = (uint32_t)dbg->transf.base;
	}

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			return n;
		}
		size = val.uint32;
		argc -= n;
		argv += n;
	} else {
		size = (uint32_t)dbg->transf.size;
	}

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			return n;
		}
		port = val.uint32;
		argc -= n;
		argv += n;
	} else {
		port = dbg->tcp_port;
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

	fprintf(f, " - Read address: 0x%08x\n", addr);
	fprintf(f, " - Listening on port %d\n", port);

	if ((tp = tcp_accept(svc)) == NULL) {
		tcp_close(svc);
		return -1;
	}

	/* close listenning socket */
	tcp_close(svc);

#if 0
	fprintf(f, " - Connected to %d.%d.%d.%d:%d\n", 
			IP4_ADDR1(tp->t_faddr), IP4_ADDR2(tp->t_faddr), 
			IP4_ADDR3(tp->t_faddr), IP4_ADDR4(tp->t_faddr), 
			ntohs(tp->t_fport));
#endif

	rem = size;
	size = 0;
	ptr = buf;

	// clock_gettime(CLOCK_REALTIME, &start);
	start = thinkos_clock();

	for (;;) {
		n = (rem < TCP_SND_BUF_LEN) ? rem : TCP_SND_BUF_LEN;
		if ((n = target_mem_read(addr, buf, n)) <= 0) {
			fprintf(f, "!");
			break;
		}

		rem -= n;
		size += n;
		addr += n;

		n = tcp_send(tp, ptr, n, 0);
		if (n <= 0)
			break;

		fprintf(f, ".");
	}

	if (size) {
	//	clock_gettime(CLOCK_REALTIME, &end);

	//	ms = ((end.tv_sec - start.tv_sec) * 1000) + 
	//		((end.tv_nsec - start.tv_nsec) / 1000000);

		ms = (int32_t)(thinkos_clock() - start);

		fprintf(f, "\n - EOT: size=%d tm=%d[ms] speed=%d[bytes/sec]\n", 
				size, ms, (size * 1000) / ms);
	}

	tcp_close(tp);
	dbg->transf.size = size;

	return 0;
}

#endif /* ENABLE_TCP_SND */
