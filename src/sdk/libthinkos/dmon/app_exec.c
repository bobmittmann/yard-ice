/* 
 * File:	 usb-cdc.c
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

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>
#include <sys/dcclog.h>

static const char * const app_argv[] = {
	"thinkos_app"
};

static void __attribute__((naked, noreturn)) app_bootstrap(void * arg)
{
	int (* app_reset)(int argc, char ** argv);
	uintptr_t thumb_call = (uintptr_t)arg | 1;

	DCC_LOG1(LOG_TRACE, "sp=0x%08x", cm3_sp_get());

	app_reset = (void *)thumb_call;
	for (;;) {
		app_reset(1, (char **)app_argv);
	}
}

/* -------------------------------------------------------------------------
 * Application execution
 * ------------------------------------------------------------------------- */

bool dmon_app_exec(uint32_t addr, bool paused)
{
	uint32_t * app = (uint32_t *)addr;
	int thread_id = 0;

	if ((app[0] != 0x0a0de004) ||
		(app[1] != 0x6e696854) ||
		(app[2] != 0x00534f6b)) {
		DCC_LOG1(LOG_WARNING, "invalid signature at %p!", app);
		return false;
	}

	DCC_LOG2(LOG_TRACE, "app=%p paused=%s", app,
			 paused ? "true" : "false");

	__thinkos_exec(thread_id, (void *)app_bootstrap, (void *)app, paused);

	return true;
}


static void __dmon_irq_disable_all(void)
{
	int i;

	for (i = 0; i < NVIC_IRQ_REGS; ++i) {
		/* save interrupt state */
		thinkos_dmon_rt.nvic_ie[i] = CM3_NVIC->iser[i];
		DCC_LOG2(LOG_TRACE, "nvic_ie[%d]=0x%08x", 
				i, thinkos_dmon_rt.nvic_ie[i]);
		CM3_NVIC->icer[i] = 0xffffffff; /* disable all interrupts */
	}
	this_board.comm_irqen();
}

static void __dmon_irq_restore_all(void)
{
	int i;

	for (i = 0; i < NVIC_IRQ_REGS; ++i) {
		/* restore interrupt state */
		CM3_NVIC->iser[i] = thinkos_dmon_rt.nvic_ie[i];
	}
}

bool dmon_app_suspend(void)
{
	__dmon_irq_disable_all();

	__thinkos_pause_all();

	if (thinkos_rt.active == THINKOS_THREADS_MAX) {
		DCC_LOG(LOG_INFO, "Current is Idle!");
	} else {
		DCC_LOG1(LOG_INFO, "current_thread=%d", thinkos_rt.active);
	}

#if (THINKOS_ENABLE_DEBUG_STEP)
	/* clear stepping requests */
	thinkos_rt.step_req = 0;
#endif

	dmon_wait_idle();

	return true;
}

bool dmon_app_continue(void)
{
	DCC_LOG(LOG_INFO, "....");

	__thinkos_resume_all();

	__dmon_irq_restore_all();

	return true;
}

