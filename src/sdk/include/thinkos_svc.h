/* 
 * thikos_svc.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */


#ifndef __THINKOS_SVC_H__
#define __THINKOS_SVC_H__

/* -------------------------------------------------------------------------- 
 * Service numbers
 * --------------------------------------------------------------------------*/

#define THINKOS_THREAD_SELF      0
#define THINKOS_THREAD_CREATE    1
   
#define THINKOS_CLOCK            2
#define THINKOS_ALARM            3
#define THINKOS_SLEEP            4

#define THINKOS_MUTEX_LOCK       5
#define THINKOS_MUTEX_TRYLOCK    6
#define THINKOS_MUTEX_TIMEDLOCK  7
#define THINKOS_MUTEX_UNLOCK     8

#define THINKOS_SEM_INIT         9
#define THINKOS_SEM_WAIT        10
#define THINKOS_SEM_TRYWAIT     11
#define THINKOS_SEM_TIMEDWAIT   12
#define THINKOS_SEM_POST        13

#define THINKOS_COND_WAIT       14
#define THINKOS_COND_TIMEDWAIT  15
#define THINKOS_COND_SIGNAL     16
#define THINKOS_COND_BROADCAST  17

#define THINKOS_FLAG_TAKE       18
#define THINKOS_FLAG_TIMEDTAKE  19
#define THINKOS_FLAG_GIVE       20

#define THINKOS_FLAG_VAL        21
#define THINKOS_FLAG_CLR        22
#define THINKOS_FLAG_SET        23
#define THINKOS_FLAG_WATCH      24
#define THINKOS_FLAG_TIMEDWATCH 25

#define THINKOS_GATE_WAIT       26
#define THINKOS_GATE_TIMEDWAIT  27
#define THINKOS_GATE_EXIT       28
#define THINKOS_GATE_OPEN       29
#define THINKOS_GATE_CLOSE      30

#define THINKOS_EVENT_WAIT      31
#define THINKOS_EVENT_TIMEDWAIT 32
#define THINKOS_EVENT_RAISE     33
#define THINKOS_EVENT_MASK      34
#define THINKOS_EVENT_CLEAR     35

#define THINKOS_CONSOLE         36

#define THINKOS_IRQ_WAIT        37
#define THINKOS_IRQ_REGISTER    38
#define THINKOS_IRQ_CTL         39

#define THINKOS_MUTEX_ALLOC     40
#define THINKOS_MUTEX_FREE      41

#define THINKOS_SEM_ALLOC       42
#define THINKOS_SEM_FREE        43

#define THINKOS_COND_ALLOC      44
#define THINKOS_COND_FREE       45

#define THINKOS_FLAG_ALLOC      46
#define THINKOS_FLAG_FREE       47

#define THINKOS_EVENT_ALLOC     48
#define THINKOS_EVENT_FREE      49

#define THINKOS_GATE_ALLOC      50
#define THINKOS_GATE_FREE       51

#define THINKOS_JOIN            52
#define THINKOS_PAUSE           53
#define THINKOS_RESUME          54

#define THINKOS_CANCEL          55
#define THINKOS_EXIT            56
#define THINKOS_TERMINATE       57

#define THINKOS_CTL             58

#define THINKOS_COMM            59

#define THINKOS_DBGMON          60

#define THINKOS_ESCALATE        61

#define CONSOLE_WRITE             0
#define CONSOLE_READ              1
#define CONSOLE_TIMEDREAD         2
#define CONSOLE_OPEN              3
#define CONSOLE_CLOSE             4
#define CONSOLE_DRAIN             5
#define CONSOLE_IOCTL             6

#define COMM_SEND                 0
#define COMM_RECV                 1

#define THINKOS_CTL_ABORT         0
#define THINKOS_CTL_UDELAY_FACTOR 1
#define THINKOS_CTL_CLOCKS        2
#define THINKOS_CTL_SNAPSHOT      3
#define THINKOS_CTL_TRACE         4

#define THINKOS_IRQ_DISABLE       0
#define THINKOS_IRQ_ENABLE        1
#define THINKOS_IRQ_PRIORITY_SET  2
#define THINKOS_IRQ_SVC_SET       3

#define DBGMON_SIGNAL_IDLE        0

#include <arch/cortex-m3.h>

#ifndef __ASSEMBLER__

/* ------------------------------------------------------------------------- 
 * C service call macros 
 * ------------------------------------------------------------------------- */

#define __SVC_CALL(N) ( { register int ret asm("r0"); \
	asm volatile ("svc " #N "\n" : "=r"(ret) : : ); \
	ret; } )

#define __SVC_CALL1(N, A1) ( { register int ret asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : "0"(r0) : ); \
	ret; } )

#define __SVC_CALL2(N, A1, A2) ( { register int ret asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : \
				  "0"(r0), "r"(r1) : ); \
	ret; } )

#define __SVC_CALL3(N, A1, A2, A3) ( { register int ret asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	register int r2 asm("r2") = (int)A3; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : \
				  "0"(r0), "r"(r1), "r"(r2) : ); \
	ret; } )

#define __SVC_CALL4(N, A1, A2, A3, A4) ( { register int ret asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	register int r2 asm("r2") = (int)A3; \
	register int r3 asm("r3") = (int)A4; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : \
				  "0"(r0), "r"(r1), "r"(r2), "r"(r3) : ); \
	ret; } )

#define __SVC_CALL5(N, A1, A2, A3, A4, A5) ( { register int ret asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	register int r2 asm("r2") = (int)A3; \
	register int r3 asm("r3") = (int)A4; \
	register int r12 asm("r12") = (int)A5; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : \
				  "0"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r12) : ); \
	ret; } )


#define THINKOS_SVC(N) __SVC_CALL(N)

#define THINKOS_SVC1(N, A1) __SVC_CALL1(N, (A1))

#define THINKOS_SVC2(N, A1, A2) __SVC_CALL2(N, (A1), (A2))

#define THINKOS_SVC3(N, A1, A2, A3) __SVC_CALL3(N, (A1), (A2), (A3))

#define THINKOS_SVC4(N, A1, A2, A3, A4) __SVC_CALL4(N, (A1), (A2), (A3), (A4))

#define THINKOS_SVC5(N, A1, A2, A3, A4, A5) __SVC_CALL5(N, (A1), (A2), \
														(A3), (A4), (A5))

#ifdef __cplusplus
extern "C" {
#endif

static inline int const __attribute__((always_inline)) 
	thinkos_thread_self(void) {
	return THINKOS_SVC(THINKOS_THREAD_SELF);
}

static inline int __attribute__((always_inline)) 
	thinkos_thread_create(int (* task)(void *), 
						  void * arg, void * stack_ptr,
						  unsigned int opt) {
	return THINKOS_SVC5(THINKOS_THREAD_CREATE, task, arg, 
						stack_ptr, opt, 0);
}

static inline int __attribute__((always_inline)) 
	thinkos_thread_create_inf(int (* task)(void *), void * arg, 
							  const struct thinkos_thread_inf * inf) {
	return THINKOS_SVC5(THINKOS_THREAD_CREATE, task, arg, 
						inf->stack_ptr, inf->opt, inf);
}

static inline int 
__attribute__((always_inline)) thinkos_cancel(unsigned int id, int code) {
	return THINKOS_SVC2(THINKOS_CANCEL, id, code);
}

static inline int 
__attribute__((always_inline)) thinkos_exit(int code) {
	return THINKOS_SVC1(THINKOS_EXIT, code);
}

static inline int 
__attribute__((always_inline)) thinkos_terminate(int code, unsigned int id) {
	return THINKOS_SVC2(THINKOS_TERMINATE, code, id);
}

static inline int 
__attribute__((always_inline)) thinkos_thread_abort(unsigned int id) {
	return THINKOS_SVC2(THINKOS_TERMINATE, THINKOS_THREAD_ABORTED, id);
}

static inline int 
__attribute__((always_inline)) thinkos_join(unsigned int id) {
	return THINKOS_SVC1(THINKOS_JOIN, id);
}

static inline int 
__attribute__((always_inline)) thinkos_pause(unsigned int id) {
	return THINKOS_SVC1(THINKOS_PAUSE, id);
}

static inline int 
__attribute__((always_inline)) thinkos_resume(unsigned int id) {
	return THINKOS_SVC1(THINKOS_RESUME, id);
}

static inline int
__attribute__((always_inline)) thinkos_sleep(unsigned int ms) {
	return THINKOS_SVC1(THINKOS_SLEEP, ms);
}

static inline int
__attribute__((always_inline)) thinkos_alarm(uint32_t clk) {
	return THINKOS_SVC1(THINKOS_ALARM, clk);
}

static inline uint32_t __attribute__((always_inline)) thinkos_clock(void) {
	return THINKOS_SVC(THINKOS_CLOCK);
}

static inline int __attribute__((always_inline)) thinkos_mutex_alloc(void) {
	return THINKOS_SVC(THINKOS_MUTEX_ALLOC);
}

static inline int __attribute__((always_inline)) 
thinkos_mutex_free(int mutex) {
	return THINKOS_SVC1(THINKOS_MUTEX_FREE, mutex);
}

static inline int __attribute__((always_inline)) 
thinkos_mutex_lock(int mutex) {
	return THINKOS_SVC1(THINKOS_MUTEX_LOCK, mutex);
}

static inline int __attribute__((always_inline)) 
thinkos_mutex_trylock(int mutex) {
	return THINKOS_SVC1(THINKOS_MUTEX_TRYLOCK, mutex);
}

static inline int __attribute__((always_inline)) 
thinkos_mutex_timedlock(int mutex, unsigned int ms) {
	return THINKOS_SVC2(THINKOS_MUTEX_TIMEDLOCK, mutex, ms);
}

static inline int __attribute__((always_inline)) 
thinkos_mutex_unlock(int mutex) {
	return THINKOS_SVC1(THINKOS_MUTEX_UNLOCK, mutex);
}


static inline int __attribute__((always_inline)) thinkos_cond_alloc(void) {
	return THINKOS_SVC(THINKOS_COND_ALLOC);
}

static inline int __attribute__((always_inline)) thinkos_cond_free(int cond) {
	return THINKOS_SVC1(THINKOS_COND_FREE, cond);
}

static inline int __attribute__((always_inline)) 
thinkos_cond_wait(int cond, int mutex) {
	int ret;
	if ((ret = THINKOS_SVC2(THINKOS_COND_WAIT, 
							cond, mutex)) >= THINKOS_ETIMEDOUT) {
		THINKOS_SVC1(THINKOS_MUTEX_LOCK, mutex);
	}
	return ret;
}

static inline int __attribute__((always_inline)) 
thinkos_cond_timedwait(int cond, int mutex, unsigned int ms) {
	int ret;
	if ((ret = THINKOS_SVC3(THINKOS_COND_TIMEDWAIT, 
							cond, mutex, ms)) >= THINKOS_ETIMEDOUT) { 
		THINKOS_SVC1(THINKOS_MUTEX_LOCK, mutex);
	}
	return ret;
}

static inline int __attribute__((always_inline)) 
thinkos_cond_signal(int cond) {
	return THINKOS_SVC1(THINKOS_COND_SIGNAL, cond);
}

static inline int __attribute__((always_inline)) 
thinkos_cond_broadcast(int cond) {
	return THINKOS_SVC1(THINKOS_COND_BROADCAST, cond);
}

static inline int __attribute__((always_inline)) 
thinkos_sem_alloc(unsigned int value) {
	return THINKOS_SVC1(THINKOS_SEM_ALLOC, value);
}

static inline int __attribute__((always_inline)) thinkos_sem_free(int sem) {
	return THINKOS_SVC1(THINKOS_SEM_FREE, sem);
}

static inline int __attribute__((always_inline)) 
	thinkos_sem_init(int sem, unsigned int value) {
	return THINKOS_SVC2(THINKOS_SEM_INIT, sem, value);
}

static inline int __attribute__((always_inline)) thinkos_sem_wait(int sem) {
	return THINKOS_SVC1(THINKOS_SEM_WAIT, sem);
}

static inline int __attribute__((always_inline)) thinkos_sem_trywait(int sem) {
	return THINKOS_SVC1(THINKOS_SEM_TRYWAIT, sem);
}

static inline int __attribute__((always_inline))
thinkos_sem_timedwait(int sem, unsigned int ms) {
	return THINKOS_SVC2(THINKOS_SEM_TIMEDWAIT, sem, ms);
}

static inline int __attribute__((always_inline)) thinkos_sem_post(int sem) {
	return THINKOS_SVC1(THINKOS_SEM_POST, sem);
}

static inline void  __attribute__((always_inline)) thinkos_sem_post_i(int sem) {
	uintptr_t * except = (uintptr_t *)(0);
	void (* sem_post_i)(int) = (void *)except[7];
	sem_post_i(sem);
}

/* --------------------------------------------------------------------------
 *  Event sets
 * --------------------------------------------------------------------------*/

static inline int __attribute__((always_inline)) thinkos_ev_alloc(void) {
	return THINKOS_SVC(THINKOS_EVENT_ALLOC);
}

static inline int __attribute__((always_inline)) thinkos_ev_free(int set) {
	return THINKOS_SVC1(THINKOS_EVENT_FREE, set);
}

static inline int __attribute__((always_inline)) thinkos_ev_wait(int set) {
	return THINKOS_SVC1(THINKOS_EVENT_WAIT, set);
}

static inline int __attribute__((always_inline)) thinkos_ev_timedwait(
	int set, unsigned int ms) {
	return THINKOS_SVC2(THINKOS_EVENT_TIMEDWAIT, set, ms);
}

static inline int __attribute__((always_inline)) thinkos_ev_raise(
	int set, int ev) {
	return THINKOS_SVC2(THINKOS_EVENT_RAISE, set, ev);
}

static inline int __attribute__((always_inline)) thinkos_ev_mask(
	int set, int ev, int val) {
	return THINKOS_SVC3(THINKOS_EVENT_MASK, set, ev, val);
}

static inline int __attribute__((always_inline)) thinkos_ev_clear(
	int set, int ev) {
	return THINKOS_SVC2(THINKOS_EVENT_CLEAR, set, ev);
}

static inline void __attribute__((always_inline)) thinkos_ev_raise_i(
	int set, int ev) {
	uintptr_t * except = (uintptr_t *)(0);
	void (* ev_raise_i)(int, int) = (void *)except[9];
	ev_raise_i(set, ev);
}

/* ---------------------------------------------------------------------------
   Flags
  ----------------------------------------------------------------------------*/

static inline int __attribute__((always_inline)) thinkos_flag_alloc(void) {
	return THINKOS_SVC(THINKOS_FLAG_ALLOC);
}

static inline int __attribute__((always_inline)) thinkos_flag_free(int flag) {
	return THINKOS_SVC1(THINKOS_FLAG_FREE, flag);
}

static inline int __attribute__((always_inline)) thinkos_flag_set(int flag) {
	return THINKOS_SVC1(THINKOS_FLAG_SET, flag);
}

static inline int __attribute__((always_inline)) thinkos_flag_clr(int flag) {
	return THINKOS_SVC1(THINKOS_FLAG_CLR, flag);
}

static inline int __attribute__((always_inline)) thinkos_flag_val(int flag) {
	return THINKOS_SVC1(THINKOS_FLAG_VAL, flag);
}

static inline int __attribute__((always_inline)) thinkos_flag_watch(int flag) {
	return THINKOS_SVC1(THINKOS_FLAG_WATCH, flag);
}

static inline int __attribute__((always_inline)) 
	thinkos_flag_timedwatch(int flag, unsigned int ms) {
	return THINKOS_SVC2(THINKOS_FLAG_TIMEDWATCH, flag, ms);
}

static inline int __attribute__((always_inline)) thinkos_flag_give(int flag) {
	return THINKOS_SVC1(THINKOS_FLAG_GIVE, flag);
}

static inline int __attribute__((always_inline)) thinkos_flag_take(int flag) {
	return THINKOS_SVC1(THINKOS_FLAG_TAKE, flag);
}

static inline int __attribute__((always_inline)) 
	thinkos_flag_timedtake(int flag, unsigned int ms) {
	return THINKOS_SVC2(THINKOS_FLAG_TIMEDTAKE, flag, ms);
}

static inline void __attribute__((always_inline)) 
	thinkos_flag_give_i(int flag) {
	uintptr_t * except = (uintptr_t *)(0);
	void (* flag_give_i)(int) = (void *)except[10];
	flag_give_i(flag);
}

/* ---------------------------------------------------------------------------
   Gates
   ---------------------------------------------------------------------------*/

static inline int __attribute__((always_inline)) thinkos_gate_alloc(void) {
	return THINKOS_SVC(THINKOS_GATE_ALLOC);
}

static inline int __attribute__((always_inline)) thinkos_gate_free(int gate) {
	return THINKOS_SVC1(THINKOS_GATE_FREE, gate);
}


static inline int __attribute__((always_inline)) 
	thinkos_gate_open(int gate) {
	return THINKOS_SVC1(THINKOS_GATE_OPEN, gate);
}

static inline int __attribute__((always_inline)) 
	thinkos_gate_close(int gate) {
	return THINKOS_SVC1(THINKOS_GATE_CLOSE, gate);
}

static inline int __attribute__((always_inline)) 
	thinkos_gate_exit(int gate, unsigned int open) {
	return THINKOS_SVC2(THINKOS_GATE_EXIT, gate, open);
}

static inline int __attribute__((always_inline)) 
	thinkos_gate_wait(int gate) {
	return THINKOS_SVC1(THINKOS_GATE_WAIT, gate);
}

static inline int __attribute__((always_inline)) 
	thinkos_gate_timedwait(int gate, unsigned int ms) {
	return THINKOS_SVC2(THINKOS_GATE_TIMEDWAIT, gate, ms);
}

static inline void __attribute__((always_inline)) 
	thinkos_gate_open_i(int gate) {
	uintptr_t * except = (uintptr_t *)(0);
	void (* __gate_open_i)(uint32_t) = (void *)except[13];
	__gate_open_i(gate);
}

/* ---------------------------------------------------------------------------
   IRQ
   ---------------------------------------------------------------------------*/

static inline int __attribute__((always_inline)) thinkos_irq_wait(int irq) {
	return THINKOS_SVC1(THINKOS_IRQ_WAIT, irq);
}

static inline int __attribute__((always_inline)) 
	thinkos_irq_register(int irq, int pri, void (* isr)(void)) {
	return THINKOS_SVC3(THINKOS_IRQ_REGISTER, irq, pri, isr);
}

static inline int __attribute__((always_inline)) 
	thinkos_irq_enable(int irq) {
	return THINKOS_SVC2(THINKOS_IRQ_CTL, THINKOS_IRQ_ENABLE, irq);
}

static inline int __attribute__((always_inline)) 
	thinkos_irq_disable(int irq) {
	return THINKOS_SVC2(THINKOS_IRQ_CTL, THINKOS_IRQ_DISABLE, irq);
}

static inline int __attribute__((always_inline)) 
	thinkos_irq_priority_set(int irq, int pri) {
	return THINKOS_SVC3(THINKOS_IRQ_CTL, 
						THINKOS_IRQ_PRIORITY_SET, irq, pri);
}
/* ---------------------------------------------------------------------------
   Console
   ---------------------------------------------------------------------------*/

static inline int __attribute__((always_inline)) 
thinkos_console_write(const void * buf, unsigned int len) {
	return THINKOS_SVC3(THINKOS_CONSOLE, CONSOLE_WRITE, buf, len);
}

static inline int __attribute__((always_inline)) 
thinkos_console_ioctl(unsigned int ioctl, void * arg, unsigned int size) {
	return THINKOS_SVC4(THINKOS_CONSOLE, CONSOLE_IOCTL, ioctl, arg, size);
}

static inline int __attribute__((always_inline)) 
thinkos_console_read(void * buf, unsigned int len) {
	return THINKOS_SVC3(THINKOS_CONSOLE, CONSOLE_READ, buf, len);
}

static inline int __attribute__((always_inline)) 
thinkos_console_timedread(void * buf, unsigned int len, unsigned int ms) {
	return THINKOS_SVC4(THINKOS_CONSOLE, CONSOLE_TIMEDREAD, buf, len, ms);
}

static inline int __attribute__((always_inline)) 
thinkos_console_close(void) {
	return THINKOS_SVC1(THINKOS_CONSOLE, CONSOLE_CLOSE);
}

static inline int __attribute__((always_inline)) 
thinkos_console_drain(void) {
	return THINKOS_SVC1(THINKOS_CONSOLE, CONSOLE_DRAIN);
}

/* ---------------------------------------------------------------------------
   OS Monitor and Control 
   ---------------------------------------------------------------------------*/

static inline int __attribute__((always_inline)) 
	thinkos_clocks(uint32_t * clk[]) {
		return THINKOS_SVC2(THINKOS_CTL, THINKOS_CTL_CLOCKS, clk);
	}

static inline int __attribute__((always_inline)) 
	thinkos_udelay_factor(int32_t * factor) {
		return THINKOS_SVC2(THINKOS_CTL, THINKOS_CTL_UDELAY_FACTOR, factor);
	}

static inline void __attribute__((always_inline, noreturn)) 
	thinkos_abort(void) {
		for (;;) {
		THINKOS_SVC1(THINKOS_CTL, THINKOS_CTL_ABORT);
		}
	}

static inline int __attribute__((always_inline))
	thinkos_trace(const char * msg) {
		return THINKOS_SVC2(THINKOS_CTL, THINKOS_CTL_TRACE, msg);
	}

static inline int __attribute__((always_inline)) 
	thinkos_rt_snapshot(void * rt) {
		return THINKOS_SVC2(THINKOS_CTL, THINKOS_CTL_SNAPSHOT, rt);
	}

/* ---------------------------------------------------------------------------
   Communication channel
   ---------------------------------------------------------------------------*/

static inline int __attribute__((always_inline)) 
thinkos_comm_send(uint32_t hdr, const void * buf, unsigned int len) {
	return THINKOS_SVC4(THINKOS_COMM, COMM_SEND, hdr, buf, len);
}

static inline int __attribute__((always_inline)) 
thinkos_comm_recv(uint32_t * hdr, void * buf, unsigned int len) {
	return THINKOS_SVC4(THINKOS_COMM, COMM_RECV, hdr, buf, len);
}

/* ---------------------------------------------------------------------------
   Other
   ---------------------------------------------------------------------------*/

static inline void thinkos_yield(void) {
	CM3_SCB->icsr = SCB_ICSR_PENDSVSET; /* PendSV rise */
	asm volatile ("dsb\n"); /* Data synchronization barrier */
}

static inline void thinkos_dbgmon(unsigned int sig) {
	THINKOS_SVC1(THINKOS_DBGMON, sig);
}

static inline int thinkos_escalate(int (* call)(void *), void * arg) {
	return THINKOS_SVC2(THINKOS_ESCALATE, call, arg);
}

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __THINKOS_SVC_H__ */

