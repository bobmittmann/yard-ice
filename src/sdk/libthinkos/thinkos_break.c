/* 
 * thikos.c
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

#if THINKOS_ENABLE_BREAK

extern const uint8_t thinkos_obj_type_lut[];

void thinkos_break_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	uint32_t * alloc;
	unsigned int idx;
	int type;

	if (wq >= THINKOS_WQ_LST_END) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif 

#if THINKOS_ENABLE_ARG_CHECK
	type = thinkos_obj_type_lut[wq];
	alloc = thinkos_obj_alloc_lut[type];
	idx = wq - thinkos_wq_base_lut[type];

	if ((alloc != NULL) && __bit_mem_rd(alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid object %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif 

	cm3_cpsid_i();

	/* remove all threads from the wait queue */
	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		do {
			/* break (wakeup) from the wait queue and set
			 the return value to EINTR */
			__thinkos_wakeup_return(wq, th, THINKOS_EINTR);
			DCC_LOG2(LOG_INFO, "<%d> wakeup from %d.", th, wq);
		} while  ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}

	cm3_cpsie_i();

	arg[0] = 0;
}

#endif /* THINKOS_ENABLE_BREAK */

