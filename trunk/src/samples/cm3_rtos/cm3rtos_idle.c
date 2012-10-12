/* 
 * File:	 dac-test.c
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


#include <arch/cortex-m3.h>
#include <stdlib.h>

#include <cm3rtos.h>
#define __CM3RTOS_SYS__
#include <cm3rtos_sys.h>

#ifndef CM3RTOS_ENABLE_WFI
#define CM3RTOS_ENABLE_WFI 1
#endif

void __attribute__((noreturn, naked)) cm3rtos_idle_task(void)
{
	for (;;) {
		asm volatile ("add  lr, lr, #1\n");
#if CM3RTOS_ENABLE_WFI
		asm volatile ("wfi\n"); /* wait for interrupt */
#endif
	}
}

