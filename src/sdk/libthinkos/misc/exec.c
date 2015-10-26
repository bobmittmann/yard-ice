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

#define __THINKOS_SYS__
#include <thinkos_sys.h>

/* -------------------------------------------------------------------------
 * Application execution
 * ------------------------------------------------------------------------- */

extern uint32_t _stack;
extern const struct thinkos_thread_inf thinkos_main_inf;


void __thinkos_exec(int thread_id, void (* func)(void *), 
					void * arg, bool paused)
{
	DCC_LOG(LOG_TRACE, "__thinkos_thread_abort()");
	__thinkos_thread_abort(thread_id);

	DCC_LOG2(LOG_TRACE, "__thinkos_thread_init(func=%p arg=%p)", func, arg);
	__thinkos_thread_init(thread_id, (uintptr_t)&_stack, func, arg);

#if THINKOS_ENABLE_THREAD_INFO
	DCC_LOG(LOG_TRACE, "__thinkos_thread_inf_set()");
	__thinkos_thread_inf_set(thread_id, &thinkos_main_inf);
#endif

	if (!paused) {
		DCC_LOG(LOG_TRACE, "__thinkos_thread_resume()");
		__thinkos_thread_resume(thread_id);
		__thinkos_defer_sched();
	}
}

