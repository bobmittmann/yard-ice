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
 * @file gdb-rsp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include "gdbtcp-i.h"

#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>

#include <sys/dcclog.h>

int __attribute__((noreturn)) gdbtcpd_task(struct gdbtcpd * gdb)
{
	char pktbuf[RSP_BUFFER_LEN + 16];
	struct tcp_pcb * svc = gdb->svc;
	struct tcp_pcb * tp;

	DCC_LOG1(LOG_TRACE, "<%d>", thinkos_thread_self());
	INF("<%d>", thinkos_thread_self());

	gdb->noack_mode = false;
	gdb->thread.id = 1; 
	gdb->last_signal = TARGET_SIGNAL_0;//TRAP;

	for (;;) {
		gdb->running = false;

//		gdb->target.op->breakpoint_clear_all(gdb->target.arg);
//		gdb->target.op->watchpoint_clear_all(gdb->target.arg);

		if ((tp = tcp_accept(svc)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept().");
			break;
		}

		INFS("GDB: TCP accepted");
		DCC_LOG(LOG_TRACE, "GDB: TCP connection accepted.");

		gdb->tp = tp;
		rsp_comm_loop(gdb, pktbuf);

		tcp_close(tp);
		gdb->tp = NULL;

		DCC_LOG(LOG_TRACE, "GDB: TCP connection closed.");
	}

	for (;;);
}

uint32_t __attribute__((aligned(8))) gdb_srv_stack[(RSP_BUFFER_LEN)/4 + 512];

struct gdbtcpd gdbtcpd;

const struct thinkos_thread_inf gdb_srv_inf = {
	.stack_ptr = gdb_srv_stack, 
	.stack_size = sizeof(gdb_srv_stack),
	.priority = 32,
	.thread_id = 32,
	.paused = false,
	.tag = "GDB_SRV"
};

int gdbtcpd_start(void)
{
	struct gdbtcpd * gdb = &gdbtcpd;
	struct tcp_pcb * svc;
	char * env;
	int th;

	svc = tcp_alloc();

	tcp_bind(svc, INADDR_ANY, htons(1000));

	if (tcp_listen(svc, 1) != 0) {
		WARN("Can't register the TCP listner!");
		return -1;
	}

	gdb->svc = svc;
	gdb->tp = NULL;
	gdb->connected = false;
	gdb->mutex = thinkos_mutex_alloc();
	gdb->cond = thinkos_cond_alloc();

	if ((env = getenv("TEXT")) != NULL) {
		gdb->offs.text = strtoul(env, NULL, 16);
		INF("TEXT='0x%08x'", gdb->offs.text);
	} else
		gdb->offs.text = 0;

	if ((env = getenv("DATA")) != NULL) {
		gdb->offs.data = strtoul(env, NULL, 16);
		INF("DATA='0x%08x'", gdb->offs.data);
	} else
		gdb->offs.data = 0;

	mod_gdb_register(gdb);

	th = thinkos_thread_create_inf((void *)gdbtcpd_task, (void *)gdb, 
								   &gdb_srv_inf);

	INF("GDB server started th=%d", th);

	gdb_brk_start(gdb);


	(void)th;

	return 0;
}

