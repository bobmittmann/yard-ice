/* 
 * thinkos.h
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
 * @brief ThinkOS API
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
	THINKOS_OBJ_READY     = 0,
	THINKOS_OBJ_TMSHARE   = 1,
	THINKOS_OBJ_CLOCK     = 2,
	THINKOS_OBJ_MUTEX     = 3,
	THINKOS_OBJ_COND      = 4,
	THINKOS_OBJ_SEMAPHORE = 5,
	THINKOS_OBJ_EVENT     = 6,
	THINKOS_OBJ_FLAG      = 7,
	THINKOS_OBJ_GATE      = 8,
	THINKOS_OBJ_JOIN      = 9,
	THINKOS_OBJ_CONREAD   = 10,
	THINKOS_OBJ_CONWRITE  = 11,
	THINKOS_OBJ_PAUSED    = 12,
	THINKOS_OBJ_CANCELED  = 13,
	THINKOS_OBJ_FAULT     = 14,
	THINKOS_OBJ_COMMSEND  = 15,
	THINKOS_OBJ_COMMRECV  = 16,
	THINKOS_OBJ_INVALID
};

enum thinkos_thread_status {
	THINKOS_THREAD_CANCELED = 0x7ffffffe,
	THINKOS_THREAD_ABORTED  = 0x7fffffff
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

#include <stdint.h>

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

/** @brief Initializes the Cortex-M MPU.
 *
 */
void thinkos_mpu_init(unsigned int size);

/** @brief Switch processor to user mode.
 *
 */
void thinkos_userland(void);

/** @defgroup threads Threads
 *
 * @{
 */

/** @brief create a new thread
 *
 * @param task_ptr
 * @param task_arg
 * @param stack_ptr
 * @param opt 
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_thread_create(int (* task_ptr)(void *), 
						  void * task_arg, void * stack_ptr,
						  unsigned int opt);

/** @brief create a new thread with extend information
 *
 * @param task_ptr
 * @param task_arg
 * @param inf
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_thread_create_inf(int (* task_ptr)(void *), void * task_arg,
							  const struct thinkos_thread_inf * inf);

/** @brief obtaind a handler for the calling thread
 *
 * @return This function always succeeds, returning the calling thread's ID.
 */
int const thinkos_thread_self(void);

/** @brief request a thread to terminate
 *
 * @param thread_id thread handler
 * @param code return code
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_cancel(unsigned int thread_id, int code);

/** @brief cause the thread to terminate
 *
 * @param code return code
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_exit(int code);

/** @brief join with a terminated thread
 *
 * The thinkos_join() function waits for the thread specified by @p thread_id 
 * to terminate. If that thread has already terminated, then 
 * thinkos_join() returns immediately.
 *
 * @param thread_id thread handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_join(unsigned int thread_id);

/** @brief pause the thread execution
 *
 * @param thread_id thread handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_pause(unsigned int thread_id);

/** @brief resume a thread operation
 *
 * @param thread_id thread handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_resume(unsigned int thread_id);


/** @brief causes the calling thread to relinquish the CPU.
 * The thread is moved to the end of the queue for its static priority 
 * and a new thread gets to run.
 */
void thinkos_yield(void);


/**@}*/


/** @defgroup time Time related calls
 *
 * @{
 */

/** @brief pause for a specified amount of time
 *
 * The thinkos_sleep() function suspends execution of the calling thread 
 * for (at least) ms milliseconds. The sleep may be lengthened slightly by 
 * any system activity or by the time spent processing the call or by the 
 * granularity of system timer.
 *
 * @param ms waiting time in milliseconds
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_sleep(unsigned int ms);

/** @brief retrieve the ThikOS clock time
 *
 * The ThikOS clock is a tick counter with a resolution of 1 milisecond, 
 * which cannot be set and is allways running.
 * It represents monotonic time since the system power up.

 * @return the ThinkOS monotonic clock.
 */
uint32_t thinkos_clock(void);

/** @brief pause until a specified alarm time
 *
 * This function should be used in conjucntion with thinkos_clock().
 * ... 
 *
 * @param clock time for the thread to be waked up.
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_alarm(uint32_t clock);

/**@}*/


/** @defgroup mutex Mutexes
 *
 * @{
 */

/** @brief alloc a mutex
 *
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_mutex_alloc(void);

/** @brief release a mutex
 *
 * @param mutex mutual exclusion handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_mutex_free(int mutex);

/** @brief lock a mutex, blocking if already locked.
 *
 * @param mutex mutual exclusion handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_mutex_lock(int mutex);

/** @brief try to lock a mutex, non blocking.
 *
 * @param mutex mutual exclusion handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_mutex_trylock(int mutex);

/** @brief lock a mutex, blocking for limited time.
 *
 * @param mutex mutual exclusion handler
 * @param ms waiting time in milliseconds
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_mutex_timedlock(int mutex, unsigned int ms);

/** @brief unlock the mutex
 *
 * @param mutex mutual exclusion handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_mutex_unlock(int mutex);

/**@}*/


/** @defgroup cond Conditional Variables
 *
 * @{
 */

/** @brief alloc a conditional variable
 *
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_cond_alloc(void);

/** @brief release a conditional variable
 *
 * @param cond conditional variable handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_cond_free(int cond);

/** @brief wait for a conditional variable
 *
 * @param cond conditional variable handler
 * @param mutex mutex handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_cond_wait(int cond, int mutex);

/** @brief wait for a conditional variable or timeout
 *
 * @param cond conditional variable handler
 * @param mutex mutex handler
 * @param ms waiting time in milliseconds
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_cond_timedwait(int cond, int mutex, unsigned int ms);

/** @brief signal a conditional variable wake a single thread
 *
 * @param cond conditional variable handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_cond_signal(int cond);

/** @brief signal a conditional variable wake all waiting threads
 *
 * @param cond conditional variable handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_cond_broadcast(int cond);

/**@}*/


/** @defgroup sem Semaphores
 *
 * @{
 */

/** @brief alloc a semaphore
 *
 * @param val initial semaphore value
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_sem_alloc(unsigned int val);

/** @brief release a semaphore
 *
 * @param sem semaphore handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_sem_free(int sem);

/** @brief set the initial value of a semaphore
 *
 * @param sem semaphore handler
 * @param val
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_sem_init(int sem, unsigned int val);

/** @brief wait for a semaphore to be signaled
 *
 * @param sem semaphore handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_sem_wait(int sem);

/** @brief wait for a semaphore to be signaled or timeout
 *
 * @param sem semaphore handler
 * @param ms
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_sem_timedwait(int sem, unsigned int ms);

/** @brief signal a semaphore 
 *
 * @param sem semaphore handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_sem_post(int sem);

/** @brief signal a semaphore inside an interrupt handler
 *
 * @param sem semaphore handler
 */
void thinkos_sem_post_i(int sem);

/**@}*/


/** @defgroup evset Event sets
 *
 * @{
 */

/** @brief alloc an event set
 *
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_ev_alloc(void);

/** @brief release an event set
 *
 * @param set event set handler
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_ev_free(int set);

/** @brief wait for an event 
 *
 * @param set event set handler
 * @return #THINKOS_ENOSYS if call is not implemented, an event otherwise. 
 */
int thinkos_ev_wait(int set);

/** @brief wait for an event or timeout
 *
 * @param set event set handler
 * @param ms time to wait in milliseconds
 * @return #THINKOS_ENOSYS if call is not implemented, an event otherwise. 
 */
int thinkos_ev_timedwait(int set, unsigned int ms);

/** @brief signal an event
 *
 * @param set event set handler
 * @param ev event identifier
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_ev_raise(int set, int ev);

/** @brief signal an event from inside an interrupt handler
 *
 * @param set event set handler
 * @param ev event identifier
 */
void thinkos_ev_raise_i(int set, int ev);

/** @brief mask/unmask an event
 *
 * @param set event set handler
 * @param ev event identifier
 * @param val mask value. 1 enable the event, 0 disable the event.
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_ev_mask(int set, int ev, int val);

/** @brief clear an event
 *
 * @param set event set handler
 * @param ev event identifier
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_ev_clear(int set, int ev);

/**@}*/


/** @defgroup flag Flags
 *
 * @{
 */

/** @brief alloc a flag 
 *
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_flag_alloc(void);

/** @brief release a flag
 *
 * @param flag
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_flag_free(int flag);

/** @brief return the flag value
 *
 * @param flag
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_flag_val(int flag);

/** @brief set a flag 
 *
 * @param flag
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_flag_set(int flag);

/** @brief clear a flag 
 *
 * @param flag
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_flag_clr(int flag);

/** @brief signal a flag 
 *
 * @param flag
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_flag_give(int flag);

/** @brief signal a flag form inside an interrupt handler
 *
 * @param flag
 */
void thinkos_flag_give_i(int flag);

/** @brief wait for flag to be signaled
 *
 * @param flag
 */
int thinkos_flag_take(int flag);

/** @brief wait for flag or timeout
 *
 * @param flag
 * @param ms 
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
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
 * The resulting gate's state will depend on the current gate state and 
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

/** @brief Open or signal the gate from inside an interrupt handler.
 * 
 * This call is similar to the @c thinkos_gate_open() except that it
 * is safe to ba called from inside an interrupt handler.
 *
 * @param gate The gate descriptor.
 *
 * @b Warning: no argument validation is performed. 
 */
void thinkos_gate_open_i(int gate);


/** @brief Close the gate if the gate is @b OPEN or 
 * remove pending signaling if the gate is @ LOCKED.
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

/** @brief wait for interrupt
 *
 * @param irq
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_irq_wait(int irq);

/** @brief reqister an interrupt handler
 *
 * @param irq
 * @param pri 
 * @param isr 
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int	thinkos_irq_register(int irq, int pri, void (* isr)(void));
/**@}*/


/** @defgroup ctl OS Monitor and Control.
 *
 * These calls return information about the ThinkOS configuratiion
 * and operational parameters.
 *
 * @{
 */

/** @brief get hardware clocks frequencies 
 *
 * @param clk[] pointer to an array to receive the clocks frequency list.
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_clocks(uint32_t * clk[]);

/** @brief get udelay calibration factor
 *
 * @param factor pointer to an integer. 
 * @return #THINKOS_ENOSYS if call is not implemented, #THINKOS_OK otherwise. 
 */
int thinkos_udelay_factor(int32_t * factor);

/** @brief abort the operating system.
 *
 * @return This function does not return.
 */
void __attribute__((noreturn)) thinkos_abort(void);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __THINKOS_H__ */

