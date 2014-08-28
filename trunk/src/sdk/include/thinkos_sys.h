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

#ifndef __THINKOS_SYS_H__
#define __THINKOS_SYS_H__


#ifndef __THINKOS_SYS__
#error "Never use <thinkos_sys.h> directly; include <thinkos.h> instead."
#endif 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef THINKOS_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#ifndef __ASSEMBLER__

#include <arch/cortex-m3.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* -------------------------------------------------------------------------- 
 * Thread context layout
 * --------------------------------------------------------------------------*/

struct thinkos_context {
	/* saved context */
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;

	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;

	/* exception context */
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;

	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t xpsr;
};

/* -------------------------------------------------------------------------- 
 * Set default configuration options
 * --------------------------------------------------------------------------*/

#ifndef THINKOS_EXCEPT_STACK_SIZE
#define THINKOS_EXCEPT_STACK_SIZE 256
#endif

#ifndef THINKOS_IRQ_MAX 
#define THINKOS_IRQ_MAX 80
#endif

#ifndef THINKOS_THREADS_MAX 
#define THINKOS_THREADS_MAX 8
#endif

#ifndef THINKOS_ENABLE_THREAD_ALLOC
#define THINKOS_ENABLE_THREAD_ALLOC 1
#endif

#ifndef THINKOS_ENABLE_TIMESHARE
#define THINKOS_ENABLE_TIMESHARE 1
#endif

#ifndef THINKOS_ENABLE_JOIN
#define THINKOS_ENABLE_JOIN 1
#endif

#ifndef THINKOS_ENABLE_IDLE_WFI
#define THINKOS_ENABLE_IDLE_WFI 1
#endif

#ifndef THINKOS_ENABLE_IDLE_SNAPSHOT
#define THINKOS_ENABLE_IDLE_SNAPSHOT 1
#endif

#ifndef THINKOS_ENABLE_PAUSE
#define THINKOS_ENABLE_PAUSE 1
#endif

#ifndef THINKOS_ENABLE_CANCEL
#define THINKOS_ENABLE_CANCEL 1
#endif

#ifndef THINKOS_ENABLE_EXIT
#define THINKOS_ENABLE_EXIT 1
#endif

#ifndef THINKOS_SCHED_LIMIT_MAX
#define THINKOS_SCHED_LIMIT_MAX 32
#endif

#ifndef THINKOS_SCHED_LIMIT_MIN
#define THINKOS_SCHED_LIMIT_MIN 1
#endif

#ifndef THINKOS_ENABLE_CLOCK
#define THINKOS_ENABLE_CLOCK 1
#endif

#ifndef THINKOS_MUTEX_MAX
#define THINKOS_MUTEX_MAX 8
#endif

#ifndef THINKOS_ENABLE_MUTEX_ALLOC
#define THINKOS_ENABLE_MUTEX_ALLOC 1
#endif

#ifndef THINKOS_COND_MAX
#define THINKOS_COND_MAX 8
#endif

#ifndef THINKOS_ENABLE_COND_ALLOC
#define THINKOS_ENABLE_COND_ALLOC 1
#endif

#ifndef THINKOS_SEMAPHORE_MAX
#define THINKOS_SEMAPHORE_MAX 8
#endif

#ifndef THINKOS_ENABLE_SEM_ALLOC
#define THINKOS_ENABLE_SEM_ALLOC 1
#endif

#ifndef THINKOS_EVENT_MAX
#define THINKOS_EVENT_MAX 8
#endif

#ifndef THINKOS_ENABLE_EVENT_ALLOC
#define THINKOS_ENABLE_EVENT_ALLOC 1
#endif

#ifndef THINKOS_ENABLE_EVENT_SYSCALL
#define THINKOS_ENABLE_EVENT_SYSCALL 0
#endif

#ifndef THINKOS_FLAG_MAX
#define THINKOS_FLAG_MAX 8
#endif

#ifndef THINKOS_ENABLE_FLAG_ALLOC
#define THINKOS_ENABLE_FLAG_ALLOC 1
#endif

#ifndef THINKOS_ENABLE_FLAG_SYSCALL
#define THINKOS_ENABLE_FLAG_SYSCALL 1
#endif

#ifndef THINKOS_ENABLE_THREAD_STAT
#define THINKOS_ENABLE_THREAD_STAT 0
#endif

#ifndef THINKOS_ENABLE_TIMED_CALLS
#define THINKOS_ENABLE_TIMED_CALLS 1
#endif

#ifndef THINKOS_ENABLE_TIMER
#define THINKOS_ENABLE_TIMER 1
#endif

#ifndef THINKOS_ENABLE_IRQ_SVC_CALL
#define THINKOS_ENABLE_IRQ_SVC_CALL 1
#endif

#ifndef THINKOS_ENABLE_ARG_CHECK
#define THINKOS_ENABLE_ARG_CHECK 1
#endif

#ifndef THINKOS_ENABLE_DEADLOCK_CHECK
#define THINKOS_ENABLE_DEADLOCK_CHECK 1
#endif

#ifndef THINKOS_ENABLE_EXCEPTIONS
#define THINKOS_ENABLE_EXCEPTIONS 1
#endif

#ifndef THINKOS_ENABLE_SCHED_DEBUG
#define THINKOS_ENABLE_SCHED_DEBUG 0
#endif

#ifndef THINKOS_ENABLE_THREAD_INFO
#define THINKOS_ENABLE_THREAD_NAME 0
#endif

/* -------------------------------------------------------------------------- 
 * Sanity check
 * --------------------------------------------------------------------------*/

#if THINKOS_ENABLE_THREAD_ALLOC && (THINKOS_THREADS_MAX > 32)
  #undef THINKOS_THREADS_MAX 
  #define THINKOS_THREADS_MAX 32
  #warn "THINKOS_THREADS_MAX set to 32"
#endif

#if (THINKOS_ENABLE_COND_ALLOC) & !(THINKOS_COND_MAX)
  #undef THINKOS_ENABLE_COND_ALLOC
  #define THINKOS_ENABLE_COND_ALLOC 0
#endif

#if (THINKOS_ENABLE_COND_ALLOC) & !(THINKOS_COND_MAX)
#undef THINKOS_ENABLE_COND_ALLOC
#define THINKOS_ENABLE_COND_ALLOC 0
#endif

#if (THINKOS_ENABLE_SEM_ALLOC) & !(THINKOS_SEMAPHORE_MAX)
#undef THINKOS_ENABLE_SEM_ALLOC
#define THINKOS_ENABLE_SEM_ALLOC 0
#endif

#if (THINKOS_ENABLE_EVENT_ALLOC) & !(THINKOS_EVENT_MAX)
#undef THINKOS_ENABLE_EVENT_ALLOC
#define THINKOS_ENABLE_EVENT_ALLOC 0
#endif

#if (THINKOS_ENABLE_FLAG_ALLOC) & !(THINKOS_FLAG_MAX)
#undef THINKOS_ENABLE_FLAG_ALLOC
#define THINKOS_ENABLE_FLAG_ALLOC 0
#endif

#if (THINKOS_ENABLE_QUEUE_ALLOC) & !(THINKOS_QUEUE_MAX)
#undef THINKOS_ENABLE_QUEUE_ALLOC
#define THINKOS_ENABLE_QUEUE_ALLOC 0
#endif

/* timed calls depends on clock */
#if THINKOS_ENABLE_TIMED_CALLS
 #undef THINKOS_ENABLE_CLOCK
 #define THINKOS_ENABLE_CLOCK 1
#endif

/* timed calls cancel and pause depends on thread status */
#if THINKOS_ENABLE_TIMED_CALLS || THINKOS_ENABLE_PAUSE || THINKOS_ENABLE_CANCEL
 #undef THINKOS_ENABLE_THREAD_STAT
 #define THINKOS_ENABLE_THREAD_STAT 1
#endif

/* -------------------------------------------------------------------------- 
 * Run Time RTOS block
 * --------------------------------------------------------------------------*/

struct thinkos_rt {
	/* This fields must be at the beginning of this structure 
	   and their order and sizes must not be changed.
	   This is critical for the scheduler operation. */

	/* Thread context pointers */
	struct thinkos_context * ctx[THINKOS_THREADS_MAX]; 
	/* Idle thread context pointer */
	struct thinkos_context * idle_ctx; 

	int32_t active; /* current active thread */

	uint32_t wq_lst[0]; /* queue list placeholder */

	uint32_t wq_ready; /* ready threads queue */

#if THINKOS_ENABLE_TIMESHARE
	uint32_t wq_tmshare; /* Threads waiting for time share cycle */
#endif

#if THINKOS_ENABLE_JOIN
	uint32_t wq_canceled; /* canceled threads wait queue */
#endif

#if THINKOS_ENABLE_PAUSE
	uint32_t wq_paused;
#endif

#if THINKOS_ENABLE_CLOCK
	uint32_t wq_clock;
#endif

#if THINKOS_MUTEX_MAX > 0
	uint32_t wq_mutex[THINKOS_MUTEX_MAX];
#endif /* THINKOS_MUTEX_MAX > 0 */

#if THINKOS_COND_MAX > 0
	uint32_t wq_cond[THINKOS_COND_MAX];
#endif /* THINKOS_COND_MAX > 0 */

#if THINKOS_SEMAPHORE_MAX > 0
	uint32_t wq_sem[THINKOS_SEMAPHORE_MAX];
#endif /* THINKOS_SEMAPHORE_MAX > 0 */

#if THINKOS_EVENT_MAX > 0
	uint32_t wq_event[THINKOS_EVENT_MAX]; /* event wait queue */
#endif /* THINKOS_EVENT_MAX > 0 */

#if THINKOS_FLAG_MAX > 0
	uint32_t wq_flag[THINKOS_FLAG_MAX]; /* flags wait queue */
#endif /* THINKOS_FLAG_MAX > 0 */

#if THINKOS_ENABLE_JOIN
	uint32_t wq_join[THINKOS_THREADS_MAX];
#endif /* THINKOS_ENABLE_JOIN */

	uint32_t wq_end[0]; /* end of queue list placeholder */

#if THINKOS_ENABLE_THREAD_STAT
	uint8_t th_stat[THINKOS_THREADS_MAX]; /* Per thread status */
#endif

#if THINKOS_ENABLE_TIMESHARE
	/* This fields are used for time sharing (round robin) schedule only */
	struct {
		int8_t sched_val[THINKOS_THREADS_MAX]; /* Thread schedule value */
		int8_t sched_idle_val;
		uint8_t sched_pri[THINKOS_THREADS_MAX]; /* Thread schedule priority */
		int8_t sched_idle_pri;
		uint32_t sched_limit; /* Limiting time for an active thread */
	};
#endif

#if THINKOS_ENABLE_CLOCK
	struct {
		uint32_t ticks;
		/* This fields are used for time wait (e.g. sleep()) */
		uint32_t clock[THINKOS_THREADS_MAX];
	};
#endif

#if THINKOS_MUTEX_MAX > 0
	int8_t lock[THINKOS_MUTEX_MAX];
#endif /* THINKOS_MUTEX_MAX > 0 */

#if THINKOS_COND_MAX > 0
	uint8_t cond_mutex[THINKOS_COND_MAX];
#endif /* THINKOS_COND_MAX > 0 */

#if THINKOS_SEMAPHORE_MAX > 0
	uint32_t sem_val[THINKOS_SEMAPHORE_MAX];
#endif /* THINKOS_SEMAPHORE_MAX > 0 */

#if THINKOS_FLAG_MAX > 0
	uint32_t flag; /* event flags */
#endif /* THINKOS_FLAG_MAX > 0 */

#if THINKOS_IRQ_MAX > 0
	int8_t irq_th[THINKOS_IRQ_MAX];
#endif /* THINKOS_IRQ_MAX */

#if THINKOS_ENABLE_THREAD_ALLOC
	uint32_t th_alloc;
#endif

#if THINKOS_ENABLE_MUTEX_ALLOC
	uint32_t mutex_alloc;
#endif

#if THINKOS_ENABLE_COND_ALLOC
	uint32_t cond_alloc;
#endif

#if THINKOS_ENABLE_SEM_ALLOC
	uint32_t sem_alloc;
#endif

#if THINKOS_ENABLE_EVENT_ALLOC
	uint32_t ev_alloc;
#endif

#if THINKOS_ENABLE_FLAG_ALLOC
	uint32_t flag_alloc;
#endif

#if THINKOS_ENABLE_SCHED_DEBUG
	uint32_t sched_trace_req;
#endif

#if THINKOS_ENABLE_THREAD_INFO
	struct thinkos_thread_info * th_inf[THINKOS_THREADS_MAX]; 
#endif

} __attribute__ ((aligned (8)));


/* -------------------------------------------------------------------------- 
 * Base indexes for the wait queue list (wq_lst[])
 * --------------------------------------------------------------------------*/

#define THINKOS_WQ_READY 0

#define THINKOS_WQ_TMSHARE ((offsetof(struct thinkos_rt, wq_tmshare) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_WQ_CANCELED ((offsetof(struct thinkos_rt, wq_canceled) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_WQ_PAUSED ((offsetof(struct thinkos_rt, wq_paused) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_WQ_CLOCK ((offsetof(struct thinkos_rt, wq_clock) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_MUTEX_BASE ((offsetof(struct thinkos_rt, wq_mutex) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_COND_BASE ((offsetof(struct thinkos_rt, wq_cond) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_SEM_BASE ((offsetof(struct thinkos_rt, wq_sem) \
						   - offsetof(struct thinkos_rt, wq_lst)) \
						  / sizeof(uint32_t))

#define THINKOS_EVENT_BASE ((offsetof(struct thinkos_rt, wq_event) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_FLAG_BASE ((offsetof(struct thinkos_rt, wq_flag) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_JOIN_BASE ((offsetof(struct thinkos_rt, wq_join) \
						   - offsetof(struct thinkos_rt, wq_lst)) \
						  / sizeof(uint32_t))

#define THINKOS_WQ_LST_END ((offsetof(struct thinkos_rt, wq_end) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

/* -------------------------------------------------------------------------- 
 * Thread initialization 
 * --------------------------------------------------------------------------*/

struct thinkos_thread_opt {
	uint16_t stack_size;
	uint8_t priority;
	uint8_t id: 7;
	uint8_t f_paused: 1;
};

struct thinkos_thread_init {
	void * task;
	void * arg;
	void * stack_ptr;
	struct thinkos_thread_opt opt;
	struct thinkos_thread_info * inf;
};


/* -------------------------------------------------------------------------- 
 * Idle thread
 * --------------------------------------------------------------------------*/

#define IDLE_UNUSED_REGS 12

struct thinkos_idle {
	union {
		struct thinkos_context ctx;
		struct {
			uint32_t r[IDLE_UNUSED_REGS];
			uint32_t val;
			uint32_t * ptr;
		} snapshot;
	};
} __attribute__ ((aligned (8)));

struct thinkos_except_and_idle {
	uint32_t except_stack[(THINKOS_EXCEPT_STACK_SIZE / 4) - IDLE_UNUSED_REGS];
	union {
		struct thinkos_context ctx;
		struct {
			uint32_t res2[IDLE_UNUSED_REGS];
			uint32_t r12;
			uint32_t lr;
			uint32_t pc;
			uint32_t xpsr;
		} stack;
		struct {
			uint32_t res3[IDLE_UNUSED_REGS];
			uint32_t val;
			uint32_t * ptr;
			uint32_t pc;
			uint32_t xpsr;
		} snapshot;
	};
} __attribute__ ((aligned (8)));

extern struct thinkos_except_and_idle thinkos_idle;

extern struct thinkos_rt thinkos_rt;

#ifdef __cplusplus
extern "C" {
#endif

void __attribute__((noreturn)) thinkos_thread_exit(int code);

void __attribute__((noreturn)) thinkos_idle_task(void);

/* Moves the current MSP to PSP and 
   assert a new MSP stack top */
void cm3_msp_init(uint64_t * stack_top);

/* -------------------------------------------------------------------------- 
 * Support Functions
 * --------------------------------------------------------------------------*/

/* set a bit in a bit map atomically */
static void inline __attribute__((always_inline)) 
bmp_bit_set(void * bmp, unsigned int bit)
{
	__bit_mem_wr(bmp, bit, 1);  
}

/* clear a bit in a bit map atomically */
static void inline __attribute__((always_inline)) 
bmp_bit_clr(void * bmp, unsigned int bit)
{
	__bit_mem_wr(bmp, bit, 0);  
}

static inline int __attribute__((always_inline)) 
thinkos_alloc_lo(uint32_t * ptr, int start) {
	int idx;
	/* Look for an empty bit MSB first */
	idx = __clz(__rbit(~(*ptr >> start))) + start;
	if (idx >= 32)
		return -1;
	/* Mark as used */
	__bit_mem_wr(ptr, idx, 1);  
	return idx;
}

static inline int __attribute__((always_inline)) 
thinkos_alloc_hi(uint32_t * ptr, int start) {
	int idx;

	if (start > 31)
		start = 31;

	/* Look for an empty bit LSB first */
	idx = start - __clz(~(*ptr << (31 - start)));
	if (idx < 0)
		return -1;
	/* Mark as used */
	__bit_mem_wr(ptr, idx, 1);  
	return idx;
}

/* flags a deferred execution of the scheduler */
static void inline __attribute__((always_inline)) __thinkos_defer_sched(void) {
	struct cm3_scb * scb = CM3_SCB;
	/* rise a pending service interrupt */
	scb->icsr = SCB_ICSR_PENDSVSET;
}

static void inline __attribute__((always_inline)) __thinkos_wait(void) {
	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, thinkos_rt.active, 0);  
#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
	cm3_cpsid_i();
#if (THINKOS_THREADS_MAX < 32) 
	if (thinkos_rt.wq_ready == (1 << THINKOS_THREADS_MAX)) {
		/* No more threads into the ready queue,
		 move the timeshare queue to the ready queue.
		 Keep the IDLE bit set */
		thinkos_rt.wq_ready |= thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#else
	if (thinkos_rt.wq_ready == 0) {
		/* no more threads into the ready queue,
		 move the timeshare queue to the ready queue */
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#endif
	cm3_cpsie_i();
#endif
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

#define THINKOS_THREAD_NULL 32
#define THINKOS_THREAD_IDLE THINKOS_THREADS_MAX

static int inline __attribute__((always_inline)) __wq_idx(uint32_t * ptr) {
	return ptr - thinkos_rt.wq_lst;
}

static int inline __attribute__((always_inline)) 
__thinkos_wq_head(unsigned int wq) {
	/* get a thread from the queue bitmap */
	return __clz(__rbit(thinkos_rt.wq_lst[wq]));
}

static void inline __attribute__((always_inline)) 
__thinkos_wq_insert(unsigned int wq, unsigned int th) {
	/* insert into the event wait queue */
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);  
#if THINKOS_ENABLE_THREAD_STAT
	thinkos_rt.th_stat[th] = wq << 1;
#endif
}

#if THINKOS_ENABLE_TIMED_CALLS
static void inline __attribute__((always_inline)) 
__thinkos_tmdwq_insert(unsigned int wq, unsigned int th, unsigned int ms) {
	/* set the clock */
	thinkos_rt.clock[th] = thinkos_rt.ticks + ms;
	/* insert into the event wait queue */
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
	/* insert into the clock wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 1);  
#if THINKOS_ENABLE_THREAD_STAT
	/* update status, mark the thread clock enable bit */
	thinkos_rt.th_stat[th] = (wq << 1) + 1;
#endif
}
#endif

static void inline __attribute__((always_inline)) 
__thinkos_wq_remove(unsigned int wq, unsigned int th) {
	/* remove from the wait queue */
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 0);  
#if THINKOS_ENABLE_TIMED_CALLS
	/* possibly remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);  
#endif
}

static void inline __attribute__((always_inline)) 
__thinkos_wakeup(unsigned int wq, unsigned int th) {
	/* insert the thread into ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
	/* remove from the wait queue */
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 0);  
#if THINKOS_ENABLE_TIMED_CALLS
	/* possibly remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);  
	/* set the thread's return value */
	thinkos_rt.ctx[th]->r0 = 0;
#endif
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[th] = 0;
#endif
}

#if THINKOS_ENABLE_TIMER
static void inline __attribute__((always_inline))
__thinkos_timer_set(unsigned int ms) {
	int self = thinkos_rt.active;
#if THINKOS_ENABLE_CLOCK
	/* set the clock */
	thinkos_rt.clock[self] = thinkos_rt.ticks + ms;
	/* insert into the clock wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, self, 1);
#if THINKOS_ENABLE_THREAD_STAT
	/* update status, mark the thread clock enable bit */
	__bit_mem_wr(&thinkos_rt.th_stat[self], 0, 1);
#endif
#else
#error "__thinkos_timer_set() depends on THINKOS_ENABLE_CLOCK"
#endif
}

static void inline __attribute__((always_inline))
	__thinkos_timer_cancel(void) {
	int self = thinkos_rt.active;
#if THINKOS_ENABLE_CLOCK
	/* remove from the clock wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, self, 0);
#if THINKOS_ENABLE_THREAD_STAT
	/* clear the thread clock enable bit */
	__bit_mem_wr(&thinkos_rt.th_stat[self], 0, 0);
#endif
#else
#error "__thinkos_timer_cancel() depends on THINKOS_ENABLE_CLOCK"
#endif
}

static bool inline __attribute__((always_inline))
	__thinkos_timer_timedout(void) {
	int self = thinkos_rt.active;
#if THINKOS_ENABLE_CLOCK
	return ((int32_t)(thinkos_rt.clock[self] - thinkos_rt.ticks) <= 0) ? true : false;
#else
#error "__thinkos_timedout() depends on THINKOS_ENABLE_CLOCK"
#endif
}

static volatile inline uint32_t __attribute__((always_inline))
	__thinkos_ticks(void) {
#if THINKOS_ENABLE_CLOCK
	return thinkos_rt.ticks;
#else
#error "__thinkos_ticks() depends on THINKOS_ENABLE_CLOCK"
#endif
}

#endif /* THINKOS_ENABLE_TIMER */

void thinkos_rt_snapshot(struct thinkos_rt * rt);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __THINKOS_SYS_H__ */

