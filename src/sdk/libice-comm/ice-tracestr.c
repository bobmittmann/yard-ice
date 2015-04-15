/* 
 * File:	ice-tracestr.c
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

void ice_tracestr(const struct trace_entry * __entry, const char * __str)
{
	unsigned int head;
	int fm = cm3_faultmask_get(); /* save fault mask */
	uint32_t val;
	int i;
	int c;
	
	cm3_cpsid_f(); /* disable interrupts and faults */
	if (ice_comm_blk.dbg != DBG_CONNECTED) {
		if (ice_comm_blk.dbg == DBG_SYNC)
			ice_comm_blk.dev = DEV_CONNECTED;
		goto ret;
	}
	head = ice_comm_blk.tx_head;
	while ((16 - ((head - ice_comm_blk.tx_tail) & 0xffff)) < 1);
	ice_comm_blk.tx_buf.u32[head++ & 0xf] = (int)__entry;
	i = 0;
	do {
		val = 0;
		if ((c = __str[i]) != '\0') {
			val = c;
			if ((c = __str[i + 1]) != '\0') {
				val |= c << 8;
				if ((c = __str[i + 2]) != '\0') {
					val |= c << 16;
					if ((c = __str[i + 3]) != '\0') {
						val |= c << 24;
					}
				}
			}
		} 
		while ((16 - ((head - ice_comm_blk.tx_tail) & 0xffff)) < 1);
		ice_comm_blk.tx_buf.u32[head++ & 0xf] = val;
		ice_comm_blk.tx_head = head;
		i += 4;
	} while (c != '\0');	
ret:
	cm3_faultmask_set(fm);  /* restore fault mask */
}

void dcclogstr(const struct trace_entry *, const char *) 
	__attribute__ ((weak, alias ("ice_tracestr")));
