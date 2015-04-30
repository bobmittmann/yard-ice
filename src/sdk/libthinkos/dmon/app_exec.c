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
	int (* app_main)(int argc, char ** argv) = (void *)arg;

	for (;;) {
		app_main(1, (char **)app_argv);
	}
}

extern uint32_t _stack;

int dmon_app_exec(void)
{
	uint32_t * app_main = (uint32_t *)0x08010000;

	DCC_LOG1(LOG_TRACE, "app_main=%p", app_main);

	if ((app_main[0] != 0x0a0de004) ||
		(app_main[1] != 0x6e696854) ||
		(app_main[2] != 0x00534f6b)) {
		DCC_LOG(LOG_WARNING, "invalid application signature!");
		return -1;
	}

	__thinkos_thread_exec(0, (uintptr_t)&_stack, 
						  (void *)app_bootstrap, (void *)app_main);

	return 0;
}

void dmon_irq_disable_all(void)
{
	int irq;

	/* adjust IRQ priorities to regular (above SysTick and bellow SVC) */
	for (irq = 0; irq < THINKOS_IRQ_MAX; irq++) {
		cm3_irq_disable(irq);
		thinkos_rt.irq_th[irq] = -1;
	}
}

