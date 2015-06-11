/* 
 * thikos_svc.c
 *
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

_Pragma ("GCC optimize (\"Ofast\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <thinkos.h>

#if THINKOS_ENABLE_DEBUG_STEP
void __attribute__((naked)) __thinkos_debug_step_i(unsigned int thread_id) 
{
	unsigned int xpsr;
	unsigned int ipsr;

	DCC_LOG(LOG_MSG, "??????? ?????????");

	/* save the current stepping thread */
	thinkos_rt.step_id = thread_id;

	/* get the XPSR from the stack */
	asm volatile ("ldr   %0, [sp, #7 * 4]\n"
				  : "=r" (xpsr) : : ); 
	ipsr = xpsr & 0x1ff;

	if (ipsr == CM3_EXCEPT_PENDSV) {
		/* Clear PendSV active */
		CM3_SCB->shcsr &= ~SCB_SHCSR_PENDSVACT; 
		/* Remove NMI context */
		asm volatile ("pop    {r0-r3,r12,lr}\n"
					  "add    sp, sp, #2 * 4\n"
					  : : : ); 
	}
	/* Step and return */
	asm volatile ("movw   r3, #0xedf0\n"
				  "movt   r3, #0xe000\n"
				  "ldr    r2, [r3, #12]\n"
				  "orr.w  r2, r2, #(1 << 18)\n"
				  "str    r2, [r3, #12]\n"
				  "bx     lr\n"
				  : :  : "r3", "r2"); 
}
#else
void __thinkos_debug_step_i(unsigned int thread_id)
{
}
#endif /* THINKOS_ENABLE_DEBUG_STEP */

void __attribute__((naked)) cm3_nmi_isr(int arg1, int arg2, int arg3, int arg4)
{
	asm volatile ("add    r12, r12\n"
				  "add    r12, r12, pc\n"
				  "ldr    pc, [r12, #2]\n"
				  ".word  __thinkos_debug_step_i\n"
				  : : "r" (arg1), "r" (arg2), "r" (arg3), "r" (arg4) : ); 
}

/* FIXME: this is a hack to force linking this file. 
 The linker then will override the weak alias for the cm3_nmi_isr() */
const char thinkos_nmi_nm[] = "NMI";

