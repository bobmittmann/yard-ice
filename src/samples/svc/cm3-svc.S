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

	.text

	.align  0
	.thumb_func
NMI_Handler:
	.global NMI_Handler
	.type   NMI_Handler, %function
	nop
	b		NMI_Handler
	.size   NMI_Handler, . - NMI_Handler

	.align  0
	.thumb_func
HardFault_Handler:
	.global HardFault_Handler
	.type   HardFault_Handler, %function
	nop
	b		HardFault_Handler
	.size   HardFault_Handler, . - HardFault_Handler

	.align  0
	.thumb_func
MemManage_Handler:
	.global MemManage_Handler
	.type   MemManage_Handler, %function
	nop
	b		MemManage_Handler
	.size   MemManage_Handler, . - MemManage_Handler

	.align  0
	.thumb_func
BusFault_Handler:
	.global BusFault_Handler
	.type   BusFault_Handler, %function
	nop
	b		BusFault_Handler
	.size   BusFault_Handler, . - BusFault_Handler

	.align  0
	.thumb_func
UsageFault_Handler:
	.global UsageFault_Handler
	.type   UsageFault_Handler, %function
	mov     r0, #5
	b		UsageFault_Handler
	.size   UsageFault_Handler, . - UsageFault_Handler


	.align	0
	.global cm3_pendsv_isr
	.thumb
	.thumb_func
	.type   cm3_pendsv_isr, %function
cm3_pendsv_isr:
	mrs     r0, PSP /* get the process stack pointer */
	/* store the context */
    stmdb	r0!, {r3-r11, lr}
	/* call the scheduler */
	bl      cxos_scheduler
	/* restore the process stack pointer */
	add     r2, r0, (9 * 4)
    msr		PSP, r2
	/* restore context */
    ldmia	r0, {r3-r11, pc}
	.size   cm3_pendsv_isr, . - cm3_pendsv_isr

	.align  0
	.thumb_func
cm3_svc_init:
	.global cm3_svc_init
	.type   cm3_svc_init, %function
	mrs     r1, MSP
	msr     MSP, r0
	msr     PSP, r1
	mov     r0, 0x2
	msr     CONTROL, r0
	bx      lr
	.size   cm3_svc_init, . - cm3_svc_init

	.balign 8
	.thumb_func
cm3_svc_handler:
	.global cm3_svc_handler
	.type   cm3_svc_handler, %function

	/* save scratch registers and EXC_RETURN (lr) */
	mrs     r1, PSP /* get a pointer to the process' stack */
	ldr     r0, [r1, #24] /* get PC value */
	ldrb    r0, [r0, #-2] /* get the immediate data from instruction */
	b       svc_handler
//	movw    r2, #:lower16:service_call_tab
//	movt    r2, #:upper16:service_call_tab
//	ldr     pc, [r2, r1, lsl #2]

service_call_tab:
	.word   svc_0
	.word   svc_1
	.word   svc_2
	.word   svc_3

	.size   cm3_svc_handler, . - cm3_svc_handler


	.align  0
	.thumb_func
svc_0:
	.global svc_0
	.type   svc_0, %function
	mov     r1, #0
	str     r1, [r0]
	bx      lr
	.size   svc_0, . - svc_0

	.align  0
	.thumb_func
svc_1:
	.global svc_1
	.type   svc_1, %function
	mov     r1, #1
	str     r1, [r0]
	bx      lr
	.size   svc_1, . - svc_1

	.align  0
	.thumb_func
svc_2:
	.global svc_2
	.type   svc_2, %function
	mov     r0, #2
	bx      lr
	.size   svc_2, . - svc_2

	.align  0
	.thumb_func
svc_3:
	.global svc_3
	.type   svc_3, %function
	mov     r0, #3
	bx      lr
	.size   svc_2, . - svc_3

	.align  0
	.thumb_func
__cm3_svc_handler:

	/* save scratch registers and EXC_RETURN (lr) */
	push    {r6, r7, lr}

	tst     lr, #04 /* Test EXC_RETURN bit 2 */
	ite     eq 
	addeq   r7, sp, #12
	mrsne   r7, PSP /* get a pointer to the process stack */

	bl      .L_svc_call

	/* save the return value on the process stack,
	 this value will ended back to R0 on the exception's return */
	str     r0, [r7]
	/* restore registers and return */
	pop		{r6, r7, pc}


.L_svc_call:
	ldr     r6, [r7, #24] /* get PC value */
	ldrb    r6, [r6, #-2] /* get the immediate data from instruction */

	tbb.w   [pc, r6]
.L_table:
	.byte   (svc_0 - .L_table) / 2
	.byte   (svc_1 - .L_table) / 2
	.byte   (svc_2 - .L_table) / 2
	.byte   (svc_3 - .L_table) / 2


	.align  0
	.thumb_func
___cm3_svc_handler:
	/* save scratch registers and EXC_RETURN (lr) */
	push    {r6, r7, lr}
	mrs     r7, PSP /* get a pointer to the process stack */
	bl      1f
	/* save the return value on the process stack,
	 this value will ended back to R0 on the exception's return */
	str     r0, [r7]
	/* restore registers and return */
	pop		{r6, r7, pc}
1:
	ldr     r6, [r7, #24] /* get PC value */
	ldrb    r6, [r6, #-2] /* get the immediate data from instruction */
	tbb.w   [pc, r6]
	.byte   (svc_0 - .L_table) / 2
	.byte   (svc_1 - .L_table) / 2
	.byte   (svc_2 - .L_table) / 2
	.byte   (svc_3 - .L_table) / 2

	.balign 8
	.thumb_func
_cm3_svc_handler:
	/* save scratch registers and EXC_RETURN (lr) */
	mrs     r0, PSP /* get a pointer to the process' stack */
	ldr     r1, [r0, #24] /* get PC value */
	ldrb    r1, [r1, #-2] /* get the immediate data from instruction */
	movw    r2, #:lower16:service_call_tab
	movt    r2, #:upper16:service_call_tab
	ldr     pc, [r2, r1, lsl #2]
	
	.align  0
	.thumb_func
____cm3_svc_handler:
	/* save scratch registers and EXC_RETURN (lr) */
	push    {r6, r7, lr}
	mrs     r7, PSP /* get a pointer to the process stack */
	movw    r12, #:lower16:service_call_tab
	movt    r12, #:upper16:service_call_tab
	ldr     r6, [r7, #24] /* get PC value */
	ldrb    r6, [r6, #-2] /* get the immediate data from instruction */
	ldr     r12, [r12, r6, lsl #2]
	blx     r12
	/* save the return value on the process stack,
	 this value will ended back to R0 on the exception's return */
	str     r0, [r7]
	/* restore registers and return */
	pop		{r6, r7, pc}

