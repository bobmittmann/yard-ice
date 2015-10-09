/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>

#include <sys/param.h>

#include "board.h"
#include "simlnk.h"
#include "simrpc.h"
#include "simrpc_svc.h"
#include "version.h"

int __simrpc_send(uint32_t opc, void * data, unsigned int cnt);
int __simrpc_send_int(uint32_t opc, int val);
int __simrpc_send_opc(uint32_t opc);

#define NVIC_IRQ_REGS ((THINKOS_IRQ_MAX + 31) / 32)

uint32_t nvic_ie[NVIC_IRQ_REGS]; /* interrupt state */
uint8_t active;

static void __irq_disable_all(void)
{
	int i;

	/* disbale systick interrupt */
	CM3_SYSTICK->csr &= ~SYSTICK_CSR_TICKINT;
	for (i = 0; i < NVIC_IRQ_REGS; ++i) {
		/* save interrupt state */
		nvic_ie[i] = CM3_NVIC->iser[i];
		CM3_NVIC->icer[i] = 0xffffffff; /* disable all interrupts */
	}
	board_comm_irqen();
}

static void __irq_restore_all(void)
{
	int i;

	for (i = 0; i < NVIC_IRQ_REGS; ++i) {
		/* restore interrupt state */
		CM3_NVIC->iser[i] = nvic_ie[i];
	}
	/* enable systick interrupt */
	CM3_SYSTICK->csr |= SYSTICK_CSR_TICKINT;
}


void simrpc_suspend_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	__irq_disable_all();

	active = thinkos_rt.active;
	if ((uint32_t)thinkos_rt.active < THINKOS_THREADS_MAX) {
		__thinkos_thread_pause(thinkos_rt.active);
		__thinkos_defer_sched();
	}

	__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
}

void simrpc_resume_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	if ((uint32_t)active < THINKOS_THREADS_MAX)
		__thinkos_thread_resume(active);

	__thinkos_defer_sched();

	__irq_restore_all();

	__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
}

void simrpc_reboot_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	if (cnt != 0) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	__simrpc_send_opc(SIMRPC_REPLY_OK(opc));

	board_soft_reset();

	board_exec(board_reboot, 0);
}

void simlnk_int_enable(void);
void app_default(int);

void simrpc_exec_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t key;
	int mode;

	if (cnt != 4) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	board_soft_reset();

	key = data[0];
	if (key == SIMRPC_EXEC_KEY('A', 'P', 'P', 0)) {
		mode = APP_MODE_NORMAL;
	} else if (key == SIMRPC_EXEC_KEY('S', 'A', 'F', 'E')) {
		mode = APP_MODE_SAFE;
	} else {
		DCC_LOG(LOG_WARNING, "Invalid Key");
		board_exec(app_default, APP_MODE_SAFE);
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), SIMRPC_EINVAL);
		return;
	}

	if (board_app_exec(THINKOS_APP_ADDR, mode)) {
		__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
	} else {
		DCC_LOG(LOG_WARNING, "Invalid application!");
		board_exec(app_default, mode);
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), SIMRPC_EINVAL);
	}
}

void simrpc_kernelinfo_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct simrpc_kernelinfo inf;

	DCC_LOG(LOG_TRACE, "...");

	inf.ticks = __thinkos_ticks();
	inf.version.major = VERSION_MAJOR;
	inf.version.minor = VERSION_MINOR;
	inf.version.build = VERSION_BUILD;
	inf.version.timestamp = VERSION_TIMESTAMP;

	__simrpc_send(SIMRPC_REPLY_OK(hdr), &inf, sizeof(inf));
}

#define __THINKOS_SYS__
#include <thinkos_sys.h>

void simrpc_exceptinfo_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	struct thinkos_except * xcpt;
	struct simrpc_exceptinfo inf;

	xcpt = __thinkos_except_buf();

	inf.ctx.r[0] = xcpt->ctx.r0;
	inf.ctx.r[1] = xcpt->ctx.r1;
	inf.ctx.r[2] = xcpt->ctx.r2;
	inf.ctx.r[3] = xcpt->ctx.r3;
	inf.ctx.r[4] = xcpt->ctx.r4;
	inf.ctx.r[5] = xcpt->ctx.r5;
	inf.ctx.r[6] = xcpt->ctx.r6;
	inf.ctx.r[7] = xcpt->ctx.r7;
	inf.ctx.r[8] = xcpt->ctx.r8;
	inf.ctx.r[9] = xcpt->ctx.r9;
	inf.ctx.r[10] = xcpt->ctx.r10;
	inf.ctx.r[11] = xcpt->ctx.r11;
	inf.ctx.r[12] = xcpt->ctx.r12;
	inf.ctx.sp = xcpt->psp;
	inf.ctx.lr = xcpt->ctx.lr;
	inf.ctx.pc = xcpt->ctx.pc;
	inf.ctx.xpsr = xcpt->ctx.xpsr;
	inf.psp = xcpt->psp;
	inf.msp = xcpt->msp;
	inf.ret = xcpt->ret;
	inf.icsr = xcpt->icsr;

	__simrpc_send(SIMRPC_REPLY_OK(opc), &inf, sizeof(struct simrpc_exceptinfo));
}

void simrpc_threadinfo_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	int32_t thread_id;
	struct thinkos_context * ctx;
	struct simrpc_threadinfo inf;

	if (cnt != 4) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	}

	thread_id = (int32_t)data[0];

	if (thread_id > THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "Invalid thread: %d!", thread_id);
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -1);
		return;
	}

	if (thread_id == THINKOS_THREAD_IDLE)
		ctx = thinkos_rt.idle_ctx;
	else
		ctx = thinkos_rt.ctx[thread_id];

	if (ctx == NULL){
		DCC_LOG(LOG_ERROR, "Invalid context!");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	inf.ctx.r[0] = ctx->r0;
	inf.ctx.r[1] = ctx->r1;
	inf.ctx.r[2] = ctx->r2;
	inf.ctx.r[3] = ctx->r3;
	inf.ctx.r[4] = ctx->r4;
	inf.ctx.r[5] = ctx->r5;
	inf.ctx.r[6] = ctx->r6;
	inf.ctx.r[7] = ctx->r7;
	inf.ctx.r[8] = ctx->r8;
	inf.ctx.r[9] = ctx->r9;
	inf.ctx.r[10] = ctx->r10;
	inf.ctx.r[11] = ctx->r11;
	inf.ctx.r[12] = ctx->r12;
	inf.ctx.sp = (uint32_t)ctx;
	inf.ctx.lr = ctx->lr;
	inf.ctx.pc = ctx->pc;
	inf.ctx.xpsr = ctx->xpsr;

	__simrpc_send(SIMRPC_REPLY_OK(opc), &inf, sizeof(struct simrpc_threadinfo));
}

