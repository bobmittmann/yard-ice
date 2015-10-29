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
 * Set default configuration options
 * --------------------------------------------------------------------------*/

#ifndef THINKOS_EXCEPT_STACK_SIZE
#define THINKOS_EXCEPT_STACK_SIZE       320
#endif

#ifndef THINKOS_IRQ_MAX 
#define THINKOS_IRQ_MAX                 80
#endif

#ifndef THINKOS_ENABLE_IRQ_CTL
#define THINKOS_ENABLE_IRQ_CTL          0
#endif

#ifndef THINKOS_THREADS_MAX 
#define THINKOS_THREADS_MAX             8
#endif

#ifndef THINKOS_ENABLE_THREAD_ALLOC
#define THINKOS_ENABLE_THREAD_ALLOC     1
#endif

#ifndef THINKOS_ENABLE_THREAD_INFO
#define THINKOS_ENABLE_THREAD_INFO      1
#endif

#ifndef THINKOS_ENABLE_THREAD_STAT
#define THINKOS_ENABLE_THREAD_STAT      1
#endif

#ifndef THINKOS_ENABLE_JOIN
#define THINKOS_ENABLE_JOIN             1
#endif

#ifndef THINKOS_ENABLE_PAUSE
#define THINKOS_ENABLE_PAUSE            1
#endif

#ifndef THINKOS_ENABLE_CANCEL
#define THINKOS_ENABLE_CANCEL           1
#endif

#ifndef THINKOS_ENABLE_EXIT
#define THINKOS_ENABLE_EXIT             1
#endif

#ifndef THINKOS_ENABLE_TERMINATE
#define THINKOS_ENABLE_TERMINATE        1
#endif

#ifndef THINKOS_ENABLE_TIMESHARE
#define THINKOS_ENABLE_TIMESHARE        1
#endif

#ifndef THINKOS_SCHED_LIMIT_MAX
#define THINKOS_SCHED_LIMIT_MAX         32
#endif

#ifndef THINKOS_SCHED_LIMIT_MIN
#define THINKOS_SCHED_LIMIT_MIN         1
#endif

#ifndef THINKOS_MUTEX_MAX
#define THINKOS_MUTEX_MAX               8
#endif

#ifndef THINKOS_ENABLE_MUTEX_ALLOC
#define THINKOS_ENABLE_MUTEX_ALLOC      1
#endif

#ifndef THINKOS_COND_MAX
#define THINKOS_COND_MAX                8
#endif

#ifndef THINKOS_ENABLE_COND_ALLOC
#define THINKOS_ENABLE_COND_ALLOC       1
#endif

#ifndef THINKOS_SEMAPHORE_MAX
#define THINKOS_SEMAPHORE_MAX           8
#endif

#ifndef THINKOS_ENABLE_SEM_ALLOC
#define THINKOS_ENABLE_SEM_ALLOC        1
#endif

#ifndef THINKOS_EVENT_MAX
#define THINKOS_EVENT_MAX               4
#endif

#ifndef THINKOS_ENABLE_EVENT_ALLOC
#define THINKOS_ENABLE_EVENT_ALLOC      1
#endif

#ifndef THINKOS_FLAG_MAX
#define THINKOS_FLAG_MAX                8
#endif

#ifndef THINKOS_ENABLE_FLAG_ALLOC
#define THINKOS_ENABLE_FLAG_ALLOC       1
#endif

#ifndef THINKOS_ENABLE_FLAG_WATCH
#define THINKOS_ENABLE_FLAG_WATCH       1
#endif

#ifndef THINKOS_GATE_MAX
#define THINKOS_GATE_MAX                8
#endif

#ifndef THINKOS_ENABLE_GATE_ALLOC
#define THINKOS_ENABLE_GATE_ALLOC       1
#endif

#ifndef THINKOS_ENABLE_TIMED_CALLS
#define THINKOS_ENABLE_TIMED_CALLS      1
#endif

#ifndef THINKOS_ENABLE_CLOCK
#define THINKOS_ENABLE_CLOCK            1
#endif

#ifndef THINKOS_ENABLE_ALARM
#define THINKOS_ENABLE_ALARM            1
#endif

#ifndef THINKOS_ENABLE_SLEEP
#define THINKOS_ENABLE_SLEEP            1
#endif

#ifndef THINKOS_ENABLE_BREAK
#define THINKOS_ENABLE_BREAK            1
#endif

#ifndef THINKOS_ENABLE_ARG_CHECK
#define THINKOS_ENABLE_ARG_CHECK        1
#endif

#ifndef THINKOS_ENABLE_DEADLOCK_CHECK
#define THINKOS_ENABLE_DEADLOCK_CHECK   1
#endif

#ifndef THINKOS_ENABLE_SANITY_CHECK
#define THINKOS_ENABLE_SANITY_CHECK     1
#endif

#ifndef THINKOS_ENABLE_EXCEPTIONS
#define THINKOS_ENABLE_EXCEPTIONS       1
#endif

#ifndef THINKOS_SYSRST_ONFAULT
#define THINKOS_SYSRST_ONFAULT          0
#endif

#ifndef THINKOS_STDERR_FAULT_DUMP
#define THINKOS_STDERR_FAULT_DUMP       0
#endif

#ifndef THINKOS_ENABLE_BUSFAULT
#define THINKOS_ENABLE_BUSFAULT         0
#endif

#ifndef THINKOS_ENABLE_USAGEFAULT 
#define THINKOS_ENABLE_USAGEFAULT       0
#endif

#ifndef THINKOS_ENABLE_MEMFAULT 
#define THINKOS_ENABLE_MEMFAULT         0
#endif

#ifndef THINKOS_UNROLL_EXCEPTIONS 
#define THINKOS_UNROLL_EXCEPTIONS       0
#endif

#ifndef THINKOS_ENABLE_SCHED_DEBUG
#define THINKOS_ENABLE_SCHED_DEBUG      0
#endif

#ifndef THINKOS_ENABLE_RT_DEBUG
#define THINKOS_ENABLE_RT_DEBUG         0
#endif

#ifndef THINKOS_ENABLE_PROFILING
#define THINKOS_ENABLE_PROFILING        0
#endif

#ifndef THINKOS_ENABLE_MONITOR
#define THINKOS_ENABLE_MONITOR          0
#endif

#ifndef THINKOS_ENABLE_DMCLOCK
#define THINKOS_ENABLE_DMCLOCK          0
#endif

#ifndef THINKOS_ENABLE_DEBUG_STEP 
#define THINKOS_ENABLE_DEBUG_STEP       0
#endif

#ifndef THINKOS_ENABLE_DEBUG_FAULT
#define THINKOS_ENABLE_DEBUG_FAULT      0
#endif

#ifndef THINKOS_ENABLE_EXCEPT_CLEAR
#define THINKOS_ENABLE_EXCEPT_CLEAR     1
#endif

#ifndef THINKOS_ENABLE_MPU 
#define THINKOS_ENABLE_MPU              0
#endif

#ifndef THINKOS_ENABLE_FPU 
#define THINKOS_ENABLE_FPU              0
#endif

#ifndef THINKOS_ENABLE_CTL
#define THINKOS_ENABLE_CTL              0
#endif

#ifndef THINKOS_ENABLE_COMM
#define THINKOS_ENABLE_COMM             0
#endif

#ifndef THINKOS_ENABLE_STACK_INIT
#define THINKOS_ENABLE_STACK_INIT       1
#endif

#ifndef THINKOS_ENABLE_THREAD_VOID 
#define THINKOS_ENABLE_THREAD_VOID      1
#endif

#ifndef THINKOS_ENABLE_ESCALATE
#define THINKOS_ENABLE_ESCALATE         THINKOS_ENABLE_MPU
#endif

#ifndef THINKOS_ENABLE_IDLE_WFI
#define THINKOS_ENABLE_IDLE_WFI         1
#endif

#ifndef THINKOS_IDLE_STACK_CONST
#define THINKOS_IDLE_STACK_CONST        0
#endif

#ifndef THINKOS_IDLE_STACK_BSS 
#define THINKOS_IDLE_STACK_BSS          0
#endif

#ifndef THINKOS_IDLE_STACK_ALLOC
  #if THINKOS_IDLE_STACK_CONST || THINKOS_IDLE_STACK_BSS 
    #define THINKOS_IDLE_STACK_ALLOC    0
  #else
    #define THINKOS_IDLE_STACK_ALLOC    1
  #endif
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

#if (THINKOS_ENABLE_GATE_ALLOC) & !(THINKOS_GATE_MAX)
#undef THINKOS_ENABLE_GATE_ALLOC
#define THINKOS_ENABLE_GATE_ALLOC 0
#endif

/* timed calls depends on clock */
#if THINKOS_ENABLE_TIMED_CALLS
 #undef THINKOS_ENABLE_CLOCK
 #define THINKOS_ENABLE_CLOCK 1
#endif

/* dbug step depends on monitor */
#if (THINKOS_ENABLE_DEBUG_STEP) & (!THINKOS_ENABLE_MONITOR)
 #undef THINKOS_ENABLE_MONITOR
 #define THINKOS_ENABLE_MONITOR 1
#endif

/* timed calls, cancel, pause and debug step depend on thread status */
#if THINKOS_ENABLE_TIMED_CALLS || THINKOS_ENABLE_PAUSE || \
	THINKOS_ENABLE_CANCEL || THINKOS_ENABLE_DEBUG_STEP 
 #undef THINKOS_ENABLE_THREAD_STAT
 #define THINKOS_ENABLE_THREAD_STAT 1
#endif

#if THINKOS_ENABLE_EXIT  || THINKOS_ENABLE_CANCEL || THINKOS_ENABLE_JOIN
 #undef THINKOS_ENABLE_THREAD_VOID
 #define THINKOS_ENABLE_THREAD_VOID 1
 #undef THINKOS_ENABLE_TERMINATE
 #define THINKOS_ENABLE_TERMINATE 1
#endif

#if THINKOS_ENABLE_MEMFAULT && !THINKOS_ENABLE_MPU 
#undef THINKOS_ENABLE_MEMFAULT 
#define THINKOS_ENABLE_MEMFAULT 0
#endif
/* -------------------------------------------------------------------------- 
 * Thread context layout
 * --------------------------------------------------------------------------*/

struct thinkos_context {
#if THINKOS_ENABLE_FPU 
	union {
		uint32_t s[32];
		uint64_t d[16];
	};
#endif
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

#if THINKOS_ENABLE_FPU 
  #define CTX_R0 (8 + 32)
#else
  #define CTX_R0 8
#endif

/* -------------------------------------------------------------------------- 
 * Flattened thread state structure
 * --------------------------------------------------------------------------*/

struct thinkos_thread {
	uint8_t idx;
	uint8_t tmw: 1;
	uint8_t alloc: 1;
	uint16_t wq;
	int8_t sched_val;
	uint8_t sched_pri;
	int32_t timeout;
	uint32_t cyccnt;
	struct thinkos_thread_inf * th_inf;
	uint32_t sp;
	struct thinkos_context ctx;
};

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
#if THINKOS_ENABLE_THREAD_VOID 
	/* void thread context pointer */
	struct thinkos_context * void_ctx; 
#endif

	int32_t active; /* current active thread */

#if THINKOS_ENABLE_DEBUG_STEP
	uint32_t step_req; /* step request bitmap */
	uint32_t step_svc; /* step at service call bitmap */
	uint32_t step_brk; /* stop after at service call bitmap */
	int8_t step_id; /* current stepping thread id */
	int8_t break_id; /* thread stopped by a breakpoint or step request */
	uint16_t xcpt_ipsr; /* Exception */
#endif

#if THINKOS_ENABLE_PROFILING
	/* Reference cycle state ... */
	uint32_t cycref;
	/* Per thread cycle count */
#if THINKOS_ENABLE_THREAD_VOID 
	uint32_t cyccnt[(THINKOS_THREADS_MAX) + 2]; /* extra slot for void thread */
#else
	uint32_t cyccnt[(THINKOS_THREADS_MAX) + 1];
#endif
#endif

	uint32_t wq_lst[0]; /* queue list placeholder */

	uint32_t wq_ready; /* ready threads queue */

#if THINKOS_ENABLE_TIMESHARE
	uint32_t wq_tmshare; /* Threads waiting for time share cycle */
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
	uint32_t wq_event[THINKOS_EVENT_MAX]; /* event sets wait queues */
#endif /* THINKOS_EVENT_MAX > 0 */

#if THINKOS_FLAG_MAX > 0
	uint32_t wq_flag[THINKOS_FLAG_MAX]; /* flags wait queues */
#endif /* THINKOS_FLAG_MAX > 0 */

#if THINKOS_GATE_MAX > 0
	uint32_t wq_gate[THINKOS_GATE_MAX]; /* gates wait queues */
#endif /* THINKOS_GATE_MAX > 0 */

#if THINKOS_ENABLE_JOIN
	uint32_t wq_join[THINKOS_THREADS_MAX];
#endif /* THINKOS_ENABLE_JOIN */

#if THINKOS_ENABLE_CONSOLE
	uint32_t wq_console_wr;
	uint32_t wq_console_rd;
#endif

#if THINKOS_ENABLE_PAUSE
	uint32_t wq_paused;
#endif

#if THINKOS_ENABLE_JOIN
	uint32_t wq_canceled; /* canceled threads wait queue */
#endif

#if THINKOS_ENABLE_DEBUG_FAULT
	uint32_t wq_fault; /* fault threads wait queue */
#endif

#if THINKOS_ENABLE_COMM
	uint32_t wq_comm_send;
	uint32_t wq_comm_recv;
#endif

	uint32_t wq_end[0]; /* end of queue list placeholder */

#if THINKOS_ENABLE_THREAD_STAT
	uint16_t th_stat[THINKOS_THREADS_MAX]; /* Per thread status */
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
#if THINKOS_ENABLE_DMCLOCK
		/* monitor timer */
		uint32_t dmclock;
#endif
	};
#endif

#if THINKOS_SEMAPHORE_MAX > 0
	uint32_t sem_val[THINKOS_SEMAPHORE_MAX];
#endif /* THINKOS_SEMAPHORE_MAX > 0 */

#if THINKOS_EVENT_MAX > 0
	struct {
		uint32_t pend; /* event set pending bitmap */
		uint32_t mask; /* event set mask */
	} ev[THINKOS_EVENT_MAX];
#endif /* THINKOS_EVENT_MAX > 0 */

#if THINKOS_FLAG_MAX > 0
	uint32_t flag[(THINKOS_FLAG_MAX + 31) / 32]; /* flag signal */
#endif /* THINKOS_FLAG_MAX > 0 */

#if THINKOS_GATE_MAX > 0
	/* gate bitmap, each gate takes two bits: 
	   1 - signal the gate is open or signaled to be open, 
	   2 - the gate is locked and can't be oepn. */
	uint32_t gate[(THINKOS_GATE_MAX + 15) / 16]; /* gates states */
#endif /* THINKOS_GATE_MAX > 0 */


#if THINKOS_MUTEX_MAX > 0
	int8_t lock[THINKOS_MUTEX_MAX];
#endif /* THINKOS_MUTEX_MAX > 0 */

#if THINKOS_IRQ_MAX > 0
	int8_t irq_th[THINKOS_IRQ_MAX];
#endif /* THINKOS_IRQ_MAX */

#if THINKOS_ENABLE_THREAD_ALLOC
	uint32_t th_alloc[1];
#endif

#if THINKOS_ENABLE_MUTEX_ALLOC
	uint32_t mutex_alloc[(THINKOS_MUTEX_MAX + 31) / 32];
#endif

#if THINKOS_ENABLE_COND_ALLOC
	uint32_t cond_alloc[(THINKOS_COND_MAX + 31) / 32];
#endif

#if THINKOS_ENABLE_SEM_ALLOC
	uint32_t sem_alloc[(THINKOS_SEMAPHORE_MAX + 31) / 32];
#endif

#if THINKOS_ENABLE_EVENT_ALLOC
	uint32_t ev_alloc[(THINKOS_EVENT_MAX + 31) / 32];
#endif

#if THINKOS_ENABLE_FLAG_ALLOC
	uint32_t flag_alloc[(THINKOS_FLAG_MAX + 31) / 32];
#endif

#if THINKOS_ENABLE_GATE_ALLOC
	uint32_t gate_alloc[(THINKOS_GATE_MAX + 31) / 32];
#endif

#if THINKOS_ENABLE_SCHED_DEBUG
	uint32_t sched_trace_req;
#endif

#if THINKOS_ENABLE_THREAD_INFO
	struct thinkos_thread_inf * th_inf[THINKOS_THREADS_MAX]; 
#endif
};


/* -------------------------------------------------------------------------- 
 * Base indexes for the wait queue list (wq_lst[])
 * --------------------------------------------------------------------------*/

#define THINKOS_WQ_READY 0

#define THINKOS_WQ_TMSHARE ((offsetof(struct thinkos_rt, wq_tmshare) \
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

#define THINKOS_GATE_BASE ((offsetof(struct thinkos_rt, wq_gate) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_JOIN_BASE ((offsetof(struct thinkos_rt, wq_join) \
						   - offsetof(struct thinkos_rt, wq_lst)) \
						  / sizeof(uint32_t))

#define THINKOS_WQ_CONSOLE_WR ((offsetof(struct thinkos_rt, wq_console_wr) \
								- offsetof(struct thinkos_rt, wq_lst)) \
							   / sizeof(uint32_t))

#define THINKOS_WQ_CONSOLE_RD ((offsetof(struct thinkos_rt, wq_console_rd) \
								- offsetof(struct thinkos_rt, wq_lst)) \
							   / sizeof(uint32_t))

#define THINKOS_WQ_COMM_RECV ((offsetof(struct thinkos_rt, wq_comm_recv) \
								- offsetof(struct thinkos_rt, wq_lst)) \
							   / sizeof(uint32_t))

#define THINKOS_WQ_COMM_SEND ((offsetof(struct thinkos_rt, wq_comm_send) \
								- offsetof(struct thinkos_rt, wq_lst)) \
							   / sizeof(uint32_t))

#define THINKOS_WQ_PAUSED ((offsetof(struct thinkos_rt, wq_paused) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_WQ_CANCELED ((offsetof(struct thinkos_rt, wq_canceled) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

#define THINKOS_WQ_FAULT ((offsetof(struct thinkos_rt, wq_fault) \
						   - offsetof(struct thinkos_rt, wq_lst)) \
						  / sizeof(uint32_t))

#define THINKOS_WQ_LST_END ((offsetof(struct thinkos_rt, wq_end) \
							 - offsetof(struct thinkos_rt, wq_lst)) \
							/ sizeof(uint32_t))

/* -------------------------------------------------------------------------- 
 * Static initialized kernel descriptors
 * --------------------------------------------------------------------------*/

#define THINKOS_MUTEX_DESC(_ID) (THINKOS_MUTEX_BASE + (_ID))
#define THINKOS_COND_DESC(_ID)  (THINKOS_COND_BASE + (_ID))
#define THINKOS_SEM_DESC(_ID)   (THINKOS_SEM_BASE + (_ID))
#define THINKOS_EVENT_DESC(_ID) (THINKOS_EVENT_BASE + (_ID))
#define THINKOS_FLAG_DESC(_ID)  (THINKOS_FLAG_BASE + (_ID))
#define THINKOS_GATE_DESC(_ID)  (THINKOS_GATE_BASE + (_ID))

/* -------------------------------------------------------------------------- 
 * Static thead references
 * --------------------------------------------------------------------------*/

#define THINKOS_THREAD_NULL (32)
#define THINKOS_THREAD_IDLE (THINKOS_THREADS_MAX)
#if THINKOS_ENABLE_THREAD_VOID 
#define THINKOS_THREAD_VOID (THINKOS_THREADS_MAX + 1)
#endif

#if 0
#define THINKOS_CYCCNT_SYS  (THINKOS_THREADS_MAX)
#define THINKOS_CYCCNT_IDLE (THINKOS_THREADS_MAX + 1)
#else
#define THINKOS_CYCCNT_IDLE (THINKOS_THREADS_MAX)
#endif

/* -------------------------------------------------------------------------- 
 * Thread initialization 
 * --------------------------------------------------------------------------*/
struct thinkos_thread_opt {
	uint16_t stack_size;
	uint8_t priority;
	uint8_t id: 7;
	uint8_t paused: 1;
};

struct thinkos_thread_init {
	void * task;
	void * arg;
	void * stack_ptr;
	struct thinkos_thread_opt opt;
	struct thinkos_thread_inf * inf;
};

/* -------------------------------------------------------------------------- 
 * Exception state
 * --------------------------------------------------------------------------*/

struct thinkos_except {
	struct thinkos_context ctx;
	uint32_t ret;
	uint32_t msp;
	uint32_t psp;
	uint32_t icsr;

	uint8_t ipsr;
	int8_t thread_id;
	uint8_t type;   /* exception type */
	uint8_t unroll; /* unroll count */
};

/* -------------------------------------------------------------------------- 
 * Idle thread
 * --------------------------------------------------------------------------*/

extern struct thinkos_rt thinkos_rt;

extern struct thinkos_except thinkos_except_buf;

extern uint32_t thinkos_except_stack[THINKOS_EXCEPT_STACK_SIZE / 4];

extern uint32_t * const thinkos_idle_stack_ptr;

extern uint32_t * const thinkos_obj_alloc_lut[];

extern const uint16_t thinkos_wq_base_lut[];

extern const char thinkos_type_name_lut[][6];

extern const char __xcpt_name_lut[16][12];

#ifdef __cplusplus
extern "C" {
#endif

void __attribute__((noreturn)) __thinkos_thread_exit(int code);

void __thinkos_thread_abort(int thread_id);

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

/* flags a deferred queued syscall */
static void inline __attribute__((always_inline)) __thinkos_defer_svc(void) {
	struct cm3_scb * scb = CM3_SCB;
	/* rise a pending systick interrupt */
	scb->icsr = SCB_ICSR_PENDSTSET;
}

static void inline __attribute__((always_inline)) __thinkos_ready_clr(void) {
	thinkos_rt.wq_ready = 0;
#if THINKOS_ENABLE_TIMESHARE
	thinkos_rt.wq_tmshare = 0;
#endif
#if (THINKOS_THREADS_MAX < 32) 
	/* put the IDLE thread in the ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, THINKOS_THREADS_MAX, 1);
#endif
}

static void inline __attribute__((always_inline)) __thinkos_suspend(int thread) {
#if (!THINKOS_ENABLE_TIMESHARE)
	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, thread, 0);  
#else
	uint32_t ready;
	uint32_t tmshare;

	do {
		ready = __ldrex(&thinkos_rt.wq_ready);
		tmshare = thinkos_rt.wq_tmshare;
		/* remove from the ready wait queue */
		ready &= ~(1 << thread);
		/* if the ready queue is empty, collect
		   the threads from the CPU wait queue */
#if ((THINKOS_THREADS_MAX) < 32) 
		if (ready == (1 << (THINKOS_THREADS_MAX))) {
#else
		if (ready == 0) {
#endif
				/* no more threads into the ready queue,
				   move the timeshare queue to the ready queue */
			ready |= tmshare;
			tmshare = 0;
		} 
	} while (__strex(&thinkos_rt.wq_ready, ready));

	thinkos_rt.wq_tmshare = tmshare;

#endif /* (!THINKOS_ENABLE_TIMESHARE) */
}

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
//	DCC_LOG2(LOG_TRACE, "wakeup %d from wq %d", th, wq);
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

static void inline __attribute__((always_inline)) 
__thinkos_wakeup_return(unsigned int wq, unsigned int th, int ret) {
//	DCC_LOG2(LOG_TRACE, "wakeup %d from wq %d", th, wq);
	/* insert the thread into ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
	/* remove from the wait queue */
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 0);  
#if THINKOS_ENABLE_TIMED_CALLS
	/* possibly remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);  
#endif
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[th] = 0;
#endif
	/* set the thread's return value */
	thinkos_rt.ctx[th]->r0 = ret;
}

static volatile inline uint32_t __attribute__((always_inline))
	__thinkos_ticks(void) {
#if THINKOS_ENABLE_CLOCK
	return thinkos_rt.ticks;
#else
#error "__thinkos_ticks() depends on THINKOS_ENABLE_CLOCK"
#endif
}

void thinkos_trace_rt(struct thinkos_rt * rt);

int thinkos_obj_type_get(unsigned int oid);

void __thinkos_bmp_init(uint32_t bmp[], int bits);

int __thinkos_bmp_alloc(uint32_t bmp[], int bits);

void thinkos_exception_init(void);

void thinkos_exception_dsr(struct thinkos_except * xcpt);

void thinkos_console_init(void);

void __thinkos_thread_init(unsigned int thread_id, uint32_t sp, 
						   void * task, void * arg);

bool __thinkos_thread_resume(unsigned int thread_id);

bool __thinkos_thread_pause(unsigned int thread_id);

bool __thinkos_thread_isalive(unsigned int thread_id);

bool __thinkos_thread_ispaused(unsigned int thread_id);

bool __thinkos_thread_isfaulty(unsigned int thread_id);

void __thinkos_thread_inf_set(unsigned int thread_id, 
							  const struct thinkos_thread_inf * inf);

void __thinkos_irq_disable_all(void);

void __thinkos_kill_all(void);

void __thinkos_pause_all(void);

void __thinkos_resume_all(void);

int __thinkos_thread_get(struct thinkos_rt * rt, 
						 struct thinkos_thread * st, 
						 unsigned int th);

int __thinkos_thread_getnext(int th);

bool __thinkos_active(void);

bool __thinkos_suspended(void);

void __thinkos_memcpy(void * __dst, const void * __src,  
					  unsigned int __len);

void __thinkos_memcpy32(void * __dst, const void * __src,  
						unsigned int __len);

void __thinkos_memset32(void * __dst, uint32_t __val, unsigned int __len);

void __thinkos_idle_init(void);

void __thinkos_reset(void);

void __console_reset(void);

void __exception_reset(void);

int __console_rx_pipe_ptr(uint8_t ** ptr);
void __console_rx_pipe_commit(unsigned int cnt); 

int __console_tx_pipe_ptr(uint8_t ** ptr);
void __console_tx_pipe_commit(unsigned int cnt);

/* -------------------------------------------------------------------------
 * Exception handling utility functions
 * ------------------------------------------------------------------------- */

void __xdump(struct thinkos_except * xcpt);

void __idump(const char * s, uint32_t ipsr);

void __tdump(void);

void __mpudump(void);

int __xcpt_next_active_irq(int this_irq);

void __xcpt_irq_disable_all(void);

void __xcpt_systick_int_disable(void);

void __xcpt_systick_int_enable(void);

void __thinkos_flag_give(uint32_t wq);
void __thinkos_flag_clr(uint32_t wq);
void __thinkos_flag_set(uint32_t wq);

void __thinkos_sem_post(uint32_t wq);

void __thinkos_ev_raise(uint32_t wq, int ev);

void __thinkos_gate_open(uint32_t wq);

void __thinkos_ev_info(unsigned int wq);

struct thinkos_except * __thinkos_except_buf(void);

/* -------------------------------------------------------------------------
 * Main thread exec
 * ------------------------------------------------------------------------- */

void __thinkos_exec(int thread_id, void (* func)(void *), 
					void * arg, bool paused);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __THINKOS_SYS_H__ */

