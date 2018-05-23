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
 * @file gdb-tcp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#define __GDB_RSP__
#include "gdb-rsp-i.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <tcpip/tcp.h>
#include "debugger.h"

#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>
#include <sys/dcclog.h>

struct gdb_tcpd {
	int8_t noack_mode;
	int8_t run_flag;
	int8_t con_flag;
	struct gdb_rsp * gdb;
	struct tcp_pcb * svc;
	volatile struct tcp_pcb * tp;
};

int gdb_tcp_init(struct tcp_pcb * tp)
{
	return 0;
}

int gdb_tcp_send(struct tcp_pcb * tp, const void * buf, unsigned int len)
{
	INF("TX buf=%08x, len=%d", (uint32_t)buf, len);
//	INFA("TX", buf, len);
	return tcp_send(tp, buf, len, TCP_SEND_NOWAIT);
}

int gdb_tcp_recv(struct tcp_pcb * tp, void * buf, unsigned int len)
{
	int ret;

	ret = tcp_recv(tp, buf, len);
	if (ret > 0)
//		INFA("RX", buf, ret);
		INF("RX %d", ret);
	else
		WARN("TCP ERR: %d", ret);

	return ret;
}

int gdb_tcp_flush(struct tcp_pcb * tp)
{
	return 0;
}

int gdb_tcp_drain(struct tcp_pcb * tp)
{
	return 0;
}

int gdb_tcp_settmo(struct tcp_pcb * tp, unsigned int ms)
{
	return 0;
}

const struct gdb_comm_op gdb_tcp_op = {
	.init = (void *)gdb_tcp_init,
	.send = (void *)gdb_tcp_send,
	.recv = (void *)gdb_tcp_recv,
	.flush = (void *)gdb_tcp_flush,
	.drain = (void *)gdb_tcp_drain,
	.settmo = (void *)gdb_tcp_settmo
};

/*
int rsp_cmd(struct tcp_pcb * tp, char * pkt, int len)
{
	char * cp = pkt + 6;
	char * s = pkt;
	FILE * f;
	int ret;
	int c;
	int i;

	if ((f = rsp_fopen(tp)) == NULL) {
		DCC_LOG(LOG_ERROR, "rsp_fopen() failed!");
		return rsp_error(tp, -1);
	}

	len -= 6;
	DCC_LOG1(LOG_TRACE, "len=%d", len);

	for (i = 0; i < (len / 2); i++) {
		c = tochar(cp);
		cp += 2;
		s[i] = c;
	}
	s[i] = '\0';

	INF("\"%s\"", s);

	if ((ret = exec(f, yard_ice_cmd_tab, s)) < 0) {
		DCC_LOG1(LOG_ERROR, "shell_exec(): %d", ret);
		rsp_fclose(f);
		return rsp_error(tp, -ret);
	}

	rsp_fclose(f);
	return rsp_ok(tp);
} */

int __attribute__((noreturn)) gdb_brk_task(struct gdb_tcpd * tcpd)
{
	struct gdb_rsp * gdb = tcpd->gdb;
	int sig = 5;
	int state;

	DCC_LOG1(LOG_TRACE, "<%d>", thinkos_thread_self());
	INF("<%d>", thinkos_thread_self());

	for (;;) {
		/* wait for a connection */
		DBGS("BRK: waiting connect...");
		thinkos_flag_take(tcpd->con_flag);

		/* wait for a 'target run' indication */
		DBGS("BRK: waiting run...");

		/* wait for a target run */
		thinkos_flag_take(tcpd->run_flag);

		DBGS("BRK: waiting halt...");

		while ((state = target_halt_wait(5000)) == ERR_TIMEOUT) {
			DBGS("BRK: waiting...");
		}

		if (state == DBG_ST_HALTED) {
			DBGS("BRK: target halted");

			thinkos_flag_clr(tcpd->run_flag);

			gdb_rsp_on_halt(gdb, sig);
		}

		state = target_status();
		if (state < DBG_ST_CONNECTED) {
			WARNS("BRK: target not connected!");
			gdb_rsp_error(gdb, state);
			continue;
		}

		if (state != DBG_ST_HALTED) {
			DBGS("BRK: target running");

			if ((state = target_halt(0)) < 0) {
				DCC_LOG(LOG_WARNING, "target_halt() failed!");
				gdb_rsp_puts(gdb, "YARD-ICE: halt fail\n");
				gdb_rsp_error(gdb, 1);
				continue;
			}

			if ((state = target_halt_wait(500)) == ERR_TIMEOUT) {
				DCC_LOG(LOG_TRACE, "timeout...");
				gdb_rsp_puts(gdb, "YARD-ICE: target_halt failed!");
				gdb_rsp_error(gdb, 1);
				continue;
			}
		} 

		switch (state) {
		case DBG_ST_ERROR:
			gdb_rsp_puts(gdb, "YARD-ICE: error state\n");
			break;
		case DBG_ST_OUTOFSYNC:
			DCC_LOG(LOG_TRACE, "out of sync");
			gdb_rsp_puts(gdb, "YARD-ICE: Out of sync\n");
			break;
		case DBG_ST_BUSY:
			DCC_LOG(LOG_TRACE, "busy...");
			gdb_rsp_puts(gdb, "YARD-ICE: busy ... \n");
			break;
		case DBG_ST_UNDEF:
			gdb_rsp_puts(gdb, "YARD-ICE: undefined state\n");
			break;
		case DBG_ST_UNCONNECTED:
			DCC_LOG(LOG_TRACE, "unconnected");
			gdb_rsp_puts(gdb, "YARD-ICE: unconnected ?\n");
			break;
		case DBG_ST_CONNECTED:
			DCC_LOG(LOG_TRACE, "connected");
			gdb_rsp_puts(gdb, "YARD-ICE: connected (busy)\n");
			break;
		case DBG_ST_RUNNING:
			DCC_LOG(LOG_TRACE, "running");
			gdb_rsp_puts(gdb, "YARD-ICE: running\n");
			thinkos_flag_set(tcpd->run_flag);
			break;
		default:
			DCC_LOG1(LOG_WARNING, "unknown state: %d", state);
			gdb_rsp_puts(gdb, "YARD-ICE: unknown state, bailing out!\n");
//			return -1;
		}
	}
}

struct gdb_tcpd gdb_tcp_singleton;

int __attribute__((noreturn)) gdb_tcp_task(struct gdb_tcpd * tcpd)
{
	struct tcp_pcb * svc = tcpd->svc;
	struct gdb_rsp * gdb = tcpd->gdb;
	struct tcp_pcb * tp;
	int state;

	INF("<%d>", thinkos_thread_self());

	for (;;) {
		if ((tp = tcp_accept(svc)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept().");
			break;
		}

		gdb_rsp_comm_init(gdb, &gdb_tcp_op, tp);

		INF("GDB: accepted: %08x", (int)tp);

		state = target_status();
		if (state == DBG_ST_RUNNING) {
			INFS("GDB: target running");
			/* wakeup the break wait thread */
			thinkos_flag_set(tcpd->run_flag);
		} else {
			thinkos_flag_clr(tcpd->run_flag);
		} 

		thinkos_flag_set(tcpd->con_flag);

		gdb_rsp_comm_loop(gdb);

		INFS("GDB: close...");

		tcp_close(tp);
		tcpd->tp = NULL;
		thinkos_flag_clr(tcpd->con_flag);
		/* wakeup the brk signaling thread */
		thinkos_flag_set(tcpd->run_flag);
	}

	for (;;);
}

uint32_t gdb_srv_stack[((RSP_BUFFER_LEN * 3) / 8)  + 512];
uint32_t gdb_brk_stack[(RSP_BUFFER_LEN / 3) + 512];

const struct thinkos_thread_inf gdb_srv_inf = {
	.stack_ptr = gdb_srv_stack, 
	.stack_size = sizeof(gdb_srv_stack),
	.priority = 32,
	.thread_id = 32,
	.paused = false,
	.tag = "GDB_SRV"
};

const struct thinkos_thread_inf gdb_brk_inf = {
	.stack_ptr = gdb_brk_stack, 
	.stack_size = sizeof(gdb_brk_stack),
	.priority = 32,
	.thread_id = 32,
	.paused = false,
	.tag = "GDB_BRK"
};

struct gdb_tcpd gdb_tcp_singleton;

extern const struct gdb_target_op gdb_target_arm_jtag_op;

int gdb_rspd_start(void)
{  
	struct gdb_tcpd * tcpd = &gdb_tcp_singleton;
	struct gdb_rsp * gdb = &gdb_rsp_singleton;
	struct tcp_pcb * svc;
	int th;

	svc = tcp_alloc();

	tcp_bind(svc, INADDR_ANY, htons(1000));

	if (tcp_listen(svc, 1) != 0) {
		WARNS("GDB: Can't register the TCP listner!");
		return -1;
	}

	gdb_rsp_target_init(gdb, &gdb_target_arm_jtag_op, NULL);

	tcpd->gdb = gdb;
	tcpd->svc = svc;
	tcpd->tp = NULL;
	tcpd->run_flag = thinkos_flag_alloc();
	tcpd->con_flag = thinkos_flag_alloc();

	th = thinkos_thread_create_inf((void *)gdb_tcp_task, (void *)tcpd, 
								   &gdb_srv_inf);

	INF("GDB server started th=%d", th);

	th = thinkos_thread_create_inf((void *)gdb_brk_task, (void *)tcpd, 
								   &gdb_brk_inf);

	INF("GDB monitor started th=%d", th);

	(void)th;

	return 0;
}

