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
#include <sys/delay.h>

#if THINKOS_ENABLE_SLEEP
void thinkos_sleep_svc(int32_t * arg)
{
	uint32_t ms = (uint32_t)arg[0];
#if THINKOS_ENABLE_CLOCK
	int self = thinkos_rt.active;

	/* set the clock */
	thinkos_rt.clock[self] = thinkos_rt.ticks + ms;
	/* insert into the clock wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, self, 1);

#if THINKOS_ENABLE_THREAD_STAT
	/* mark the thread clock enable bit */
	thinkos_rt.th_stat[self] = (THINKOS_WQ_CLOCK << 1) + 1;
#endif

	DCC_LOG2(LOG_MSG, "<%d> waiting %d milliseconds...", self, ms);

	/* wait for event */
	__thinkos_suspend(self);
	/* signal the scheduler ... */
	__thinkos_defer_sched();
#else
	DCC_LOG1(LOG_MSG, "busy wait: %d milliseconds...", ms);
	udelay(1000 * ms);
#endif
}
#endif

#if THINKOS_ENABLE_ALARM
void thinkos_alarm_svc(int32_t * arg)
{
	uint32_t ms = (uint32_t)arg[0];
	int self = thinkos_rt.active;

	/* set the clock */
	thinkos_rt.clock[self] = ms;
	/* insert into the clock wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, self, 1);
#if THINKOS_ENABLE_THREAD_STAT
	/* mark the thread clock enable bit */
	thinkos_rt.th_stat[self] = (THINKOS_WQ_CLOCK << 1) + 1;
#endif
	/* wait for event */
	__thinkos_suspend(self);
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}
#endif

