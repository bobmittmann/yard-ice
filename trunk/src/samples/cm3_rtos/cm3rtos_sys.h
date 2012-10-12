/*
 * File:     cm3rtos_sys.h
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

#ifndef __CM3RTOS_SYS_H__
#define __CM3RTOS_SYS_H__

#ifndef __CM3RTOS_SYS__
#error "Never use <cm3rtos_sys.h> directly; include <cm3rtos.h> instead."
#endif 

#ifndef __ASSEMBLER__

/* -------------------------------------------------------------------------- 
 * Thread context layout
 * --------------------------------------------------------------------------*/

struct cm3rtos_context {
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

#ifndef CM3RTOS_THREADS_MAX 
#define CM3RTOS_THREADS_MAX 8
#endif

#ifndef CM3RTOS_ENABLE_THREAD_ALLOC
#define CM3RTOS_ENABLE_THREAD_ALLOC 1
#endif

#ifndef CM3RTOS_ENABLE_TIMESHARE
#define CM3RTOS_ENABLE_TIMESHARE 1
#endif

#ifndef CM3RTOS_ENABLE_JOIN
#define CM3RTOS_ENABLE_JOIN 1
#endif

#ifndef CM3RTOS_ENABLE_WFI
#define CM3RTOS_ENABLE_WFI 1
#endif

#ifndef CM3RTOS_ENABLE_PAUSE
#define CM3RTOS_ENABLE_PAUSE 0
#endif

#ifndef CM3RTOS_ENABLE_CANCEL
#define CM3RTOS_ENABLE_CANCEL 1
#endif

#ifndef CM3RTOS_SCHED_LIMIT_MAX
#define CM3RTOS_SCHED_LIMIT_MAX 32
#endif

#ifndef CM3RTOS_SCHED_LIMIT_MIN
#define CM3RTOS_SCHED_LIMIT_MIN 1
#endif

#ifndef CM3RTOS_ENABLE_CLOCK
#define CM3RTOS_ENABLE_CLOCK 1
#endif

#ifndef CM3RTOS_MUTEX_MAX
#define CM3RTOS_MUTEX_MAX 8
#endif

#ifndef CM3RTOS_ENABLE_MUTEX_ALLOC
#define CM3RTOS_ENABLE_MUTEX_ALLOC 1
#endif

#ifndef CM3RTOS_WAIT_QUEUE_MAX
#define CM3RTOS_WAIT_QUEUE_MAX 8
#endif

#ifndef CM3RTOS_ENABLE_THREAD_STAT
#define CM3RTOS_ENABLE_THREAD_STAT 0
#endif

#ifndef CM3RTOS_ENABLE_TIMED_CALLS
#define CM3RTOS_ENABLE_TIMED_CALLS 1
#endif

/* timed calls depends on clock */
#if CM3RTOS_ENABLE_TIMED_CALLS
 #undef CM3RTOS_ENABLE_CLOCK
 #define CM3RTOS_ENABLE_CLOCK 1
#endif

/* timed calls and pause depends on thread status */
#if CM3RTOS_ENABLE_TIMED_CALLS || CM3RTOS_ENABLE_PAUSE
 #undef CM3RTOS_ENABLE_THREAD_STAT
 #define CM3RTOS_ENABLE_THREAD_STAT 1
#endif


/* -------------------------------------------------------------------------- 
 * Run Time RTOS block
 * --------------------------------------------------------------------------*/

struct cm3rtos_rt {
	/* This fields must be at the beginning of this structure 
	   and their order and sizes must not be changed.
	   This is critical for the scheduler operation. */
	struct {
		struct cm3rtos_context * ctx[32]; /* Thread context pointers */
		struct cm3rtos_context * idle_ctx; /* Idle thread context pointer */
		int32_t active; /* Active (current) thread index and schedule flag  */
	};

	struct {
		uint32_t wq_lst[0]; /* queue list placeholder */
		uint32_t wq_ready; /* ready threads queue: position 0 in the list */
#if CM3RTOS_ENABLE_JOIN
		uint32_t wq_canceled;
#endif
#if CM3RTOS_MUTEX_MAX > 0
		uint32_t wq_mutex[CM3RTOS_MUTEX_MAX];
#endif /* CM3RTOS_MUTEX_MAX > 0 */
#if CM3RTOS_ENABLE_JOIN
		uint32_t wq_join[CM3RTOS_THREADS_MAX];
#endif /* CM3RTOS_ENABLE_JOIN */
		uint32_t wq[CM3RTOS_WAIT_QUEUE_MAX];
#if CM3RTOS_ENABLE_CLOCK
		uint32_t wq_clock;
#endif
#if CM3RTOS_ENABLE_PAUSE
		uint32_t wq_paused;
#endif
		uint32_t wq_end[0]; /* end of queue placeholder */
	};

#if CM3RTOS_ENABLE_THREAD_STAT
	uint8_t th_stat[CM3RTOS_THREADS_MAX]; /* Per thread status */
#endif

#if CM3RTOS_ENABLE_TIMESHARE
	/* This fields are used for time sharing (round robin) schedule only */
	struct {
		int8_t sched_val[CM3RTOS_THREADS_MAX]; /* Thread schedule value */
		uint8_t sched_pri[CM3RTOS_THREADS_MAX]; /* Thread schedule priority */
		uint32_t wq_tmshare; /* Threads waiting for time share cycle */
		uint32_t sched_limit; /* Limiting time for an active thread */
	};
#endif

#if CM3RTOS_ENABLE_CLOCK
	struct {
		/* This fields are used for time wait (e.g. sleep()) */
		uint32_t clock[CM3RTOS_THREADS_MAX];
		uint32_t ticks;
	};
#endif

#if CM3RTOS_MUTEX_MAX > 0
	uint8_t lock[CM3RTOS_MUTEX_MAX];
#endif /* CM3RTOS_MUTEX_MAX > 0 */

#if CM3RTOS_ENABLE_THREAD_ALLOC
	uint32_t th_alloc;
#endif

#if CM3RTOS_ENABLE_MUTEX_ALLOC
	uint32_t mutex_alloc;
#endif
//	uint32_t wq_alloc;
} __attribute__ ((aligned (8)));
/* -------------------------------------------------------------------------- 
 * Thread initialization 
 * --------------------------------------------------------------------------*/

struct cm3rtos_thread_opt {
	uint8_t priority;
	uint8_t id;
	uint8_t f_paused: 1;
	uint8_t reserved;
};

struct cm3rtos_thread_init {
	void * task;
	void * arg;
	void * stack_ptr;
	uint32_t stack_size;
	struct cm3rtos_thread_opt opt;
};


/* -------------------------------------------------------------------------- 
 * Idle thread
 * --------------------------------------------------------------------------*/
struct cm3rtos_idle {
	union {
		struct cm3rtos_context ctx;
		struct {
			uint32_t r[12];
			uint32_t val;
			uint32_t * ptr;
		} snapshot;
	};
} __attribute__ ((aligned (8)));

struct cm3rtos_except_and_idle {
	uint32_t res[64 - 12];
	union {
		struct cm3rtos_context ctx;
		struct {
			uint32_t r[12];
			uint32_t val;
			uint32_t * ptr;
		} snapshot;
	};
} __attribute__ ((aligned (8)));

extern struct cm3rtos_except_and_idle cm3rtos_idle;

extern struct cm3rtos_rt cm3rtos_rt;

#ifdef __cplusplus
extern "C" {
#endif

void __attribute__((noreturn)) cm3rtos_thread_exit(int code);

void __attribute__((noreturn)) cm3rtos_idle_task(void);

/* Moves the current MSP to PSP and 
   assert a new MSP stack top */
void cm3_msp_init(uint64_t * stack_top);

/* -------------------------------------------------------------------------- 
 * Support Functions
 * --------------------------------------------------------------------------*/

/* read a bit from a bitmap */
static int inline bmp_bit_rd(void * bmp, unsigned int bit)
{
	/* get the bitband bit address */
	uint32_t * bit_addr = CM3_BITBAND_MEM(bmp, 0);  
	return bit_addr[bit];
}

/* write a bit into a bitmap */
static void inline bmp_bit_wr(void * bmp, unsigned int bit, int val)
{
	/* get the bitband bit address */
	uint32_t * bit_addr = CM3_BITBAND_MEM(bmp, 0);  
	bit_addr[bit] = val;
}


/* set a bit in a bit map atomically */
static void inline bmp_bit_set(void * bmp, unsigned int bit)
{
	/* get the bitband bit address */
	uint32_t * bit_addr = CM3_BITBAND_MEM(bmp, 0);  
	bit_addr[bit] = 1;
}

/* clear a bit in a bit map atomically */
static void inline bmp_bit_clr(void * bmp, unsigned int bit)
{
	/* get the bitband bit address */
	uint32_t * bit_addr = CM3_BITBAND_MEM(bmp, 0);  
	bit_addr[bit] = 0;
}

static int inline bmp_ffs32(void * bmp)
{
	return cm3_clz(*(uint32_t *)bmp);
}

static inline int cm3rtos_alloc_lo(uint32_t * ptr, int start) {
	int idx;
	/* Look for an empty bit MSB first */
	idx = cm3_clz(cm3_rbit(~(*ptr >> start))) + start;
	if (idx >= 32)
		return -1;
	/* Mark as used */
	bmp_bit_wr(ptr, idx, 1);  
	return idx;
}

static inline int cm3rtos_alloc_hi(uint32_t * ptr, int start) {
	int idx;
	/* Look for an empty bit LSB first */
	idx = start - cm3_clz(~(*ptr << (31 - start)));
	if (idx < 0)
		return -1;
	/* Mark as used */
	bmp_bit_wr(ptr, idx, 1);  
	return idx;
}

/* flags a defered execution of the scheduller */
static void inline __attribute__((always_inline)) __cm3rtos_defer_sched(void) {
	struct cm3_scb * scb = CM3_SCB;
	/* rise a pending service interrupt */
	scb->icsr = SCB_ICSR_PENDSVSET;
}

static void inline __attribute__((always_inline)) __cm3rtos_wait(void) {
	/* clear the schedule flag */
	bmp_bit_wr(&cm3rtos_rt.active, 0, 0);  
#if CM3RTOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
	if (cm3rtos_rt.wq_ready == 0) {
		cm3rtos_rt.wq_ready |= cm3rtos_rt.wq_tmshare;
		cm3rtos_rt.wq_tmshare = 0;
	}
#endif
	/* signal the scheduler ... */
	__cm3rtos_defer_sched();
}

static int inline __attribute__((always_inline)) __wq_idx(uint32_t * ptr) {
	return ptr - cm3rtos_rt.wq_lst;
}

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __CM3RTOS_SYS_H__ */

