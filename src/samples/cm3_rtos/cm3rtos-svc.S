/*
 * File:    cm3_rtos-svc.S
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
 * Target:  jtagtool3
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

	.syntax unified
	.cpu cortex-m3

	.text
	.thumb

	.balign 8
	.thumb_func
	.global __cm3_pendsv_isr
	.type   cm3_pendsv_isr, %function
__cm3_pendsv_isr:
	mrs     r1, PSP /* get the process stack pointer */
	/* store the context */
    stmdb	r1!, {r3-r11, lr}

	movw    r4, #:lower16:cm3rtos_rt
	movt    r4, #:upper16:cm3rtos_rt

	ldr     r0, [r4]
	/* store the stack pointer */
	str     r1, [r0]

	/* call the scheduler */
	bl      cm3rtos_scheduler

	/* set the active thread */
    str	    r0, [r4]
	/* get thread's stack pointer */
	ldr     r1, [r0]

	/* restore the process stack pointer */
	add     r2, r1, (10 * 4)
    msr		PSP, r2
	/* restore context */
    ldmia	r1, {r3-r11, pc}
	.size   cm3_pendsv_isr, . - cm3_pendsv_isr

	.end



