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


void dmon_print_context(struct dmon_comm * comm, 
						const struct thinkos_context * ctx, 
						uint32_t sp)
{
	uint32_t psr = ctx->xpsr;

	dmprintf(comm, " xpsr=%08x [N=%c Z=%c C=%c V=%c Q=%c "
				"ICI/IT=%02x XCP=%02x]\r\n", 
				psr,
				((psr >> 31) & 0x01) + '0',
				((psr >> 30) & 0x01) + '0',
				((psr >> 29) & 0x01) + '0',
				((psr >> 28) & 0x01) + '0',
				((psr >> 27) & 0x01) + '0',
				((psr >> 19) & 0xc0) | ((psr >> 10) & 0x3f),
				psr & 0xff);
	dmprintf(comm, "   r0=%08x   r4=%08x   r8=%08x  r12=%08x\r\n",
				ctx->r0, ctx->r4, ctx->r8, ctx->r12);
	dmprintf(comm, "   r1=%08x   r5=%08x   r9=%08x   sp=%08x\r\n", 
				ctx->r1, ctx->r5, ctx->r9, sp);
	dmprintf(comm, "   r2=%08x   r6=%08x  r10=%08x   lr=%08x\r\n", 
				ctx->r2, ctx->r6, ctx->r10, ctx->lr);
	dmprintf(comm, "   r3=%08x   r7=%08x  r11=%08x   pc=%08x\r\n",  
				ctx->r3, ctx->r7, ctx->r11, ctx->pc);
}


