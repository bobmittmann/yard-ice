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

/** 
 * @file thinkos.h
 * @brief ThikOS API
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifndef __THINKOS_H__
#define __THINKOS_H__

enum thinkos_err {
	THINKOS_OK        =  0, /**< No error */
	THINKOS_ETIMEDOUT = -1, /**< System call timed out */
	THINKOS_EINTR     = -2, /**< System call interrupted out */
	THINKOS_EINVAL    = -3, /**< Invalid argument */
	THINKOS_EAGAIN    = -4, /**< Non blocking call failed */
	THINKOS_EDEADLK   = -5, /**< Deadlock condition detected */
	THINKOS_EPERM     = -6,
	THINKOS_ENOSYS    = -7, /**< Invalid system call */
	THINKOS_EFAULT    = -8,        
	THINKOS_ENOMEM    = -9  /**< Resource pool exausted */ 
};

enum thinkos_obj_kind {
	THINKOS_OBJ_READY = 0,
	THINKOS_OBJ_TMSHARE,
	THINKOS_OBJ_CLOCK,
	THINKOS_OBJ_MUTEX,
	THINKOS_OBJ_COND,
	THINKOS_OBJ_SEMAPHORE,
	THINKOS_OBJ_EVENT,
	THINKOS_OBJ_FLAG,
	THINKOS_OBJ_GATE,
	THINKOS_OBJ_JOIN,
	THINKOS_OBJ_CONREAD,
	THINKOS_OBJ_CONWRITE,
	THINKOS_OBJ_PAUSED,
	THINKOS_OBJ_CANCELED,
	THINKOS_OBJ_FAULT,
	THINKOS_OBJ_INVALID
};

#define IRQ_PRIORITY_HIGHEST   (1 << 5)
#define IRQ_PRIORITY_VERY_HIGH (2 << 5)
#define IRQ_PRIORITY_HIGH      (3 << 5)
#define IRQ_PRIORITY_REGULAR   (4 << 5)
#define IRQ_PRIORITY_LOW       (5 << 5)
#define IRQ_PRIORITY_VERY_LOW  (6 << 5)

#define THINKOS_OPT_PRIORITY(VAL)   (((VAL) & 0xff) << 16)
#define THINKOS_OPT_ID(VAL)         (((VAL) & 0x07f) << 24)
#define THINKOS_OPT_PAUSED          (1 << 31) /* don't run at startup */
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

/** @brief Initializes the @b ThinkOS library.
 *
 * On return the current program execution thread turns into the first 
 * thread of the system.
 * @return THINKOS_OK
 */
int thinkos_init(unsigned int opt);


/** @defgroup threads Threads
 *
 * @{
 */
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


/** @brief causes the calling thread to relinquish the CPU.
 * The thread is moved to the end of the queue for its static priority 
 * and a new thread gets to run.</p>
 */
void thinkos_yield(void);


/**@}*/


/** @defgroup time Time related calls
 *
 * @{
 */

int thinkos_sleep(unsigned int ms);

int thinkos_alarm(uint32_t clock);

uint32_t thinkos_clock(void);

/**@}*/


/** @defgroup mutex Mutexes
 *
 * @{
 */
int thinkos_mutex_alloc(void);

int thinkos_mutex_free(int mutex);

int thinkos_mutex_lock(int mutex);

int thinkos_mutex_trylock(int mutex);

int thinkos_mutex_timedlock(int mutex, unsigned int ms);

int thinkos_mutex_unlock(int mutex);

/**@}*/


/** @defgroup cond Conditional Variables
 *
 * @{
 */

int thinkos_cond_alloc(void);

int thinkos_cond_free(int cond);

int thinkos_cond_wait(int cond, int mutex);

int thinkos_cond_timedwait(int cond, int mutex, unsigned int ms);

int thinkos_cond_signal(int cond);

int thinkos_cond_broadcast(int cond);

/**@}*/


/** @defgroup sem Semaphores
 *
 * @{
 */

int thinkos_sem_alloc(unsigned int value);

int thinkos_sem_free(int sem);

int thinkos_sem_init(int sem, unsigned int value);

int thinkos_sem_wait(int sem);

int thinkos_sem_timedwait(int sem, unsigned int ms);

int thinkos_sem_post(int sem);

/**@}*/


/** @defgroup evset Event sets
 *
 * @{
 */
int thinkos_ev_alloc(void);

int thinkos_ev_free(int set);

int thinkos_ev_wait(int set);

int thinkos_ev_timedwait(int set, unsigned int ms);

int thinkos_ev_raise(int set, int ev);

int thinkos_ev_mask(int set, int ev, int val);

int thinkos_ev_clear(int set, int ev);

/**@}*/


/** @defgroup flag Flags
 *
 * @{
 */
int thinkos_flag_alloc(void);

int thinkos_flag_free(int flag);

int thinkos_flag_val(int flag);

int thinkos_flag_set(int flag);

int thinkos_flag_clr(int flag);

int thinkos_flag_give(int flag);

int thinkos_flag_take(int flag);

int thinkos_flag_timedtake(int flag, unsigned int ms);
/**@}*/


/** @defgroup gates Gates
 * Gates are syncronization objects which provide a convenient way of 
 * creating mutual exclusion acess to 
 * code blocks signaled by interrupt handlers...
 * 
 * A gate have a lock flag and a signal flag. A gate can be in one of 
 * the following states:
 * - @b CLOSED: no threads crossed the gate yet. 
 * - @b LOCKED: a thread entered the gate, closed and locked it.
 * - @b OPENED: no threads are waiting in the gate, the first thread to
 * call @c thinkos_gate_wait() will cross the gate.
 * - @b SIGNALED: a thread crossed the gate and locked it, but the gate
 * received a signal to open. When the thread exits the gate the gate will
 * stay open.
 * @{
 */

/** @brief Alloc a gate synchronization object.
 *
 * @return return a handler for a new gate object, or a negative value if
 * an error ocurred.
 *
 * Errors:
 * - #THINKOS_ENOSYS if the system call is not enabled.
 * - #THINKOS_ENOMEM no gates left in the gate pool.
 */
int thinkos_gate_alloc(void);

/** @brief Frees the gate synchronization object.
 *
 * @param gate handler for a gate object which must have been returned by 
 * a previous call to @c thinkos_gate_alloc().
 * @return returns #THINKOS_OK on sucess. On error a negative code value is returned.
 * an error ocurred.
 *
 * Errors:
 * - #THINKOS_EINVAL @p gate is not a valid gate handler.
 * - #THINKOS_ENOSYS not implemented.
 */
int thinkos_gate_free(int gate);

/** @brief Wait for a gate to open.
 *
 * If the gate is open this function return imediatelly, otherwise it will
 * block the calling thread.
 *
 * @param gate The gate descriptor.
 * @return #THINKOS_EINVAL if @p gate is invalid, #THINKOS_OK otherwise. 
 */
int thinkos_gate_wait(int gate);

/** @brief Wait for a gate to open or a timeout.
 *
 * If the gate is open this function return imediatelly, otherwise it will
 * block the calling thread.
 *
 * @param gate The gate descriptor.
 * @param ms Timeout ins milliseconds.
 * @return 
 * - #THINKOS_OK is returned on sucess. On error a negative code value 
 * is returned.
 * - #THINKOS_EINVAL: @p gate is not a valid handler.
 * - #THINKOS_ETIMEDOUT: timer expired before the @p gate opens.
 * - #THINKOS_ENOSYS: syscall not implemented.
 */
int thinkos_gate_timedwait(int gate, unsigned int ms);

/** @brief Open or signal the gate.
 * 
 * The resulting gete's state will depend on the current gate state and 
 * whether there are threads waiting at the gate. There are four possible
 * scenarios ... :
 * -# the gate is open already, then this function does nothing.
 * -# the gate is closed and no threads are waiting it will open the gate,
 * allowing the next thread to call @c gate_open() to enter the gate.
 * -# the gate is closed and at least one thread is waiting it will allow 
 * the thread to cross the gate, in this case the gate will be locked.
 * -# a thread crossed the gate (gate state is @b LOCKED), then the gate
 * will be signaled to open when the gate is unlocked.
 *
 * @param gate The gate descriptor.
 * @return #THINKOS_EINVAL if @p gate is invalid, #THINKOS_OK otherwise. 
 */
int thinkos_gate_open(int gate);

/** @brief Close the gate if the gate is @b OPEN or 
 * remove pending signaling it the gate is @ LOCKED.
 *
 * @param gate The gate descriptor.
 * @return #THINKOS_EINVAL if @p gate is invalid, #THINKOS_OK otherwise. 
 */
int thinkos_gate_close(int gate);

/** @brief Exit the gate, leaving the gate, optionally leaving it open 
 * or closed.
 *
 * @param gate The gate descriptor.
 * @param open Indicate the state of the gate on exit. 
 * - @p open > 0, the gate will be left open, allowing for another thread 
 * to enter the gate.
 * - @p open == 0, the gate will stay closed if not signaled, in wich case
 * it will open accordingly.
 * @return #THINKOS_EINVAL if @p gate is invalid, #THINKOS_OK otherwise. 
 */
int thinkos_gate_exit(int gate, unsigned int open);

/**@}*/


/** @defgroup irq Interrupt Requests
 *
 * @{
 */
int thinkos_irq_wait(int irq);

int	thinkos_irq_register(int irq, int pri, void (* isr)(void));
/**@}*/


/** @defgroup sysinfo System Information
 *  These calls return information about the ThinkOS configuratiion
 * and operational parameters.
 *
 * @{
 */
int thinkos_sysinfo_clocks(uint32_t * clk[]);

int thinkos_sysinfo_udelay_factor(int32_t * factor);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __THINKOS_H__ */

