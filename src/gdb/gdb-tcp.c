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

int gdb_tcp_send(struct tcp_pcb * tp, const void * buf, unsigned int len)
{
	return tcp_send(tp, buf, len, TCP_SEND_NOWAIT);
}

int gdb_tcp_recv(struct tcp_pcb * tp, void * buf, unsigned int len)
{
	return tcp_recv(tp, buf, len);
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
	.send = (void *)gdb_tcp_send,
	.recv = (void *)gdb_tcp_recv,
	.flush = (void *)gdb_tcp_flush,
	.drain = (void *)gdb_tcp_drain,
	.settmo = (void *)gdb_tcp_settmo
};

int arm_jtag_mem_write(void * arg, uint32_t addr, 
					   const void * ptr, unsigned int len)
{
	return 0;
}


int arm_jtag_mem_read(void * arg, uint32_t addr, void * ptr, unsigned int len)
{
	return 0;
}

int arm_jtag_file_read(void * arg, const char * name, char * dst, 
					  unsigned int offs, unsigned int size)
{
	return 0;
}

int arm_jtag_cpu_halt(void * arg)
{
	return 0;
}

int arm_jtag_cpu_continue(void * arg)
{
	return 0;
}

int arm_jtag_cpu_goto(void * arg, uint32_t addr, int opt)
{
	return 0;
}

int arm_jtag_cpu_run(void * arg, uint32_t addr, int opt)
{
	return 0;
}

int arm_jtag_cpu_reset(void * arg)
{
	return 0;
}

int arm_jtag_app_exec(void * arg)
{
	return 0;
}

int arm_jatg_thread_getnext(void * arg, int thread_id)
{
	return 0;
}

int arm_jatg_thread_active(void * arg)
{
	return 0;
}

int arm_jatg_thread_break_id(void * arg)
{
	return 0;
}

int arm_jatg_thread_any(void * arg)
{
	return 0;
}

bool arm_jatg_thread_isalive(void * arg, int thread_id)
{
	return 0;
}

int arm_jatg_thread_register_get(void * arg, int thread_id, 
								 int reg, uint32_t * val)
{
	return 0;
}

int arm_jatg_thread_register_set(void * arg, unsigned int thread_id, 
								 int reg, uint32_t val)
{
	return 0;
}

int arm_jatg_thread_goto(void * arg, unsigned int thread_id, uint32_t addr)
{
	return 0;
}

int arm_jatg_thread_step_req(void * arg, unsigned int thread_id)
{
	return 0;
}

int arm_jatg_thread_continue(void * arg, unsigned int thread_id)
{
	return 0;
}

int arm_jatg_thread_info(void * arg, unsigned int gdb_thread_id, char * buf)
{
	return 0;
}

int arm_jatg_breakpoint_clear_all(void * arg)
{
	return 0;
}

int arm_jatg_watchpoint_clear_all(void * arg)
{
	return 0;
}

int arm_jatg_breakpoint_set(void * arg, uint32_t addr, unsigned int size)
{
	return 0;
}

int arm_jatg_breakpoint_clear(void * arg, uint32_t addr, unsigned int size)
{
	return 0;
}

int arm_jatg_watchpoint_set(void * arg, uint32_t addr, unsigned int size, unsigned int opt)
{
	return 0;
}

int arm_jatg_watchpoint_clear(void * arg, uint32_t addr, unsigned int size)
{
	return 0;
}

const struct gdb_target_op gdb_target_arm_jtag_op = {
	.mem_write = arm_jtag_mem_write,
	.mem_read = arm_jtag_mem_read,
	.file_read = arm_jtag_file_read,
	.cpu_halt = arm_jtag_cpu_halt,
	.cpu_continue = arm_jtag_cpu_continue,
	.cpu_goto = arm_jtag_cpu_goto,
	.cpu_run = arm_jtag_cpu_run,
	.cpu_reset = arm_jtag_cpu_reset,
	.app_exec = arm_jtag_app_exec,
	.thread_getnext = arm_jatg_thread_getnext,
	.thread_active = arm_jatg_thread_active,
	.thread_break_id = arm_jatg_thread_break_id,
	.thread_any = arm_jatg_thread_any,
	.thread_isalive = arm_jatg_thread_isalive,
	.thread_register_get = arm_jatg_thread_register_get,
	.thread_register_set = arm_jatg_thread_register_set,
	.thread_goto = arm_jatg_thread_goto,
	.thread_step_req = arm_jatg_thread_step_req,
	.thread_continue = arm_jatg_thread_continue,
	.thread_info = arm_jatg_thread_info,
	.breakpoint_clear_all = arm_jatg_breakpoint_clear_all,
	.watchpoint_clear_all = arm_jatg_watchpoint_clear_all,
	.breakpoint_set = arm_jatg_breakpoint_set,
	.breakpoint_clear = arm_jatg_breakpoint_clear,
	.watchpoint_set = arm_jatg_watchpoint_set,
	.watchpoint_clear = arm_jatg_watchpoint_clear
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
		DCC_LOG(LOG_TRACE, "waiting connect...");
		thinkos_flag_take(tcpd->con_flag);

		/* wait for a 'target run' indication */
		DCC_LOG(LOG_TRACE, "waiting run...");

		/* wait for a target run */
		thinkos_flag_take(tcpd->run_flag);

		DCC_LOG(LOG_TRACE, "waiting halt...");

		while ((state = target_halt_wait(5000)) == ERR_TIMEOUT) {
			DCC_LOG(LOG_TRACE, "waiting...");
		}

		if (state == DBG_ST_HALTED) {
			DCC_LOG(LOG_TRACE, "halted");

			thinkos_flag_clr(tcpd->run_flag);

			gdb_rsp_on_halt(gdb, sig);
		}

		state = target_status();
		if (state < DBG_ST_CONNECTED) {
			DCC_LOG(LOG_WARNING, "target not connected!");
			gdb_rsp_error(gdb, state);
			continue;
		}

		if (state != DBG_ST_HALTED) {
			DCC_LOG(LOG_TRACE, "running");

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

	DCC_LOG1(LOG_TRACE, "<%d>", thinkos_thread_self());
	INF("<%d>", thinkos_thread_self());

	for (;;) {
		if ((tp = tcp_accept(svc)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept().");
			break;
		}

		gdb_rsp_comm_init(gdb, &gdb_tcp_op, tp);

		INF("accepted: %08x", (int)tp);
		DCC_LOG(LOG_TRACE, "accepted.");

		state = target_status();
		if (state == DBG_ST_RUNNING) {
			DCC_LOG(LOG_TRACE, "running");
			/* wakeup the break wait thread */
			thinkos_flag_set(tcpd->run_flag);
		} else {
			thinkos_flag_clr(tcpd->run_flag);
		} 

		thinkos_flag_set(tcpd->con_flag);

		gdb_rsp_comm_loop(gdb);

		DCC_LOG(LOG_TRACE, "close...");

		tcp_close(tp);
		tcpd->tp = NULL;
		thinkos_flag_clr(tcpd->con_flag);
		/* wakeup the brk signaling thread */
		thinkos_flag_set(tcpd->run_flag);
	}

	for (;;);
}

uint32_t gdb_srv_stack[((RSP_BUFFER_LEN * 3) / 8)  + 128];
uint32_t gdb_brk_stack[(RSP_BUFFER_LEN / 3) + 128];

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

int gdb_rspd_start(void)
{  
	struct gdb_tcpd * tcpd = &gdb_tcp_singleton;
	struct gdb_rsp * gdb = &gdb_rsp_singleton;
	struct tcp_pcb * svc;
	int th;

	svc = tcp_alloc();

	tcp_bind(svc, INADDR_ANY, htons(1000));

	if (tcp_listen(svc, 1) != 0) {
		WARN("Can't register the TCP listner!");
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

