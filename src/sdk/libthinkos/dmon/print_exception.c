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

void dmon_print_exception(struct dmon_comm * comm, 
						  struct thinkos_except * xcpt)
{
//	uint32_t psr = xcpt->ctx.xpsr;
#if THINKOS_ENABLE_MPU 
	uint32_t mmfsr;
#endif
#if THINKOS_ENABLE_BUSFAULT
	uint32_t bfsr;
#endif
#if THINKOS_ENABLE_USAGEFAULT 
	uint32_t ufsr;
#endif
	uint32_t sp;

	switch (xcpt->type) {
	case CM3_EXCEPT_HARD_FAULT:
		dmprintf(comm, " Hard Fault at ");
		break;

#if THINKOS_ENABLE_MPU 
	case CM3_EXCEPT_MEM_MANAGE:
		dmprintf(comm, " Memory Manager Fault at ");
		break;
#endif

#if THINKOS_ENABLE_BUSFAULT
	case CM3_EXCEPT_BUS_FAULT:
		dmprintf(comm, " Bus Fault at ");
		break;
#endif

#if THINKOS_ENABLE_USAGEFAULT 
	case CM3_EXCEPT_USAGE_FAULT: 
		dmprintf(comm, " Usage Fault at ");
		break;
#endif
	}

	if (xcpt->thread_id != -1) {
		dmprintf(comm, "thread %d", xcpt->thread_id);
	} else {
		unsigned int xno;
		xno = xcpt->ctx.xpsr & 0xff;
		if (xno > 15) {
			dmprintf(comm, "IRQ %d", xno - 16);
		} else {
			switch (xno) {
			case CM3_EXCEPT_SVC:
				dmprintf(comm, "SVCall");
				break;
			case CM3_EXCEPT_DEBUG_MONITOR:
				dmprintf(comm, "Monitor");
				break;
			case CM3_EXCEPT_PENDSV:
				dmprintf(comm, "PendSV");
				break;
			case CM3_EXCEPT_SYSTICK:
				dmprintf(comm, "sysTick");
				break;
			}
		}	
	}

	sp = (xcpt->ret == CM3_EXC_RET_THREAD_PSP) ? xcpt->psp : xcpt->msp;
#if 0
	dmprintf(comm, "\r\n xpsr=%08x [N=%c Z=%c C=%c V=%c Q=%c T=%c "
				"ICI/IT=%02x GE=%1x XCP=%d]\r\n", 
				psr,
				((psr >> 31) & 0x01) + '0',
				((psr >> 30) & 0x01) + '0',
				((psr >> 29) & 0x01) + '0',
				((psr >> 28) & 0x01) + '0',
				((psr >> 27) & 0x01) + '0',
				((psr >> 24) & 0x01) + '0',
				((psr >> 19) & 0xc0) | ((psr >> 10) & 0x3f),
				((psr >> 16) & 0x0f),
				psr & 0x1ff);
	dmprintf(comm, "   r0=%08x   r4=%08x   r8=%08x  r12=%08x\r\n",
				xcpt->ctx.r0, xcpt->ctx.r4, xcpt->ctx.r8, xcpt->ctx.r12);
	dmprintf(comm, "   r1=%08x   r5=%08x   r9=%08x   sp=%08x\r\n", 
				xcpt->ctx.r1, xcpt->ctx.r5, xcpt->ctx.r9, sp);
	dmprintf(comm, "   r2=%08x   r6=%08x  r10=%08x   lr=%08x\r\n", 
				xcpt->ctx.r2, xcpt->ctx.r6, xcpt->ctx.r10, xcpt->ctx.lr);
	dmprintf(comm, "   r3=%08x   r7=%08x  r11=%08x   pc=%08x\r\n",  
				xcpt->ctx.r3, xcpt->ctx.r7, xcpt->ctx.r11, xcpt->ctx.pc);
#endif
	dmon_print_context(comm, &xcpt->ctx, sp);

	switch (xcpt->type) {
#if THINKOS_ENABLE_MPU 
	case CM3_EXCEPT_MEM_MANAGE:
		mmfsr = SCB_CFSR_MMFSR_GET(CM3_SCB->cfsr);
		dmprintf(comm, "mmfsr=%02x [", mmfsr);
		if (mmfsr & MMFSR_MMARVALID)
			dmprintf(comm, " MMARVALID");
		if (mmfsr & MMFSR_MLSPERR)
			dmprintf(comm, " MLSPERR");
		if (mmfsr & MMFSR_MSTKERR)
			dmprintf(comm, " MSTKERR");
		if (mmfsr & MMFSR_MUNSTKERR)
			dmprintf(comm, " MUNSTKERR");
		if (mmfsr & MMFSR_DACCVIOL)  
			dmprintf(comm, " DACCVIOL");
		if (mmfsr & MMFSR_IACCVIOL)  
			dmprintf(comm, " IACCVIOL");
		dmprintf(comm, " ]\r\n");
		if (mmfsr & MMFSR_MMARVALID) 
			dmprintf(comm, " Fault address --> %08x\r\n", 
					 (uint32_t)CM3_SCB->mmfar);
		break;
#endif

#if THINKOS_ENABLE_BUSFAULT
	case CM3_EXCEPT_BUS_FAULT:
		bfsr = SCB_CFSR_BFSR_GET(CM3_SCB->cfsr);
		dmprintf(comm, " bfsr=%02x [", bfsr);
		if (bfsr & BFSR_BFARVALID)  
			dmprintf(comm, " BFARVALID");
		if (bfsr & BFSR_LSPERR)
			dmprintf(comm, " LSPERR");
		if (bfsr & BFSR_STKERR)  
			dmprintf(comm, " STKERR");
		if (bfsr & BFSR_UNSTKERR)  
			dmprintf(comm, " UNSTKERR");
		if (bfsr & BFSR_IMPRECISERR)  
			dmprintf(comm, " IMPRECISERR");
		if (bfsr & BFSR_PRECISERR)
			dmprintf(comm, " PRECISERR");
		if (bfsr & BFSR_IBUSERR)  
			dmprintf(comm, " IBUSERR");
		dmprintf(comm, " ]\r\n");
		if (bfsr & BFSR_BFARVALID) 
			dmprintf(comm, " Fault address --> %08x\r\n", 
					 (uint32_t)CM3_SCB->bfar);
		break;
#endif

#if THINKOS_ENABLE_USAGEFAULT 
	case CM3_EXCEPT_USAGE_FAULT: 
		ufsr = SCB_CFSR_UFSR_GET(CM3_SCB->cfsr);
		dmprintf(comm, " ufsr=%04x [", ufsr);
		if (ufsr & UFSR_DIVBYZERO)  
			dmprintf(comm, " DIVBYZERO");
		if (ufsr & UFSR_UNALIGNED)  
			dmprintf(comm, " UNALIGNED");
		if (ufsr & UFSR_NOCP)  
			dmprintf(comm, " NOCP");
		if (ufsr & UFSR_INVPC)  
			dmprintf(comm, " INVPC");
		if (ufsr & UFSR_INVSTATE)  
			dmprintf(comm, " INVSTATE");
		if (ufsr & UFSR_UNDEFINSTR)  
			dmprintf(comm, " UNDEFINSTR");
		dmprintf(comm, " ]\r\n");
		break;
#endif
	}

}


