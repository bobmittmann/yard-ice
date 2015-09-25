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

void simrpc_suspend_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	if (cnt != 0) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
	__thinkos_pause_all();
}

void simrpc_resume_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	if (cnt != 0) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
	__thinkos_resume_all();
}

void simrpc_reboot_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	if (cnt != 0) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	__simrpc_send_opc(SIMRPC_REPLY_OK(opc));

	cm3_sysrst();
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

	thinkos_soft_reset();
	simlnk_int_enable();

	key = data[0];
	if (key == SIMRPC_EXEC_KEY('A', 'P', 'P', 0)) {
		if (board_app_exec(THINKOS_APP_ADDR)) {
			__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
		} else {
			DCC_LOG(LOG_WARNING, "Invalid application!");
			__simrpc_send_int(SIMRPC_REPLY_ERR(opc), SIMRPC_EINVAL);
		}
		return;
	}

#if 0
	if (key == SIMRPC_EXEC_KEY('T', 'E', 'S', 'T')) {
		__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
		thinkos_app_exec((uint32_t)board_test, false);
		return;
	}
#endif

	DCC_LOG(LOG_WARNING, "Invalid Key");
	__simrpc_send_int(SIMRPC_REPLY_ERR(opc), SIMRPC_EINVAL);
}

void simrpc_kernelinfo_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct kernelinfo inf;

	DCC_LOG(LOG_TRACE, "...");

	inf.ticks = __thinkos_ticks();
	inf.version.major = VERSION_MAJOR;
	inf.version.minor = VERSION_MINOR;
	inf.version.build = VERSION_BUILD;
	inf.version.timestamp = VERSION_TIMESTAMP;

	__simrpc_send(SIMRPC_REPLY_OK(hdr), &inf, sizeof(inf));
}

