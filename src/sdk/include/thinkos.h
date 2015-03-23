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

#include <arch/cortex-m3.h>

enum {
	THINKOS_OK        =  0,
	THINKOS_ETIMEDOUT = -1,
	THINKOS_EINTR     = -2,
	THINKOS_EINVAL    = -3,
	THINKOS_EAGAIN    = -4,
	THINKOS_EDEADLK   = -5,
	THINKOS_EPERM     = -6,
	THINKOS_ENOSYS    = -7
};

enum {
	THINKOS_OBJ_READY = 0,
	THINKOS_OBJ_TMSHARE,
	THINKOS_OBJ_CANCELED,
	THINKOS_OBJ_PAUSED,
	THINKOS_OBJ_CLOCK,
	THINKOS_OBJ_MUTEX,
	THINKOS_OBJ_COND,
	THINKOS_OBJ_SEMAPHORE,
	THINKOS_OBJ_EVENT,
	THINKOS_OBJ_FLAG,
	THINKOS_OBJ_JOIN,
	THINKOS_OBJ_INVALID
};

#define THINKOS_OPT_PRIORITY(VAL) (((VAL) & 0xff) << 16)
#define THINKOS_OPT_ID(VAL) (((VAL) & 0x07f) << 24)
#define THINKOS_OPT_PAUSED (1 << 31) /* don't run at startup */
#define THINKOS_OPT_STACK_SIZE(VAL) ((VAL) & 0xffff)

#ifndef __ASSEMBLER__

struct thinkos_thread_inf {
	void * stack_ptr;
	union {
		uint32_t opt;
		struct {
			uint16_t stack_size;
			uint8_t priority;
			uint8_t thread_id: 7;
			uint8_t paused: 1;
		};
	};
	char tag[8];
};

#include <thinkos_svc.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void thinkos_yield(void)  {
	CM3_SCB->icsr = SCB_ICSR_PENDSVSET; /* PendSV rise */
	asm volatile ("dsb\n"); /* Data synchronization barrier */
}

int thinkos_init(unsigned int opt);

int thinkos_thread_create(int (* task_ptr)(void *), 
						  void * task_arg, void * stack_ptr,
						  unsigned int opt);

int thinkos_thread_create_inf(int (* task_ptr)(void *), void * task_arg,
							  const struct thinkos_thread_inf * inf);

int const thinkos_thread_self(void);

int thinkos_cancel(unsigned int thread_id, int code);

int thinkos_exit(int code);

int thinkos_join(unsigned int thread_id);

int thinkos_pause(unsigned int thread_id);

int thinkos_resume(unsigned int thread_id);

int thinkos_sleep(unsigned int ms);

int thinkos_alarm(uint32_t clock);

uint32_t thinkos_clock(void);

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

int thinkos_ev_free(int set);

int thinkos_ev_wait(int set);

int thinkos_ev_timedwait(int set, unsigned int ms);

int thinkos_ev_raise(int set, int ev);

int thinkos_ev_mask(int set, uint32_t mask);

int thinkos_ev_unmaks(int set, uint32_t mask);


int thinkos_flag_alloc(void);

int thinkos_flag_free(int flag);

int thinkos_flag_val(int flag);

int thinkos_flag_set(int flag);

int thinkos_flag_clr(int flag);

int thinkos_flag_wait(int flag);

int thinkos_flag_timedwait(int flag, unsigned int ms);

int thinkos_flag_give(int flag);

int thinkos_flag_take(int flag);

int thinkos_flag_timedtake(int flag, unsigned int ms);


int thinkos_irq_wait(int irq);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __THINKOS_H__ */

