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

static void __attribute__((noreturn)) app_bootstrap(void * arg)
{
	int (* app_reset)(int argc, char ** argv);
	uintptr_t thumb_call = (uintptr_t)arg | 1;

	app_reset = (void *)thumb_call;
	for (;;) {
		DCC_LOG(LOG_TRACE, "app_reset()");
		app_reset(1, (char **)app_argv);
	}
}

extern uint32_t _stack;
const uint32_t thinkos_app_blk_addr = 0x08010000;
const uint32_t thinkos_app_blk_size = (64 + 128) * 1024;

/* -------------------------------------------------------------------------
 * Application execution
 * ------------------------------------------------------------------------- */

int dmon_app_exec(bool paused)
{
	uint32_t * app = (uint32_t *)thinkos_app_blk_addr;
	int thread_id = 0;

	DCC_LOG1(LOG_TRACE, "app=%p", app);

	if ((app[0] != 0x0a0de004) ||
		(app[1] != 0x6e696854) ||
		(app[2] != 0x00534f6b)) {
		DCC_LOG(LOG_WARNING, "invalid application signature!");
		return -1;
	}

	__thinkos_thread_init(thread_id, (uintptr_t)&_stack, 
						  (void *)app_bootstrap, (void *)app);

	if (!paused) {
		__thinkos_thread_resume(thread_id);
		__thinkos_defer_sched();
	}

	return 0;
}

bool dmon_app_suspend(void)
{
	__thinkos_pause_all();

	if (thinkos_rt.active == THINKOS_THREADS_MAX) {
		DCC_LOG(LOG_TRACE, "Current is Idle!");
	} else {
		DCC_LOG1(LOG_TRACE, "current_thread=%d", thinkos_rt.active);
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
	DCC_LOG(LOG_TRACE, "....");

	__thinkos_resume_all();

	return true;
}

