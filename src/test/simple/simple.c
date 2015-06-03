/* 
 * File:	 simple.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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
#include <sys/param.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>

#include <sys/dcclog.h>

#define __THINKOS_SYS__
#include <thinkos_sys.h>

void _init(void)
{
}

void _except(void)
{
}

void _isr(void)
{
}


void stm32f_can1_sce_isr(void)
{
	__NOP();
}

void stm32f_exti5_isr(void)
{
	__NOP();
}

void stm32f_wwdg_isr(void)
{
	__NOP();
}

void stm32f_pvd_isr(void)
{
	__NOP();
}

void stm32f_rtc_wkup_isr(void)
{
	__NOP();
}

void queue_insert_bitband(uint32_t * q, unsigned int i)
{
	__bit_mem_wr(q, i, 1);
}

void queue_insert_strex(uint32_t * q, unsigned int i)
{
	uint32_t x;
	do {
		x = __ldrexw(q);
		x |= (1 << i);
	} while (__strexw(q, x));
}

void queue_insert_atomic(uint32_t * q, unsigned int i)
{
	uint32_t x = 0;

	__atomic_fetch_or(q, (1 << i), x);
}


void queue_move_strex(uint32_t * q1, uint32_t * q2, unsigned int i)
{
	uint32_t x;

	do {
		x = __ldrexw(q1);
		x &= ~(1 << i);
	} while (__strexw(q1, x));

	do {
		x = __ldrexw(q2);
		x |= (1 << i);
	} while (__strexw(q2, x));
}

void queue_move_atomic(uint32_t * q1, uint32_t * q2, unsigned int i)
{
	uint32_t x = 0;

	__atomic_fetch_and(q1, ~(1 << i), x);
	__atomic_fetch_or(q2, (1 << i), x);
}

void thread_wait(unsigned int thread_id)
{
	uint32_t rdy;

	do {
		rdy = __ldrexw(&thinkos_rt.wq_ready);
		rdy &= ~(1 << thread_id);
		if (rdy == 0) {
			/* no more threads into the ready queue,
			   move the timeshare queue to the ready queue */
			rdy = thinkos_rt.wq_tmshare;
			thinkos_rt.wq_tmshare = 0;
		} 
	} while (__strexw(&thinkos_rt.wq_ready, rdy));

	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void thread_wait2(unsigned int thread_id)
{
	uint32_t rdy;
	uint32_t tms;

	do {
		rdy = __ldrexw(&thinkos_rt.wq_ready);
		tms = thinkos_rt.wq_tmshare;
		rdy &= ~(1 << thread_id);
#if ((THINKOS_THREADS_MAX) < 32) 
		if (rdy == (1 << (THINKOS_THREADS_MAX))) {
#else
		if (rdy == 0) {
#endif
			/* no more threads into the ready queue,
			   move the timeshare queue to the ready queue */
			rdy |= tms;
			tms = 0;
		} 
	} while (__strexw(&thinkos_rt.wq_ready, rdy));

	thinkos_rt.wq_tmshare = tms;

	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void thread_suspend(unsigned int thread_id)
{
	__thinkos_suspend(thread_id);
	__thinkos_defer_sched();
}

struct thinkos_rt thinkos_rt;

int main(int argc, char ** argv)
{
	int i;
	for (;;) {
		i++;
	}

	return 0;
}

