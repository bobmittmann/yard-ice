/* 
 * File:	ice-comm_wr32.c
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

#include <sys/ice-comm.h>
#include <arch/cortex-m3.h>

void ice_comm_w32(uint32_t data) 
{
	struct ice_comm_blk * comm = ICE_COMM_BLK;
	uint32_t fm = cm3_faultmask_get(); /* save fault mask */
	unsigned int head;

	cm3_cpsid_f(); /* disable interrupts and faults */
	if (comm->dbg != DBG_CONNECTED) {
		if (comm->dbg == DBG_SYNC)
			comm->dev = DEV_CONNECTED;
		goto ret;
	}
	head = comm->tx_head;
	comm->tx_buf.u32[head++ & 0xf] = data;
	comm->tx_head = head;
ret:
	cm3_faultmask_set(fm);  /* restore fault mask */
}

