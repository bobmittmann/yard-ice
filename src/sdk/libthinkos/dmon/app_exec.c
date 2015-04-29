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

const char * const app_argv[] = {
	"thinkos_app"
};

void __attribute__((noreturn)) app_task(void * arg)
{
	int (* app_main)(int argc, char ** argv) = (void *)arg;

	for (;;) {
		app_main(1, (char **)app_argv);
	}
}

extern uint32_t _stack;

int dmon_app_exec(void)
{
	uint32_t * app_main = (uint32_t *)0x08010000;
	int i;

	DCC_LOG1(LOG_TRACE, "app_main=%p", app_main);

	for (i = 0; i < 32; ++i) {
		if (app_main[i] == 0xffffffff) {
			DCC_LOG(LOG_WARNING, "invalid application!");
			return -1;
		}
	}

	__thinkos_thread_exec(0, (uintptr_t)&_stack, 
						  (void *)app_task, (void *)app_main);

	return 0;
}
