/* 
 * File:	 dac-test.c
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


#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>

#define __CM3RTOS_SYS__
#include <cm3rtos_sys.h>

#if CM3RTOS_MUTEX_MAX > 0

#if CM3RTOS_ENABLE_MUTEX_ALLOC
int cm3rtos_mutex_alloc_svc(void)
{
	return  cm3rtos_alloc_lo(&cm3rtos_rt.mutex_alloc, 0);
}

int cm3rtos_mutex_free_svc(int mutex)
{
	if (bmp_bit_rd(&cm3rtos_rt.mutex_alloc, mutex) == 0)
		return -1;

	bmp_bit_wr(&cm3rtos_rt.mutex_alloc, mutex, 0);

	return 0;
}
#endif

int cm3rtos_mutex_lock_svc(int mutex)
{
	int this = cm3rtos_rt.active >> 1;

#if CM3RTOS_ENABLE_MUTEX_ALLOC
	if (bmp_bit_rd(&cm3rtos_rt.mutex_alloc, mutex) == 0)
		return -1;
#endif

	if (cm3rtos_rt.lock[mutex] == 0) {
		cm3rtos_rt.lock[mutex] = cm3rtos_rt.active;
		return mutex;
	}

	/* insert into the mutex wait queue */
	bmp_bit_set(&cm3rtos_rt.wq_mutex[mutex], this);  

#if CM3RTOS_ENABLE_THREAD_STAT
	/* update status */
	cm3rtos_rt.th_stat[this] = __wq_idx(&cm3rtos_rt.wq_mutex[mutex]) << 1;
#endif

	/* wait for event */
	__cm3rtos_wait();

	return 0;
}


#if CM3RTOS_ENABLE_TIMED_CALLS
int cm3rtos_mutex_timedlock_svc(int mutex, unsigned int ms)
{
	int this = cm3rtos_rt.active >> 1;

#if CM3RTOS_ENABLE_MUTEX_ALLOC
	if (bmp_bit_rd(&cm3rtos_rt.mutex_alloc, mutex) == 0)
		return -1;
#endif

	if (cm3rtos_rt.lock[mutex] == 0) {
		cm3rtos_rt.lock[mutex] = cm3rtos_rt.active;
		return mutex;
	}

	/* insert into the mutex wait queue */
	bmp_bit_set(&cm3rtos_rt.wq_mutex[mutex], this);  

	/* set the clock */
	cm3rtos_rt.clock[this] = cm3rtos_rt.ticks + (ms / 1);
	/* insert into the clock wait queue */
	bmp_bit_set(&cm3rtos_rt.wq_clock, this);  

#if CM3RTOS_ENABLE_THREAD_STAT
	/* update status, mark the thread clock enable bit */
	cm3rtos_rt.th_stat[this] = __wq_idx(&cm3rtos_rt.wq_mutex[mutex]) << 1 + 1;
#endif

	cm3rtos_rt.ctx[this]->r0 = CM3RTOS_TIMEDOUT;

	/* wait for event */
	__cm3rtos_wait();

	return 0;
}
#endif


#define CM3RTOS_IDX_NULL 32

static inline int __cm3rtos_get_head(uint32_t * ptr)
{
	/* get a thread from the ready bitmap */
	return cm3_clz(cm3_rbit(*ptr));
}

int cm3rtos_mutex_unlock_svc(int mutex)
{
	int idx;

#if CM3RTOS_ENABLE_MUTEX_ALLOC
	if (bmp_bit_rd(&cm3rtos_rt.mutex_alloc, mutex) == 0)
		return -1;
#endif

	/* sanity check: avoid unlock the mutex by a thread that 
	   does not own the lock */
	if (cm3rtos_rt.lock[mutex] != cm3rtos_rt.active)
		return -1;

	idx = __cm3rtos_get_head(&cm3rtos_rt.wq_mutex[mutex]);
	if (idx == CM3RTOS_IDX_NULL) {
		/* no threads waiting on the lock, just release
		   the lock */
		cm3rtos_rt.lock[mutex] = 0;
		return 0;
	}

	/* set the mutex ownership to the new thread */
	cm3rtos_rt.lock[mutex] = (idx << 1) + 1;
#if CM3RTOS_ENABLE_THREAD_STAT
	/* update status */
	cm3rtos_rt.th_stat[idx] = 0;
#endif
	/* remove from the mutex wait queue */
	bmp_bit_clr(&cm3rtos_rt.wq_mutex[mutex], idx);  
#if CM3RTOS_ENABLE_TIMED_CALLS
	/* possibly remove from the time wait queue */
	bmp_bit_clr(&cm3rtos_rt.wq_clock, idx);  
#endif
	/* insert the thread into ready queue */
	bmp_bit_wr(&cm3rtos_rt.wq_ready, idx, 1);  

	/* set the return value */
	cm3rtos_rt.ctx[idx]->r0 = mutex;

	/* signal the scheduler ... */
	__cm3rtos_defer_sched();
	return 0;
}

#endif /* CM3RTOS_MUTEX_MAX > 0 */

