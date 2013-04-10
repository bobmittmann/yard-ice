/* 
 * File:	 sndbuf.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <arch/cortex-m3.h>
#include <stdlib.h>
#include <stdint.h>

#include "sndbuf.h"

/* ----------------------------------------------------------------------
 * sound buffer pool
 * ----------------------------------------------------------------------
 */



struct sndbuf {
	union {
		struct sndbuf * next;
		uint64_t u64;
		int16_t data[SNDBUF_LEN + sizeof(uint32_t) / (sizeof(uint32_t) * 2)];
    };
};

struct {
	uint32_t error;
	struct sndbuf * free;
	struct sndbuf buf[SNDBUF_POOL_SIZE];
} sndbuf_pool;

const unsigned int sndbuf_len  = SNDBUF_LEN;

sndbuf_t * sndbuf_alloc(void)
{
	struct sndbuf * buf;
	uint32_t primask;

	/* critical section enter */
	primask = cm3_primask_get();
	cm3_basepri_set(1);

	if ((buf = sndbuf_pool.free) != NULL)
		sndbuf_pool.free = buf->next;
	else
		sndbuf_pool.error++;

	/* critical section exit */
	cm3_primask_set(primask);

	return (sndbuf_t *)buf;
}

sndbuf_t * sndbuf_clear(sndbuf_t * buf)
{
	uint64_t * ptr;

	if ((ptr = (uint64_t *)buf) == NULL)
		return buf;

#if SNDBUF_LEN <= 4
	ptr[0] = 0;
#elif SNDBUF_LEN <= 8
	ptr[0] = 0;
	ptr[1] = 0;
#elif SNDBUF_LEN <= 12
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
#elif SNDBUF_LEN <= 16
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
#elif SNDBUF_LEN <= 20
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
#elif SNDBUF_LEN <= 24
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
#elif SNDBUF_LEN <= 28
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
#elif SNDBUF_LEN <= 32
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
	ptr[7] = 0;
#elif SNDBUF_LEN <= 36
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
	ptr[7] = 0;
	ptr[8] = 0;
#elif SNDBUF_LEN <= 40
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
	ptr[7] = 0;
	ptr[8] = 0;
	ptr[9] = 0;
#elif SNDBUF_LEN <= 44
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
	ptr[7] = 0;
	ptr[8] = 0;
	ptr[9] = 0;
	ptr[10] = 0;
#elif SNDBUF_LEN <= 48
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
	ptr[7] = 0;
	ptr[8] = 0;
	ptr[9] = 0;
	ptr[10] = 0;
	ptr[11] = 0;
#elif SNDBUF_LEN <= 52
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
	ptr[7] = 0;
	ptr[8] = 0;
	ptr[9] = 0;
	ptr[10] = 0;
	ptr[11] = 0;
	ptr[12] = 0;
#elif SNDBUF_LEN <= 56
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
	ptr[7] = 0;
	ptr[8] = 0;
	ptr[9] = 0;
	ptr[10] = 0;
	ptr[11] = 0;
	ptr[12] = 0;
	ptr[13] = 0;
#elif SNDBUF_LEN <= 60
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
	ptr[7] = 0;
	ptr[8] = 0;
	ptr[9] = 0;
	ptr[10] = 0;
	ptr[11] = 0;
	ptr[12] = 0;
	ptr[13] = 0;
	ptr[14] = 0;
#elif SNDBUF_LEN <= 64
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = 0;
	ptr[3] = 0;
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
	ptr[7] = 0;
	ptr[8] = 0;
	ptr[9] = 0;
	ptr[10] = 0;
	ptr[11] = 0;
	ptr[12] = 0;
	ptr[13] = 0;
	ptr[14] = 0;
	ptr[15] = 0;
#else
	int i;

	for (i = 0; i < (SNDBUF_LEN + 7) / 8; ++i)
		ptr[i] = 0;
#endif

	return buf;
}

sndbuf_t * sndbuf_calloc(void)
{
	return sndbuf_clear( sndbuf_alloc());
}

void sndbuf_free(sndbuf_t * ptr)
{
	struct sndbuf * buf = (struct sndbuf *)ptr;
	uint32_t primask;

	/* critical section enter */
	primask = cm3_primask_get();
	cm3_basepri_set(1);

	buf->next = sndbuf_pool.free;
	sndbuf_pool.free = buf;

	/* critical section exit */
	cm3_primask_set(primask);

}

void sndbuf_pool_init(void)
{
	int i;

	sndbuf_pool.free = &sndbuf_pool.buf[0];

	for (i = 0; i < SNDBUF_POOL_SIZE - 1; i++) {
		sndbuf_pool.buf[i].next = &sndbuf_pool.buf[i + 1];
	}

	sndbuf_pool.buf[i].next = NULL;
	sndbuf_pool.error = 0;
}


