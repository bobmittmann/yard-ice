/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file sys/clock.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_CLOCK_H__
#define __SYS_CLOCK_H__

#ifndef __ASSEMBLER__

/*
 * The IDs of the various system clocks (for POSIX.1b interval timers):
 */
#define CLOCK_REALTIME			0
#define CLOCK_MONOTONIC			1
#define CLOCK_PROCESS_CPUTIME_ID	2
#define CLOCK_THREAD_CPUTIME_ID		3

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

int clock_settime(clockid_t which, const struct timespec * tp);

int clock_gettime(clockid_t which, struct timespec * tp);

int clock_getres(clockid_t which, struct timespec * tp);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __SYS_CLOCK_H__ */

