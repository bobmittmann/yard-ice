/* 
 * thikos.h
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


#ifndef __THINKOS_H__
#define __THINKOS_H__

#include <thinkos_svc.h>
#include <arch/cortex-m3.h>

#define THINKOS_ERROR -1
#define THINKOS_ETIMEDOUT -2
#define THINKOS_EINVAL -3
#define THINKOS_EAGAIN -4
#define THINKOS_EDEADLK -5
#define THINKOS_EPERM -6
#define THINKOS_ENOSYS -7

#define THINKOS_OPT_PRIORITY(VAL) ((VAL) & 0xff)
#define THINKOS_OPT_ID(VAL) (((VAL) & 0x07f) << 8)
#define THINKOS_OPT_PAUSED (1 << 16)

#ifndef __ASSEMBLER__

#ifdef __cplusplus
extern "C" {
#endif

static inline void thinkos_yield(void)  {
	CM3_SCB->icsr = SCB_ICSR_PENDSVSET; /* PendSV rise */
	asm volatile ("dsb\n"); /* Data synchronization barrier */
}

int thinkos_init(unsigned int opt);

int thinkos_thread_create(int (* task)(void *), 
						  void * arg, void * stack_ptr,
						  unsigned int stack_size,
						  unsigned int opt);

int const thinkos_thread_self(void);


int thinkos_cancel(unsigned int thread_id, int code);

int thinkos_join(unsigned int thread_id);

int thinkos_pause(unsigned int thread_id);

int thinkos_resume(unsigned int thread_id);

void thinkos_sleep(unsigned int ms);


int thinkos_mutex_alloc(void);

int thinkos_mutex_free(int mutex);

int thinkos_mutex_lock(int mutex);

int thinkos_mutex_trylock(int mutex);

int thinkos_mutex_timedlock(int mutex, unsigned int ms);

int thinkos_mutex_unlock(int mutex);


int thinkos_cond_alloc(void);

int thinkos_cond_free(int cond);

int thinkos_cond_wait(int cond, int mutex);

int thinkos_cond_timedwait(int cond, int mutex, unsigned int ms);

int thinkos_cond_signal(int cond);

int thinkos_cond_broadcast(int cond);


int thinkos_sem_alloc(unsigned int value);

int thinkos_sem_free(int sem);

int thinkos_sem_init(int sem, unsigned int value);

int thinkos_sem_wait(int sem);

int thinkos_sem_timedwait(int sem, unsigned int ms);

int thinkos_sem_post(int sem);


int thinkos_ev_alloc(void);

int thinkos_ev_free(int ev);

int thinkos_ev_wait(int ev);

int thinkos_ev_timedwait(int ev, unsigned int ms);

int thinkos_ev_raise(int ev);


int thinkos_flag_alloc(void);

int thinkos_flag_free(int flag);

int thinkos_flag_wait(int flag);

int thinkos_flag_timedwait(int flag, unsigned int ms);

int thinkos_flag_clr(int flag);

int thinkos_flag_set(int flag);


int thinkos_irq_wait(int irq);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __THINKOS_H__ */

