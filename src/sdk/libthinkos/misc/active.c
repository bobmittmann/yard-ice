/* 
 * __thikos_active.c
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

/* return true if at least one thread is enabled in the core */
bool __thinkos_active(void)
{
	unsigned int thread_id;

	for (thread_id = 0; thread_id < THINKOS_THREADS_MAX; ++thread_id) {
		if (thinkos_rt.ctx[thread_id] != NULL)
			return true;
	}

	return false;
}

