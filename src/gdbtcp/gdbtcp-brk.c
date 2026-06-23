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

int __attribute__((noreturn)) gdbtcp_brk_task(struct gdbtcpd * gdb)
{
	struct tcp_pcb * tp;
	int state;

	DCC_LOG1(LOG_TRACE, "<%d>", thinkos_thread_self());
	INF("BRK: <%d>", thinkos_thread_self());

	thinkos_mutex_lock(gdb->mutex);
	for (;;) {
//		while (!(gdb->connected))  {
		while (!(gdb->connected && gdb->running))  {
			WARNS("waiting for connect...");
			/* wait for a connection */
			thinkos_cond_wait(gdb->cond, gdb->mutex);
		}

		/* wait for a 'target run' indication */
		WARNS("waiting for halt...");

		state = target_status();

		thinkos_mutex_unlock(gdb->mutex);
		while (gdb->running && (state != DBG_ST_HALTED)) {
			WARNS("halt wait...");
			state = target_halt_wait(5000);
			switch (state) {
			case DBG_ST_ERROR:
				WARNS("GDB: error state");
				break;
			case DBG_ST_OUTOFSYNC:
				WARNS("GDB: Out of sync");
				break;
			case DBG_ST_BUSY:
				WARNS("GDB: busy ...");
				break;
			case DBG_ST_UNDEF:
				WARNS("GDB: undefined state");
				break;
			case DBG_ST_UNCONNECTED:
				WARNS("GDB: unconnected ?");
				break;
			case DBG_ST_CONNECTED:
				WARNS("GDB: connected (busy)");
				break;
			case DBG_ST_RUNNING:
				WARNS("GDB: running");
				break;
			case DBG_ST_LOCKUP:
				WARNS("GDB: lockup");
				break;
			case DBG_ST_HALTED:
				WARNS("GDB: halted");
				break;
			case ERR_TIMEOUT:
				WARNS("GDB: timeout");
				break;
			default:
				WARN("GDB: unknown state %d", state);
			}
		} 
		thinkos_mutex_lock(gdb->mutex);

		if ((gdb->running) && (state == DBG_ST_HALTED)) {
			gdb->running = false;

			if ((tp = gdb->tp) != NULL) {
				WARNS("Core halted sending break signal...");
				rsp_send_stop_core(tp, TARGET_SIGNAL_TRAP, 1);
//				rsp_send_stop_thread(tp, TARGET_SIGNAL_INT, 1);
			}
		}
	}
}


uint32_t __attribute__((aligned(8))) gdbtcp_brk_stack[(RSP_BUFFER_LEN / 3) + 128];

const struct thinkos_thread_inf gdb_brk_inf = {
	.stack_ptr = gdbtcp_brk_stack, 
	.stack_size = sizeof(gdbtcp_brk_stack),
	.priority = 31,
	.thread_id = 31,
	.paused = false,
	.tag = "GDB_BRK"
};

int gdb_brk_start(struct gdbtcpd * gdb)
{  
	int th;

	th = thinkos_thread_create_inf((void *)gdbtcp_brk_task, (void *)gdb, 
								   &gdb_brk_inf);

	INF("GDB monitor started th=%d", th);

	(void)th;

	return 0;
}

