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

#ifndef __ASSEMBLER__

#include <arch/cortex-m3.h>
#include <stdint.h>

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

#ifndef THINKOS_ENABLE_WFI
#define THINKOS_ENABLE_WFI 0
#endif

#ifndef THINKOS_ENABLE_PAUSE
#define THINKOS_ENABLE_PAUSE 1
#endif

#ifndef THINKOS_ENABLE_CANCEL
#define THINKOS_ENABLE_CANCEL 1
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

#ifndef THINKOS_WAIT_QUEUE_MAX
#define THINKOS_WAIT_QUEUE_MAX 8
#endif

#ifndef THINKOS_ENABLE_WAIT_QUEUE_ALLOC
#define THINKOS_ENABLE_WAIT_QUEUE_ALLOC 1
#endif

#ifndef THINKOS_ENABLE_THREAD_STAT
#define THINKOS_ENABLE_THREAD_STAT 0
#endif

#ifndef THINKOS_ENABLE_TIMED_CALLS
#define THINKOS_ENABLE_TIMED_CALLS 1
#endif

#ifndef THINKOS_ENABLE_IRQ_SVC_CALL
#define THINKOS_ENABLE_IRQ_SVC_CALL 1
#endif

#ifndef THINKOS_ENABLE_ARG_CHECK
#define THINKOS_ENABLE_ARG_CHECK 1
#endif

/* timed calls depends on clock */
#if THINKOS_ENABLE_TIMED_CALLS
 #undef THINKOS_ENABLE_CLOCK
 #define THINKOS_ENABLE_CLOCK 1
#endif

/* timed calls and pause depends on thread status */
#if THINKOS_ENABLE_TIMED_CALLS || THINKOS_ENABLE_PAUSE
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
	struct thinkos_context * ctx[32]; 
	struct thinkos_context * idle_ctx; 

	int32_t active;

	uint32_t wq_lst[0]; /* queue list placeholder */

	uint32_t wq_ready; /* ready threads queue */

#if THINKOS_ENABLE_TIMESHARE
	uint32_t wq_tmshare; /* Threads waiting for time share cycle */
#endif

#if THINKOS_ENABLE_JOIN
	uint32_t wq_canceled; /* canceled threads wait queue */
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

#if THINKOS_WAIT_QUEUE_MAX > 0
	uint32_t wq_event[THINKOS_WAIT_QUEUE_MAX]; /* event wait queue */
#endif /* THINKOS_WAIT_QUEUE_MAX > 0 */

#if THINKOS_ENABLE_JOIN
	uint32_t wq_join[THINKOS_THREADS_MAX];
#endif /* THINKOS_ENABLE_JOIN */

#if THINKOS_ENABLE_PAUSE
	uint32_t wq_paused;
#endif

#if THINKOS_ENABLE_CLOCK
	uint32_t wq_clock;
#endif

	uint32_t wq_end[0]; /* end of queue list placeholder */

#if THINKOS_ENABLE_THREAD_STAT
	uint8_t th_stat[THINKOS_THREADS_MAX]; /* Per thread status */
#endif

#if THINKOS_ENABLE_TIMESHARE
	/* This fields are used for time sharing (round robin) schedule only */
	struct {
		int8_t sched_val[32]; /* Thread schedule value */
		int8_t sched_idle_val;
		uint8_t sched_pri[32]; /* Thread schedule priority */
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

#if THINKOS_ENABLE_WAIT_QUEUE_ALLOC
	uint32_t wq_alloc;
#endif

} __attribute__ ((aligned (8)));

/* -------------------------------------------------------------------------- 
 * Thread initialization 
 * --------------------------------------------------------------------------*/

struct thinkos_thread_opt {
	uint8_t priority;
	uint8_t id;
	uint8_t f_paused: 1;
	uint8_t reserved;
};

struct thinkos_thread_init {
	void * task;
	void * arg;
	void * stack_ptr;
	uint32_t stack_size;
	struct thinkos_thread_opt opt;
};


/* -------------------------------------------------------------------------- 
 * Idle thread
 * --------------------------------------------------------------------------*/
struct thinkos_idle {
	union {
		struct thinkos_context ctx;
		struct {
			uint32_t r[12];
			uint32_t val;
			uint32_t * ptr;
		} snapshot;
	};
} __attribute__ ((aligned (8)));

struct thinkos_except_and_idle {
	uint32_t res[64 - 12];
	union {
		struct thinkos_context ctx;
		struct {
			uint32_t r[12];
			uint32_t val;
			uint32_t * ptr;
		} snapshot;
	};
} __attribute__ ((aligned (8)));

extern struct thinkos_except_and_idle thinkos_idle;

extern struct thinkos_rt thinkos_rt;

#define SYSCALL_PRIORITY       0x60
#define CLOCK_PRIORITY         0xa0
#define SCHED_PRIORITY         0xc0

#define IRQ_PRIORITY_HIGHEST   0x00
#define IRQ_PRIORITY_VERY_HIGH 0x20
#define IRQ_PRIORITY_HIGH      0x40
#define IRQ_PRIORITY_REGULAR   0x80
#define IRQ_PRIORITY_LOW       0xe0

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
static void inline bmp_bit_set(void * bmp, unsigned int bit)
{
	__bit_mem_wr(bmp, bit, 1);  
}

/* clear a bit in a bit map atomically */
static void inline bmp_bit_clr(void * bmp, unsigned int bit)
{
	__bit_mem_wr(bmp, bit, 0);  
}

static inline int thinkos_alloc_lo(uint32_t * ptr, int start) {
	int idx;
	/* Look for an empty bit MSB first */
	idx = __clz(__rbit(~(*ptr >> start))) + start;
	if (idx >= 32)
		return -1;
	/* Mark as used */
	__bit_mem_wr(ptr, idx, 1);  
	return idx;
}

static inline int thinkos_alloc_hi(uint32_t * ptr, int start) {
	int idx;
	/* Look for an empty bit LSB first */
	idx = start - __clz(~(*ptr << (31 - start)));
	if (idx < 0)
		return -1;
	/* Mark as used */
	__bit_mem_wr(ptr, idx, 1);  
	return idx;
}

/* flags a defered execution of the scheduller */
static void inline __thinkos_defer_sched(void) {
	struct cm3_scb * scb = CM3_SCB;
	/* rise a pending service interrupt */
	scb->icsr = SCB_ICSR_PENDSVSET;
}

static void inline __thinkos_wait(void) {
	/* set the non schedule flag */
	bmp_bit_clr(&thinkos_rt.wq_ready, thinkos_rt.active);  
#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
	cm3_cpsid_i();
	if (thinkos_rt.wq_ready == 0) {
		thinkos_rt.wq_ready |= thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	}
	cm3_cpsie_i();
#endif
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

static void inline __thinkos_wq_insert(uint32_t * wq, int32_t idx) {
	bmp_bit_set(wq, idx);  
	bmp_bit_clr(&thinkos_rt.wq_ready, idx);
#if THINKOS_ENABLE_TIMESHARE
	cm3_cpsid_i();
	if (thinkos_rt.wq_ready == 0) {
		/* no more threads into the ready queue,
		   move the timeshare queue to the ready queue */
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
	cm3_cpsie_i();
#endif
}

static int inline __wq_idx(uint32_t * ptr) {
	return ptr - thinkos_rt.wq_lst;
}

#define THINKOS_IDX_NULL 32

static int inline __thinkos_wq_head(uint32_t * ptr) {
	/* get a thread from the queue bitmap */
	return __clz(__rbit(*ptr));
}

static inline void __thinkos_critical_enter(void)  {
	/* rise the BASEPRI to stop the scheduler */
	cm3_basepri_set(SCHED_PRIORITY); 
}

static inline void __thinkos_critical_exit(void)  {
	/* return the BASEPRI to the default to reenable the scheduler. */
	cm3_basepri_set(0x00);
}

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __THINKOS_SYS_H__ */

