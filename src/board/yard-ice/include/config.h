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
 * @file config.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* -------------------------------------------------------------------------
   ThinkOS configuration
   -------------------------------------------------------------------------- */

#define THINKOS_IRQ_MAX 80

#define THINKOS_ENABLE_IDLE_WFI 1

#define THINKOS_ENABLE_CLOCK 1

#define THINKOS_ENABLE_TIMESHARE 0
#define THINKOS_SCHED_LIMIT_MAX 32
#define THINKOS_SCHED_LIMIT_MIN 1

#define THINKOS_THREADS_MAX 20
#define THINKOS_ENABLE_THREAD_ALLOC 1

#define THINKOS_ENABLE_JOIN 0
#define THINKOS_ENABLE_PAUSE 0
#define THINKOS_ENABLE_CANCEL 0
#define THINKOS_ENABLE_EXIT 0

#define THINKOS_MUTEX_MAX 16
#define THINKOS_ENABLE_MUTEX_ALLOC 1

#define THINKOS_COND_MAX 16
#define THINKOS_ENABLE_COND_ALLOC 1

#define THINKOS_SEMAPHORE_MAX 16
#define THINKOS_ENABLE_SEM_ALLOC 1

#define THINKOS_EVENT_MAX 2
#define THINKOS_ENABLE_EVENT_ALLOC 1

#define THINKOS_FLAG_MAX 16
#define THINKOS_ENABLE_FLAG_ALLOC 1
#define THINKOS_ENABLE_FLAG_SYSCALL 1

#define THINKOS_ENABLE_THREAD_STAT 1
#define THINKOS_ENABLE_TIMED_CALLS 1

#define THINKOS_ENABLE_ARG_CHECK      0
#define THINKOS_ENABLE_DEADLOCK_CHECK 0

#define THINKOS_EXCEPT_STACK_SIZE 320

#define THINKOS_STDERR_FAULT_DUMP  0

#define THINKOS_ENABLE_SCHED_DEBUG 0

#define THINKOS_ENABLE_RT_DEBUG    0

#define THINKOS_ENABLE_BREAK       0

#define THINKOS_ENABLE_PROFILING   1

#define THINKOS_ENABLE_MONITOR     1

#define THINKOS_ENABLE_CONSOLE     1

#define THINKOS_ENABLE_DEBUG_STEP  0

#define THINKOS_ENABLE_DEBUG_FAULT 0

#define THINKOS_ENABLE_EXCEPTIONS     0
#define THINKOS_UNROLL_EXCEPTIONS     0
#define THINKOS_SYSRST_ONFAULT        1
#define THINKOS_STDERR_FAULT_DUMP     0
#define THINKOS_ENABLE_EXCEPT_RESET   0

#define THINKOS_ENABLE_BUSFAULT    0

#define THINKOS_ENABLE_USAGEFAULT  0

#define THINKOS_UNROLL_EXCEPTIONS  0

#define THINKOS_ENABLE_MPU         0

#define THINKOS_ENABLE_FPU         0

#define THINKOS_ENABLE_CTL         0

#define THINKOS_DMON_STACK_SIZE    256

/* -------------------------------------------------------------------------
   USB 
   -------------------------------------------------------------------------- */

#define STM32_ENABLE_OTG_FS 1

/* -------------------------------------------------------------------------
   Printf
   -------------------------------------------------------------------------- */
#define PRINTF_ENABLE_LEFT_ALIGN     0
#define PRINTF_ENABLE_LARGE_PADDING  0
#define PRINTF_ENABLE_FAST           0
#define PRINTF_ENABLE_UNSIGNED       1
#define PRINTF_ENABLE_POINTER        0
#define PRINTF_ENABLE_LONG           0
#define PRINTF_ENABLE_ARG_WIDTH      0

/* -------------------------------------------------------------------------
   Bootloader 
   -------------------------------------------------------------------------- */

#define BOOT_ENABLE_GDB 0
#define MONITOR_CONFIGURE_ENABLE 0
#define MONITOR_DUMPMEM_ENABLE 0

#endif /* __CONFIG_H__ */


