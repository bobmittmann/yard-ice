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

void simrpc_suspend_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	if (cnt != 0) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	simrpc_send_opc(SIMRPC_REPLY_OK(opc));
	__thinkos_pause_all();
}

void simrpc_resume_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	if (cnt != 0) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	simrpc_send_opc(SIMRPC_REPLY_OK(opc));
	__thinkos_resume_all();
}

void simrpc_reboot_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	if (cnt != 0) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	simrpc_send_opc(SIMRPC_REPLY_OK(opc));

	cm3_sysrst();
}

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

/* -------------------------------------------------------------------------
 * Application execution
 * ------------------------------------------------------------------------- */

extern const struct thinkos_thread_inf thinkos_main_inf;

bool thinkos_app_check(uint32_t addr)
{
	uint32_t * app = (uint32_t *)addr;

	if ((app[0] != 0x0a0de004) ||
		(app[1] != 0x6e696854) ||
		(app[2] != 0x00534f6b)) {
		DCC_LOG(LOG_WARNING, "invalid application signature!");
		return false;
	}

	return true;
}

bool thinkos_app_exec(uint32_t addr, bool paused)
{
	uint32_t * app = (uint32_t *)addr;
	int thread_id = 0;

	DCC_LOG1(LOG_TRACE, "app=%p", app);

	__thinkos_thread_init(thread_id, (uintptr_t)&_stack, 
						  (void *)app_bootstrap, (void *)app);

#if THINKOS_ENABLE_THREAD_INFO
	__thinkos_thread_inf_set(thread_id, &thinkos_main_inf);
#endif

	if (!paused) {
		__thinkos_thread_resume(thread_id);
		__thinkos_defer_sched();
	}

	return true;
}

void simlnk_int_enable(void);

void thinkos_soft_reset(void)
{
	DCC_LOG(LOG_TRACE, "1. disable all interrupts"); 
	__thinkos_irq_disable_all();

	DCC_LOG(LOG_TRACE, "2. kill all threads...");
	__thinkos_kill_all(); 

	DCC_LOG(LOG_TRACE, "4. ThinkOS reset...");
	__thinkos_reset();

#if THINKOS_ENABLE_CONSOLE
	DCC_LOG(LOG_TRACE, "5. console reset...");
	__console_reset();
#endif

	DCC_LOG(LOG_TRACE, "6. exception reset...");
	__exception_reset();

#if (THINKOS_ENABLE_DEBUG_STEP)
	DCC_LOG(LOG_TRACE, "7. clear all breakpoints...");
	dmon_breakpoint_clear_all();
#endif
}

void simrpc_exec_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t key;

	if (cnt != 4) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	key = data[0];
	if (key == SIMRPC_EXEC_KEY('A', 'P', 'P', 0)) {
		thinkos_soft_reset();
		simlnk_int_enable();

		if (thinkos_app_check(FLASH_BLK_FIRMWARE_OFFS)) {
			simrpc_send_opc(SIMRPC_REPLY_OK(opc));
			thinkos_app_exec(FLASH_BLK_FIRMWARE_OFFS, false);
			return;
		}
		DCC_LOG(LOG_WARNING, "Invalid application!");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	if (key == SIMRPC_EXEC_KEY('T', 'E', 'S', 'T')) {
		simrpc_send_opc(SIMRPC_REPLY_OK(opc));
		thinkos_soft_reset();
		simlnk_int_enable();
		thinkos_app_exec((uint32_t)board_test, false);
		return;
	}

	DCC_LOG(LOG_WARNING, "Invalid Key");
	simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
}



