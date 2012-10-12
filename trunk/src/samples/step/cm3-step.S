/*
 * File:    step.S
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

	.section .vectors, "ax", %progbits
__vectors:
	.global __vectors
	.type __vectors, %object

	.word	_stack
	.word	_reset
	.word	NMI_Handler
	.word	HardFault_Handler
	.word	MemManage_Handler
	.word	BusFault_Handler
	.word	UsageFault_Handler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	SVC_Handler
	.word	DebugMon_Handler
	.word	0
	.word	PendSV_Handler
	.word	SysTick_Handler

	.size   __vectors, . - __vectors

	.align  0
	.thumb

	.section .text.init

	.thumb_func
NMI_Handler:
	.global NMI_Handler
	b		.

	.thumb_func
HardFault_Handler:
	.global HardFault_Handler
	b		.

	.thumb_func
MemManage_Handler:
	.global MemManage_Handler
	b		.

	.thumb_func
BusFault_Handler:
	.global BusFault_Handler
	b		.

	.thumb_func
UsageFault_Handler:
	.global UsageFault_Handler
	b		.

	.thumb_func
SVC_Handler:
	.global SVC_Handler
	b		.

	.thumb_func
DebugMon_Handler:
	.global DebugMon_Handler
	b		.

	.thumb_func
PendSV_Handler:
	.global PendSV_Handler
	b		.

	.thumb_func
SysTick_Handler:
	.global SysTick_Handler
	b		.


	.thumb_func
_reset:
	.global _reset 
	.type   _reset, %function

	mov	    r0, #0
	mov	    r1, #0
	mov	    r2, #0
	mov	    r3, #3
	mov	    r4, #4
	mov	    r5, #5
	mov	    r6, #6
	mov	    r7, #7
.L_loop:
	add     r0, r0, #1	
	add     r0, r0, #1	
	add     r0, r0, #1	
	add     r0, r0, #1	

	add     r0, r0, #1	
	add     r0, r0, #1	
	add     r0, r0, #1	
	add     r0, r0, #1	

	add     r0, r0, #1	
	add     r0, r0, #1	
	add     r0, r0, #1	
	add     r0, r0, #1	

	add     r0, r0, #1	
	add     r0, r0, #1	
	add     r0, r0, #1	
	add     r0, r0, #1	

	mov     r1, r0
	add     r2, r2, #1	
	b       .L_loop

	.size   _reset, . - _reset
