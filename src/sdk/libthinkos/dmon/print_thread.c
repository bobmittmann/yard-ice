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


void dmon_print_thread(struct dmon_comm * comm, int id)
{
	struct thinkos_thread st;
	int type;

	if (__thinkos_thread_get(&thinkos_rt, &st, id) < 0) {
		dmprintf(comm, "Thread %d is invalid!\r\n", id);
		return;
	}

	type = thinkos_obj_type_get(st.wq);

	dmprintf(comm, " - Id: %d", id); 
	if (st.th_inf != NULL)
		dmprintf(comm, ", '%s'", st.th_inf->tag); 
	else
		dmprintf(comm, ", '%s'", "..."); 

	if (THINKOS_OBJ_READY == type) {
		dmprintf(comm, " %s.\r\n", thinkos_type_name_lut[type]); 
	} else {
		dmprintf(comm, " %swait on %s(%3d)\r\n", 
				 st.tmw ? "time" : "", thinkos_type_name_lut[type], st.wq ); 
	}

	dmprintf(comm, " - Scheduler: val=%3d pri=%3d - ", 
			 st.sched_val, st.sched_pri); 
	dmprintf(comm, " timeout=%8d ms", st.timeout); 
	dmprintf(comm, " - cycles=%u\r\n", st.cyccnt); 

	dmon_print_context(comm, &st.ctx, st.sp);

	dmprintf(comm, "\r\n");
}

