/* 
 * thikos_sysinfo.c
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
 * but WITHOUT ANY WARRANTY; without flagen the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

_Pragma ("GCC optimize (\"Os\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <thinkos.h>

#include <sys/sysclk.h>

#if THINKOS_ENABLE_CTL

#if THINKOS_ENABLE_RT_DEBUG
static void rt_snapshot(uint32_t * dst)
{
	uint32_t pri = cm3_primask_get();
	uint32_t * src;
	int i;

	cm3_primask_set(1);
#if THINKOS_ENABLE_PROFILING
	{
		int self = thinkos_rt.active;
		uint32_t cyccnt = CM3_DWT->cyccnt;
		int32_t delta = cyccnt - thinkos_rt.cycref;
		/* update the reference */
		thinkos_rt.cycref = cyccnt;
		/* update thread's cycle counter */
		thinkos_rt.cyccnt[self] += delta; 
	}
#endif

	src = (uint32_t *)&thinkos_rt;

	for (i = 0; i < (sizeof(struct thinkos_rt) / 4); ++i)
		dst[i] = src[i];

#if THINKOS_ENABLE_PROFILING
	/* Reset cycle counters */
	for (i = 0; i < THINKOS_THREADS_MAX + 1; i++)
		thinkos_rt.cyccnt[i] = 0; 
#endif

	cm3_primask_set(pri);
}
#endif

extern int32_t udelay_factor;

void thinkos_ctl_svc(int32_t * arg)
{
	unsigned int req = arg[0];
	int32_t * pval;
	const uint32_t ** ptr;

	arg[0] = 0;
	
	DCC_LOG(LOG_TRACE, ".........................");

	switch (req) {
	case THINKOS_CTL_CLOCKS:
		ptr = (const uint32_t **)arg[1];
		*ptr = sysclk_hz;
		break;

	case THINKOS_CTL_UDELAY_FACTOR:
		pval = (int32_t *)arg[1];
		*pval = udelay_factor;
		break;

	case THINKOS_CTL_ABORT:
		DCC_LOG(LOG_WARNING, "Abort!");
#if 0
		{
			volatile int * ptr = (int *)(0x30000000);
			int i;

			i = ptr[0];
			(void)i;
		}
#endif
		__thinkos_pause_all();
		__thinkos_defer_sched();
#if THINKOS_ENABLE_MONITOR
		__bkpt();
#endif
		break;

	case THINKOS_CTL_TRACE:
		DCC_LOGSTR(LOG_MSG, "%s", (char *)arg[1]);
		break;

#if THINKOS_ENABLE_RT_DEBUG
	case THINKOS_CTL_SNAPSHOT:
		rt_snapshot((uint32_t *)arg[1]);
		break;
#endif

	default:
		DCC_LOG1(LOG_ERROR, "invalid sysinfo request %d!", req);
		arg[0] = THINKOS_EINVAL;
		break;
	}
}

#endif /* THINKOS_ENABLE_SYSINFO */

