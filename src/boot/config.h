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
#define THINKOS_ENABLE_IDLE_SNAPSHOT 1

#define THINKOS_ENABLE_CLOCK 1

#define THINKOS_ENABLE_TIMESHARE 1
#define THINKOS_SCHED_LIMIT_MAX 32
#define THINKOS_SCHED_LIMIT_MIN 1

#define THINKOS_THREADS_MAX 32
#define THINKOS_ENABLE_THREAD_ALLOC 1

#define THINKOS_ENABLE_JOIN 1
#define THINKOS_ENABLE_PAUSE 1
#define THINKOS_ENABLE_CANCEL 1

#define THINKOS_MUTEX_MAX 32
#define THINKOS_ENABLE_MUTEX_ALLOC 1

#define THINKOS_COND_MAX 32
#define THINKOS_ENABLE_COND_ALLOC 1

#define THINKOS_SEMAPHORE_MAX 32
#define THINKOS_ENABLE_SEM_ALLOC 1

#define THINKOS_EVENT_MAX 8
#define THINKOS_ENABLE_EVENT_ALLOC 1

#define THINKOS_FLAG_MAX 64
#define THINKOS_ENABLE_FLAG_ALLOC 1
#define THINKOS_ENABLE_FLAG_SYSCALL 1

#define THINKOS_ENABLE_THREAD_STAT 1
#define THINKOS_ENABLE_TIMED_CALLS 1

#define THINKOS_ENABLE_IRQ_SVC_CALL 1

#define THINKOS_ENABLE_ARG_CHECK 1

#define THINKOS_ENABLE_DEADLOCK_CHECK 1

#define THINKOS_ENABLE_EXCEPTIONS 1

#define THINKOS_EXCEPT_STACK_SIZE 384

#define THINKOS_STDERR_FAULT_DUMP 1

#define THINKOS_ENABLE_SCHED_DEBUG 0

#define THINKOS_ENABLE_RT_DEBUG 1

#define THINKOS_ENABLE_BREAK 1

#define THINKOS_ENABLE_PROFILING 1

/* -------------------------------------------------------------------------
   USB 
   -------------------------------------------------------------------------- */

#define STM32_ENABLE_USB_DEVICE 1

/* -------------------------------------------------------------------------
   TTY
   -------------------------------------------------------------------------- */
#define TTY_DEV_MAX 1
#define TTY_INBUF_LEN 16

/* -------------------------------------------------------------------------
   File descriptors
   -------------------------------------------------------------------------- */
#define FILE_DEV_MAX 1

/* -------------------------------------------------------------------------
   Other configuration
   -------------------------------------------------------------------------- */

#define SERIAL_TX_FIFO_LEN 32
#define SERIAL_RX_FIFO_LEN 512
#define SERIAL_RX_TRIG_LVL 4

/* -------------------------------------------------------------------------
   Printf
   -------------------------------------------------------------------------- */
#define PRINTF_ENABLE_LEFT_ALIGN 0
#define PRINTF_ENABLE_LARGE_PADDING 0
#define PRINTF_ENABLE_FAST 0
#define PRINTF_ENABLE_UNSIGNED 0
#define PRINTF_ENABLE_POINTER 0
#define PRINTF_ENABLE_LONG 0
#define PRINTF_ENABLE_ARG_WIDTH 0

#endif /* __CONFIG_H__ */


