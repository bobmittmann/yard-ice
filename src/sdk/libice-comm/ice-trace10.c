/* 
 * File:	ice-trace10.c
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
 * Target: 
 * Comment: 
 * Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
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

#include <stdlib.h>
#include <sys/ice-comm.h>
#include <arch/cortex-m3.h>
#include <sys/dcclog.h>

void ice_trace10(const struct dcc_trace_entry * __entry, int __a, 
				 int __b, int __c, int __d, int __e, 
				 int __f, int __g, int __h, int __i, 
				 int __j)
{
	unsigned int head;
	int fm = cm3_faultmask_get(); /* save fault mask */
	
	cm3_cpsid_f(); /* disable interrupts and faults */
	if (ice_comm_blk.dbg != DBG_CONNECTED) {
		if (ice_comm_blk.dbg == DBG_SYNC)
			ice_comm_blk.dev = DEV_CONNECTED;
		goto ret;
	}
	head = ice_comm_blk.tx_head;
	while ((16 - ((head - ice_comm_blk.tx_tail) & 0xffff)) < 11);
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = (int)__entry;
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = __a;
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = __b;
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = __c;
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = __d;
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = __e;
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = __f;
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = __g;
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = __h;
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = __i;
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = __j;
	ice_comm_blk.tx_head = head;
ret:
	cm3_faultmask_set(fm);  /* restore fault mask */
}

void dcclog10(const struct dcc_trace_entry *, int, 
			 int,  int, int, int,
			 int,  int, int, int,
			 int) 
	__attribute__ ((weak, alias ("ice_trace10")));

