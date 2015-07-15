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
#include <thinkos.h>

#if THINKOS_ENABLE_THREAD_ALLOC | THINKOS_ENABLE_MUTEX_ALLOC | \
	THINKOS_ENABLE_COND_ALLOC | THINKOS_ENABLE_SEM_ALLOC | \
	THINKOS_ENABLE_EVENT_ALLOC | THINKOS_ENABLE_FLAG_ALLOC | \
	THINKOS_ENABLE_GATE_ALLOC
int __thinkos_bmp_alloc(uint32_t bmp[], int bits) 
{
	int i;
	int j;

	for (i = 0; i < ((bits + 31) / 32); ++i) {
		/* Look for an empty bit MSB first */
		if ((j = __clz(__rbit(~(bmp[i])))) < 32) {
			/* Mark as used */
			__bit_mem_wr(&bmp[i], j, 1);  
			return 32 * i + j;;
		}
	}
	return THINKOS_ENOMEM;
}
#endif

#if THINKOS_ENABLE_THREAD_ALLOC | THINKOS_ENABLE_MUTEX_ALLOC | \
	THINKOS_ENABLE_COND_ALLOC | THINKOS_ENABLE_SEM_ALLOC | \
	THINKOS_ENABLE_EVENT_ALLOC | THINKOS_ENABLE_FLAG_ALLOC | \
	THINKOS_ENABLE_GATE_ALLOC
void __thinkos_bmp_init(uint32_t bmp[], int bits) 
{
	int i;
	for (i = 0; i < bits / 32; ++i)
		bmp[i] = 0;
	if (bits % 32)
		bmp[i] = 0xffffffff << (bits % 32);
}
#endif

