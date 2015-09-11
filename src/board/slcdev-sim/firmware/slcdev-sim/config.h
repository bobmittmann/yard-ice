/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
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
 * @brief USB to Serial Converter configuration
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __CONFIG_H__
#define __CONFIG_H__

/* -------------------------------------------------------------------------
   ThinkOS configuration
   -------------------------------------------------------------------------- */

#define THINKOS_IRQ_MAX 0

#define THINKOS_EXCEPT_STACK_SIZE 312

#define THINKOS_ENABLE_IDLE_WFI 1

#define THINKOS_ENABLE_CLOCK 1

#define THINKOS_ENABLE_TIMESHARE 0

#define THINKOS_THREADS_MAX 3
#define THINKOS_ENABLE_THREAD_ALLOC 0

#define THINKOS_ENABLE_JOIN 0
#define THINKOS_ENABLE_PAUSE 0
#define THINKOS_ENABLE_CANCEL 0
#define THINKOS_ENABLE_EXIT 0

#define THINKOS_MUTEX_MAX 0
#define THINKOS_ENABLE_MUTEX_ALLOC 0

#define THINKOS_COND_MAX 0
#define THINKOS_ENABLE_COND_ALLOC 0

#define THINKOS_SEMAPHORE_MAX 0
#define THINKOS_ENABLE_SEM_ALLOC 0

#define THINKOS_EVENT_MAX 1
#define THINKOS_ENABLE_EVENT_ALLOC 0

#define THINKOS_FLAG_MAX 0
#define THINKOS_ENABLE_FLAG_ALLOC 0

#define THINKOS_GATE_MAX 2
#define THINKOS_ENABLE_GATE_ALLOC 0

#define THINKOS_ENABLE_THREAD_STAT 0
#define THINKOS_ENABLE_TIMED_CALLS 1

#define THINKOS_ENABLE_ALARM 1
#define THINKOS_ENABLE_SLEEP 1
#define THINKOS_ENABLE_BREAK 0

#define THINKOS_ENABLE_ARG_CHECK 0

#define THINKOS_ENABLE_DEADLOCK_CHECK 0

#define THINKOS_ENABLE_EXCEPTIONS 1
#define THINKOS_UNROLL_EXCEPTIONS 0

#define THINKOS_SYSRST_ONFAULT 1

#define THINKOS_STDERR_FAULT_DUMP 1

#define THINKOS_ENABLE_RT_DEBUG 0

#define THINKOS_ENABLE_PROFILING 0

/* -------------------------------------------------------------------------
   MicroJS configuration
   -------------------------------------------------------------------------- */

#define MICROJS_DEBUG_ENABLED 0
#define MICROJS_TRACE_ENABLED 0
#define MICROJS_VERBOSE_ENABLED 1
#define MICROJS_STRINGS_ENABLED 1
#define MICROJS_STRINGBUF_ENABLED 0
#define MICROJS_STDLIB_ENABLED 0
#define MICROJS_OPTIMIZATION_ENABLED 1

#define JSON_LABEL_LST_MAX 48
#define JSON_STRING_LEN_MAX 192

/* -------------------------------------------------------------------------
   Other configuration
   -------------------------------------------------------------------------- */

#define STM32_ENABLE_PLL 0

#define SHELL_LINE_MAX 72
#define SHELL_ARG_MAX 12
#define SHELL_HISTORY_MAX 5

#define TTY_INBUF_LEN 8
#define TTY_DEV_MAX 1

#define FILE_DEV_MAX 2

#endif /* __CONFIG_H__ */

