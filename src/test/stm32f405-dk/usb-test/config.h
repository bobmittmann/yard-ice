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
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define THINKOS_IRQ_MAX 80

#define THINKOS_ENABLE_IDLE_WFI 0
#define THINKOS_ENABLE_IDLE_SNAPSHOT 0

#define THINKOS_ENABLE_CLOCK 1

#define THINKOS_ENABLE_TIMESHARE 0
#define THINKOS_SCHED_LIMIT_MAX 32
#define THINKOS_SCHED_LIMIT_MIN 1

#define THINKOS_THREADS_MAX 32
#define THINKOS_ENABLE_THREAD_ALLOC 1

#define THINKOS_ENABLE_JOIN 0
#define THINKOS_ENABLE_PAUSE 0
#define THINKOS_ENABLE_CANCEL 0

#define THINKOS_MUTEX_MAX 32
#define THINKOS_ENABLE_MUTEX_ALLOC 1

#define THINKOS_COND_MAX 32
#define THINKOS_ENABLE_COND_ALLOC 1

#define THINKOS_SEMAPHORE_MAX 32
#define THINKOS_ENABLE_SEM_ALLOC 1

#define THINKOS_EVENT_MAX 32
#define THINKOS_ENABLE_EVENT_ALLOC 1

#define THINKOS_FLAG_MAX 32
#define THINKOS_ENABLE_FLAG_ALLOC 1
#define THINKOS_ENABLE_FLAG_SYSCALL 1

#define THINKOS_ENABLE_THREAD_STAT 1
#define THINKOS_ENABLE_TIMED_CALLS 1

#define THINKOS_ENABLE_ARG_CHECK 1

#define THINKOS_ENABLE_DEADLOCK_CHECK 0

#define THINKOS_ENABLE_EXCEPTIONS 1

#define THINKOS_EXCEPT_STACK_SIZE 256

#endif /* __CONFIG_H__ */

