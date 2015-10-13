/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
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

/** 
 * @file sys/os.h
 * @brief YARD-ICE 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_OS_H__
#define __SYS_OS_H__

#include <errno.h>

#define ENABLE_THINKOS 1

#if ENABLE_THINKOS
#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>
#endif

#define __OS_PRIORITY_HIGHEST 0
#define __OS_PRIORITY_HIGH    1
#define __OS_PRIORITY_MEDIUM  2
#define __OS_PRIORITY_LOW     4
#define __OS_PRIORITY_LOWEST  8

#ifdef __cplusplus
extern "C" {
#endif

#if ENABLE_THINKOS
#define __OS_ERROR THINKOS_ERROR
#define __OS_TIMEOUT THINKOS_ETIMEDOUT
#define __OS_EINVAL THINKOS_EINVAL
#define __OS_EAGAIN THINKOS_EAGAIN
#define __OS_EDEADLK THINKOS_EDEADLK
#define __OS_EPERM THINKOS_EPERM
#define __OS_ENOSYS THINKOS_ENOSYS
#endif


typedef int32_t __os_mutex_t;
typedef int32_t __os_cond_t;
typedef int32_t __os_sem_t;

static inline void __os_sleep(unsigned int msec) {
#if ENABLE_THINKOS
	thinkos_sleep(msec);
#else
	mdelay(msec);
#endif
}

extern inline void exit(int status) {
#if ENABLE_THINKOS
//	thinkos_cancel(thinkos_thread_self(), status);
//	thinkos_yield();
#else
	for(;;);
#endif
}

/********************************************************************************* 
 * Mutexes
 */

static inline int __os_mutex_alloc(void) {
#if ENABLE_THINKOS
	return thinkos_mutex_alloc();
#else
	return 0;
#endif
}

static inline int __os_mutex_free(int mutex) {
#if ENABLE_THINKOS
	return thinkos_mutex_free(mutex);
#else
	return 0;
#endif
}

static inline int __os_mutex_lock(int mutex) {
#if ENABLE_THINKOS
	return thinkos_mutex_lock(mutex);
#else
	return 0;
#endif
}

static inline int __os_mutex_trylock(int mutex) {
#if ENABLE_THINKOS
	return thinkos_mutex_trylock(mutex);
#else
	return 0;
#endif
}

static inline int __os_mutex_unlock(int mutex) {
#if ENABLE_THINKOS
	return thinkos_mutex_unlock(mutex);
#else
	return 0;
#endif
}

/********************************************************************************* 
 * Conditional variables 
 */

static inline int __os_cond_alloc(void) {
#if ENABLE_THINKOS
	return thinkos_cond_alloc();
#else
	return 0;
#endif
}

static inline int __os_cond_free(int cond) {
#if ENABLE_THINKOS
	return thinkos_cond_free(cond);
#else
	return 0;
#endif
}

static inline int __os_cond_wait(int cond, int mutex) {
#if ENABLE_THINKOS
	return thinkos_cond_wait(cond, mutex);
#else
	return 0;
#endif
}

static inline int __os_cond_timedwait(int cond, int mutex, unsigned int tmo) {
#if ENABLE_THINKOS
	return thinkos_cond_timedwait(cond, mutex, tmo);
#else
	return 0;
#endif
}

static inline int __os_cond_signal(int cond) {
#if ENABLE_THINKOS
	return thinkos_cond_signal(cond);
#else
	return 0;
#endif
}

static inline int __os_cond_broadcast(int cond) {
#if ENABLE_THINKOS
	return thinkos_cond_broadcast(cond);
#else
	return 0;
#endif
}

/********************************************************************************* 
 * Semaphores
 */

static inline int __os_sem_alloc(int val) {
#if ENABLE_THINKOS
	return thinkos_sem_alloc(val);
#else
	return 0;
#endif
}

static inline int __os_sem_init(int sem, unsigned int val) {
#if ENABLE_THINKOS
	return thinkos_sem_init(sem, val);
#else
	return 0;
#endif
}

static inline int __os_sem_free(int sem) {
#if ENABLE_THINKOS
	return thinkos_sem_free(sem);
#else
	return 0;
#endif
}

static inline int __os_sem_post(int sem) {
#if ENABLE_THINKOS
	return thinkos_sem_post(sem);
#else
	return 0;
#endif
}

static inline int __os_sem_wait(int sem) {
#if ENABLE_THINKOS
	return thinkos_sem_wait(sem);
#else
	return 0;
#endif
}

static inline int __os_sem_timedwait(int sem, unsigned int ms) {
#if ENABLE_THINKOS
	return thinkos_sem_timedwait(sem, ms);
#else
	return 0;
#endif
}

static inline int __os_sem_trywait(int sem) {
#if ENABLE_THINKOS
	return thinkos_sem_trywait(sem);
#else
	return 0;
#endif
}

static inline void __attribute__((always_inline)) __os_int_wait(int irq) {
#if ENABLE_THINKOS
	thinkos_irq_wait(irq);
#else
#endif
}

/********************************************************************************* 
 * Clock 
 */

static inline uint32_t __attribute__((always_inline)) __os_ms_ticks() {
#if ENABLE_THINKOS
	return thinkos_clock();
#else
	return 0;
#endif
}

/********************************************************************************* 
 * Threads
 */

static inline int __os_thread_create(int (* task)(void *), void * arg, 
									  void * stack_ptr, 
									  unsigned int stack_size,
									  int priority) {
	int id = (priority <= __OS_PRIORITY_HIGH) ? 0 : 32;
	return thinkos_thread_create(task, arg, stack_ptr, 
								 THINKOS_OPT_PRIORITY(priority) |
								 THINKOS_OPT_ID(id) | 
								 THINKOS_OPT_STACK_SIZE(stack_size));
}

static inline int __os_thread_self(void) {
#if ENABLE_THINKOS
	return thinkos_thread_self();
#else
	return 0;
#endif
}

static inline int __os_thread_cancel(int th, int code) {
#if ENABLE_THINKOS
	return thinkos_cancel(th, code);
#else
	return 0;
#endif
}

static inline int __os_thread_join(int th) {
#if ENABLE_THINKOS
	return thinkos_join(th);
#else
	return 0;
#endif
}


#ifdef __cplusplus
}
#endif

#endif /* __SYS_OS_H__ */

