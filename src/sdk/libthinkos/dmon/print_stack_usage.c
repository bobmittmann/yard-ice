/* 
 * File:	 .c
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

int __scan_stack(void * stack, unsigned int size);

extern uint32_t thinkos_dmon_stack[];
extern const uint16_t thinkos_dmon_stack_size;

void dmon_print_stack_usage(struct dmon_comm * comm)
{
	struct thinkos_rt * rt = &thinkos_rt;
	int i;

	dmprintf(comm, "\r\n Th"); 
	dmprintf(comm, " |     Tag"); 
	dmprintf(comm, " |    Stack"); 
	dmprintf(comm, " |  Context"); 
	dmprintf(comm, " |   Size"); 
	dmprintf(comm, " |   Free"); 
	dmprintf(comm, "\r\n");

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->ctx[i] != NULL) {
			dmprintf(comm, "%3d", i);
#if THINKOS_ENABLE_THREAD_INFO
			if (rt->th_inf[i] != NULL) {
				dmprintf(comm, " | %7s", rt->th_inf[i]->tag); 
				dmprintf(comm, " | %08x", (uint32_t)rt->th_inf[i]->stack_ptr); 
			} else 
#endif
			{
				dmprintf(comm, " |     ..."); 
				dmprintf(comm, " |      ..."); 
			}
			dmprintf(comm, " | %08x", (uint32_t)rt->ctx[i]); 
#if THINKOS_ENABLE_THREAD_INFO
			if (rt->th_inf[i] != NULL) {
				dmprintf(comm, " | %6d", rt->th_inf[i]->stack_size); 
				dmprintf(comm, " | %6d", __scan_stack(rt->th_inf[i]->stack_ptr, 
												rt->th_inf[i]->stack_size));
			} else 
#endif
			{
				dmprintf(comm, " |    ..."); 
				dmprintf(comm, " |    ..."); 
			}
			dmprintf(comm, "\r\n");
		}
	}

	dmprintf(comm, "%3d", THINKOS_THREAD_IDLE);
	dmprintf(comm, " |  <IDLE>"); 
	dmprintf(comm, " | %08x", (uint32_t)*thinkos_idle_stack_ptr); 
	dmprintf(comm, " | %08x", (uint32_t)rt->idle_ctx); 
	dmprintf(comm, " | %6d", THINKOS_EXCEPT_STACK_SIZE); 
	dmprintf(comm, " | %6d", __scan_stack(thinkos_except_stack, 
										  THINKOS_EXCEPT_STACK_SIZE));
	dmprintf(comm, "\r\n");

	dmprintf(comm, "%3d", -1);
	dmprintf(comm, " |  <DMON>"); 
	dmprintf(comm, " | %08x", (uint32_t)thinkos_dmon_stack); 
	dmprintf(comm, " | %08x", (uint32_t)cm3_msp_get()); 
	dmprintf(comm, " | %6d", thinkos_dmon_stack_size); 
	dmprintf(comm, " | %6d", __scan_stack(thinkos_dmon_stack, 
										thinkos_dmon_stack_size));
	dmprintf(comm, "\r\n");
}

