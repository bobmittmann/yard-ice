/* 
 * thikos_util.c
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
 * but WITHOUT ANY WARRANTY; without flagen the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

#define __THINKOS_SYS__
#include <thinkos_sys.h>

#if THINKOS_ENABLE_JOIN || THINKOS_ENABLE_DEBUG_FAULT
bool __thinkos_thread_isalive(unsigned int th)
{
//	bool dead;

	if (th == THINKOS_THREAD_IDLE)
		return true;

#if (THINKOS_ENABLE_THREAD_VOID)
	if (th == THINKOS_THREAD_VOID)
		return (thinkos_rt.void_ctx == NULL) ? false : true;
#endif

	if (th >= THINKOS_THREADS_MAX)
		return false;

	if (thinkos_rt.ctx[th] == NULL)
		return false;
#if 0
	dead = false;
#if THINKOS_ENABLE_JOIN
	dead |= __bit_mem_rd(&thinkos_rt.wq_canceled, th);
#endif
#if THINKOS_ENABLE_DEBUG_FAULT
	dead |= __bit_mem_rd(&thinkos_rt.wq_fault, th);
#endif

	return !dead;
#endif

	return true;
}
#endif

