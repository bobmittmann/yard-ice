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

int arm_jtag_init(void * arg)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jtag_mem_write(void * arg, uint32_t addr, 
					   const void * ptr, unsigned int len)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}


int arm_jtag_mem_read(void * arg, uint32_t addr, void * ptr, unsigned int len)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jtag_file_read(void * arg, const char * name, char * dst, 
					  unsigned int offs, unsigned int size)
{
	DCC_LOGSTR(LOG_TRACE, "file=%s.", name);
	return 0;
}

int arm_jtag_cpu_halt(void * arg)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jtag_cpu_continue(void * arg)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jtag_cpu_goto(void * arg, uint32_t addr, int opt)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jtag_cpu_run(void * arg, uint32_t addr, int opt)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jtag_cpu_reset(void * arg)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jtag_app_exec(void * arg)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_thread_getnext(void * arg, int thread_id)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_thread_active(void * arg)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_thread_break_id(void * arg)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_thread_any(void * arg)
{
	DCC_LOG(LOG_TRACE, ".");
	return 1;
}

bool arm_jatg_thread_isalive(void * arg, int thread_id)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_thread_register_get(void * arg, int thread_id, 
								 int reg, uint32_t * val)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_thread_register_set(void * arg, unsigned int thread_id, 
								 int reg, uint32_t val)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_thread_goto(void * arg, unsigned int thread_id, uint32_t addr)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_thread_step_req(void * arg, unsigned int thread_id)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_thread_continue(void * arg, unsigned int thread_id)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_thread_info(void * arg, unsigned int gdb_thread_id, char * buf)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_breakpoint_clear_all(void * arg)
{
	struct dbg_bp * bp = NULL;

	DCC_LOG(LOG_TRACE, ".");

	while (target_breakpoint_next(bp, &bp) == 0)
		target_breakpoint_clear(bp->addr, bp->size);

	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_watchpoint_clear_all(void * arg)
{
	struct dbg_wp * wp = NULL;

	DCC_LOG(LOG_TRACE, ".");

	while (target_watchpoint_next(wp, &wp) == 0)
		target_watchpoint_clear(wp->addr, wp->size);

	return 0;
}

int arm_jatg_breakpoint_set(void * arg, uint32_t addr, unsigned int size)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_breakpoint_clear(void * arg, uint32_t addr, unsigned int size)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_watchpoint_set(void * arg, uint32_t addr, 
							unsigned int size, unsigned int opt)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

int arm_jatg_watchpoint_clear(void * arg, uint32_t addr, unsigned int size)
{
	DCC_LOG(LOG_TRACE, ".");
	return 0;
}

const struct gdb_target_op gdb_target_arm_jtag_op = {
	.init = arm_jtag_init,
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

