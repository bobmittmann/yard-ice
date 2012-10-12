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

#include <cm3rtos_svc.h>
#define __CM3RTOS_SYS__
#include <cm3rtos_sys.h>

uint32_t cm3rtos_idle_val(void)
{
	return cm3rtos_idle.snapshot.val;
}

void cm3rtos_wq_insert(unsigned int wq_id, unsigned int th_id)
{
	bmp_bit_set(&cm3rtos_rt.wq[wq_id], th_id);  
}

void cm3rtos_wq_remove(unsigned int wq_id, unsigned int th_id)
{
	bmp_bit_clr(&cm3rtos_rt.wq[wq_id], th_id);  
}

int cm3rtos_wq_get_head(unsigned int wq_id)
{
	return bmp_ffs32(&cm3rtos_rt.wq[wq_id]);
}

void cm3rtos_signal(unsigned int wq_id)
{
	int idx;

	idx = cm3_clz(cm3_rbit(cm3rtos_rt.wq[wq_id]));
	bmp_bit_set(&cm3rtos_rt.wq[wq_id], idx);
	__cm3rtos_defer_sched();
}

#if CM3RTOS_ENABLE_JOIN
int cm3rtos_join_svc(unsigned int idx)
{
#if CM3RTOS_ENABLE_THREAD_ALLOC
	if (bmp_bit_rd(&cm3rtos_rt.th_alloc, idx) == 0)
		return -1;
#endif
	
	bmp_bit_set(&cm3rtos_rt.wq_join[idx], cm3rtos_rt.active >> 1); 

#if CM3RTOS_ENABLE_CANCEL
	if (bmp_bit_rd(&cm3rtos_rt.wq_canceled, idx)) {
		bmp_bit_clr(&cm3rtos_rt.wq_canceled, idx);  
#if CM3RTOS_ENABLE_THREAD_STAT
		cm3rtos_rt.th_stat[idx] = 0;
#endif
		bmp_bit_set((void *)&cm3rtos_rt.wq_ready, idx);  
	}
#endif /* CM3RTOS_ENABLE_CANCEL */

	/* clear the LSB */
	bmp_bit_clr(&cm3rtos_rt.active, 0);  

	/* wait for event */
	__cm3rtos_wait();
	return -1;
}
#endif

#if CM3RTOS_ENABLE_CANCEL
int cm3rtos_cancel_svc(unsigned int idx, int code)
{
#if CM3RTOS_ENABLE_THREAD_ALLOC
	if (bmp_bit_rd(&cm3rtos_rt.th_alloc, idx) == 0)
		return -1;
#endif

#if CM3RTOS_ENABLE_JOIN
	/* insert into the canceled wait queue and
	 wait for a join call */ 
	bmp_bit_set(&cm3rtos_rt.wq_canceled, idx);  
#if CM3RTOS_ENABLE_THREAD_STAT
	int stat;
	/* update the thread status */
	stat = cm3rtos_rt.th_stat[idx];
	/* remove from other wait queue including wq_ready */
	bmp_bit_clr(&cm3rtos_rt.wq_lst[stat >> 1], idx);  
	cm3rtos_rt.th_stat[idx] = __cm3rtos_wq_idx(&cm3rtos_rt.wq_canceled) << 1;
#else /* CM3RTOS_ENABLE_THREAD_STAT */
	bmp_bit_clr(&cm3rtos_rt.wq_ready, idx);  
#endif /* CM3RTOS_ENABLE_THREAD_STAT */

#else /* CM3RTOS_ENABLE_JOIN */
	/* if join is not enabled insert into the ready queue */
	bmp_bit_set(&cm3rtos_rt.wq_ready, idx);  
#endif /* CM3RTOS_ENABLE_JOIN */

#if CM3RTOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	bmp_bit_clr(&cm3rtos_rt.wq_tmshare, idx);  
#endif

#if CM3RTOS_ENABLE_CLOCK
	/* possibly remove from the time wait queue */
	bmp_bit_clr(&cm3rtos_rt.wq_clock, idx);  
#endif

#if 0
	printf("%s(): <%d> cancel %d, with code %d!\n", 
		   __func__, cm3rtos_rt.active, idx, code); 
#endif
	cm3rtos_rt.ctx[idx]->pc = (uint32_t)cm3rtos_thread_exit;
	cm3rtos_rt.ctx[idx]->r0 = code;

	return 0;
}
#endif

#if CM3RTOS_ENABLE_PAUSE
int cm3rtos_resume_svc(unsigned int idx)
{
	int stat;
#if CM3RTOS_ENABLE_THREAD_ALLOC
	if (bmp_bit_rd(&cm3rtos_rt.th_alloc, idx) == 0)
		return -1;
#endif

	if (bmp_bit_rd(&cm3rtos_rt.wq_paused, idx) == 0) {
		/* not paused */
		return 0;
	}
	/* remove from the paused queue */
	bmp_bit_clr(&cm3rtos_rt.wq_paused, idx);  
	/* reinsert the thread into a waiting queue, including ready  */
	stat = cm3rtos_rt.th_stat[idx];
	bmp_bit_set(&cm3rtos_rt.wq_lst[stat >> 1], idx);  
#ifndef CM3RTOS_ENABLE_CLOCK
	/* reenable the clock according to the thread status */
	bmp_bit_set(&cm3rtos_rt.wq_clock, stat);
#endif
	__cm3rtos_defer_sched();
	return 0;
}

int cm3rtos_pause_svc(unsigned int idx)
{
#if CM3RTOS_ENABLE_THREAD_ALLOC
	if (bmp_bit_rd(&cm3rtos_rt.th_alloc, idx) == 0)
		return -1;
#endif
	/* insert into the paused queue */
	bmp_bit_set(&cm3rtos_rt.wq_paused, idx);  
	/* possibly remove from the ready queue */
	bmp_bit_clr(&cm3rtos_rt.wq_ready, idx);  
#if CM3RTOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	bmp_bit_clr(&cm3rtos_rt.wq_tmshare, idx);  
#endif
#ifndef CM3RTOS_ENABLE_CLOCK
	/* disable the clock */
	bmp_bit_clr(&cm3rtos_rt.wq_clock, idx);  
#endif
	__cm3rtos_defer_sched();
	return 0;
}
#endif /* CM3RTOS_ENABLE_PAUSE */

/* initialize a thread context */
int cm3rtos_thread_create_svc(struct cm3rtos_thread_init * init)
{
	struct cm3rtos_context * ctx;
	uint32_t sp;
	int idx;

#if CM3RTOS_ENABLE_THREAD_ALLOC
	if (init->opt.id >= CM3RTOS_THREADS_MAX) {
		idx = cm3rtos_alloc_hi(&cm3rtos_rt.th_alloc, CM3RTOS_THREADS_MAX - 1);
		printf("%s(): alloc hi: %d -> %d\n", __func__, 
			   CM3RTOS_THREADS_MAX - 1, idx);
	} else {
		/* Look for the next available slot */
		idx = cm3rtos_alloc_lo(&cm3rtos_rt.th_alloc, init->opt.id);
		printf("%s(): alloc lo: %d -> %d\n", __func__, init->opt.id, idx);
		if (idx < 0) {
			idx = cm3rtos_alloc_hi(&cm3rtos_rt.th_alloc, init->opt.id);
			printf("%s(): alloc hi: %d -> %d\n", __func__, init->opt.id, idx);
		}
	}

	if (idx < 0) {
		return idx;
	}
#else
	idx = init->opt.id;
	if (idx >= CM3RTOS_THREADS_MAX)
		idx = CM3RTOS_THREADS_MAX - 1;
#endif

	sp = (uint32_t)init->stack_ptr + init->stack_size;
	sp &= 0xfffffff8; /* 64bits alignemnt */
	sp -= sizeof(struct cm3rtos_context);

	ctx = (struct cm3rtos_context *)sp;

	ctx->r0 = (uint32_t)init->arg;
	ctx->r1 = idx;
	ctx->lr = (uint32_t)cm3rtos_thread_exit;
	ctx->pc = (uint32_t)init->task;
	ctx->xpsr = 0x01000000;

	cm3rtos_rt.ctx[idx] = ctx;

#if CM3RTOS_ENABLE_TIMESHARE
	cm3rtos_rt.sched_pri[idx] = init->opt.priority;
	if (cm3rtos_rt.sched_pri[idx] > CM3RTOS_SCHED_LIMIT_MAX)
		cm3rtos_rt.sched_pri[idx] = CM3RTOS_SCHED_LIMIT_MAX;

	/* update schedule limit */
	if (cm3rtos_rt.sched_limit < cm3rtos_rt.sched_pri[idx]) {
		cm3rtos_rt.sched_limit = cm3rtos_rt.sched_pri[idx];
	}
	cm3rtos_rt.sched_val[idx] = cm3rtos_rt.sched_limit / 2;
#endif

#if CM3RTOS_ENABLE_PAUSE
	if (init->opt.f_paused) 
		/* insert into the paused list */
		bmp_bit_set(&cm3rtos_rt.wq_paused, idx);  
	else
#endif
	{
		/* insert into the ready list */
		bmp_bit_set(&cm3rtos_rt.wq_ready, idx);  
		__cm3rtos_defer_sched();
	}

#if 0
	printf("%s(): <%d> ready=%08x priority=%d/%d task=%08x sp=%08x\n", 
		   __func__, idx, cm3rtos_rt.wq_ready, 
		   cm3rtos_rt.sched_pri[idx], cm3rtos_rt.sched_limit,
		   (int)init->task, (int)ctx);
#endif

	return idx;
}

int cm3rtos_thread_id_svc(void)
{
	return cm3rtos_rt.active >> 1;
}

void cm3rtos_sleep_svc(unsigned int ms)
{
#if CM3RTOS_ENABLE_CLOCK
	int this = cm3rtos_rt.active >> 1;

	/* set the clock */
	cm3rtos_rt.clock[this] = cm3rtos_rt.ticks + (ms / 1);
	/* insert into the clock wait queue */
	bmp_bit_set(&cm3rtos_rt.wq_clock, this);  

#if CM3RTOS_ENABLE_THREAD_STAT
	/* mark the thread clock enable bit */
	cm3rtos_rt.th_stat[this] = 1;
#endif

	/* wait for event */
	__cm3rtos_wait();
#endif
}

