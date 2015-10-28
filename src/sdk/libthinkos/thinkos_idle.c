/* 
 * thikos_core.c
 *
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
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

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>

/* -------------------------------------------------------------------------- 
 * Idle task
 * --------------------------------------------------------------------------*/

void __attribute__((noreturn, naked)) thinkos_idle_task(void)
{
//	DCC_LOG(LOG_TRACE, "ThinkOS Idle started..."); 

	for (;;) {
#if THINKOS_ENABLE_IDLE_WFI
		asm volatile ("wfi\n"); /* wait for interrupt */
#endif
#if (THINKOS_ENABLE_MONITOR)
		thinkos_dbgmon(DBGMON_SIGNAL_IDLE);
#endif
	}
}

#if THINKOS_ENABLE_CONST_IDLE 
/* Constant IDLE stack:

   Define the IDLE context (stack) in Flash or read only memory. This
   is helpful in two aspects:
   1 - it reduces the memory footprint.
   2 - it won't be unintetionally modified by a misbehaved application.

 */
const struct thinkos_context __attribute__((aligned(8))) thinkos_idle_ctx = {
	.pc = (uint32_t)thinkos_idle_task,
	.xpsr = CM_EPSR_T /* set the thumb bit */
};
#endif

void __thinkos_idle_init(void)
{
	struct thinkos_context * idle_ctx;

	DCC_LOG(LOG_TRACE, "..."); 

#if (THINKOS_ENABLE_CONST_IDLE) 
	idle_ctx = (struct thinkos_context *)&thinkos_idle_ctx;
#else
	/* initialize the idle thread */
//	idle_ctx = (struct thinkos_context *)&thinkos_idle_stack[-CTX_R0];
	idle_ctx = (struct thinkos_context *)&thinkos_idle_stack[0];
#if DEBUG
	idle_ctx->r0 = 0,
	idle_ctx->r1 = 0x01010101,
	idle_ctx->r2 = 0x02020202,
	idle_ctx->r3 = 0x03030303,
	idle_ctx->r12 = 0x12121212,
	idle_ctx->lr = 0x14141414,
#endif
	idle_ctx->pc = (uint32_t)thinkos_idle_task,
	idle_ctx->xpsr = CM_EPSR_T; /* set the thumb bit */

#endif
	thinkos_rt.idle_ctx = idle_ctx;
#if (THINKOS_THREADS_MAX < 32) 
	/* put the IDLE thread in the ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, THINKOS_THREADS_MAX, 1);
#endif
}

