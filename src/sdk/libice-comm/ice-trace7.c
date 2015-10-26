/* 
 * File:	ice-trace7.c
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

void ice_trace7(const struct dcc_trace_entry * __entry, int __a, 
				int __b, int __c, int __d, int __e, 
				int __f, int __g)
{
	struct ice_comm_blk * comm = ICE_COMM_BLK;
	unsigned int head;
	int fm = cm3_faultmask_get(); /* save fault mask */
	
	cm3_cpsid_f(); /* disable interrupts and faults */
	if (comm->dbg != DBG_CONNECTED) {
		if (comm->dbg == DBG_SYNC)
			comm->dev = DEV_CONNECTED;
		goto ret;
	}
	head = comm->tx_head;
	while ((16 - ((head - comm->tx_tail) & 0xffff)) < 8);
	comm->tx_buf.u32[head++ & 0xf] = (int)__entry;
	comm->tx_buf.u32[head++ & 0xf] = __a;
	comm->tx_buf.u32[head++ & 0xf] = __b;
	comm->tx_buf.u32[head++ & 0xf] = __c;
	comm->tx_buf.u32[head++ & 0xf] = __d;
	comm->tx_buf.u32[head++ & 0xf] = __e;
	comm->tx_buf.u32[head++ & 0xf] = __f;
	comm->tx_buf.u32[head++ & 0xf] = __g;
	comm->tx_head = head;
ret:
	cm3_faultmask_set(fm);  /* restore fault mask */
}

void dcclog7(const struct dcc_trace_entry *, int, 
			 int,  int, int, int,
			 int,  int) 
	__attribute__ ((weak, alias ("ice_trace7")));

