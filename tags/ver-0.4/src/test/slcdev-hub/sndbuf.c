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
#include <sys/dcclog.h>

#include "sndbuf.h"
#include "trace.h"

const unsigned int sndbuf_len  = SNDBUF_LEN;

/* ----------------------------------------------------------------------
 * sound buffer pool
 * ----------------------------------------------------------------------
 */

struct sndbuf_blk {
	union {
		struct sndbuf buf;
		struct {
			uint32_t ref;
			struct sndbuf_blk * next;
		};
    };
};

struct {
	uint32_t error;
	uint32_t free_cnt;
	uint32_t alloc_cnt;
	struct sndbuf_blk * free;
	struct sndbuf_blk blk[SNDBUF_POOL_SIZE];
} sndbuf_pool;


sndbuf_t * sndbuf_alloc(void)
{
	struct sndbuf_blk * blk;
	uint32_t primask;

	/* critical section enter */
	primask = cm3_primask_get();
	cm3_primask_set(1);

	if ((blk = sndbuf_pool.free) != NULL) {
		sndbuf_pool.free = blk->next;
		blk->ref = 1;
		sndbuf_pool.alloc_cnt++;
		DCC_LOG1(LOG_INFO, "buf=0x%08x", blk);
	} else {
		sndbuf_pool.error++;
		DCC_LOG2(LOG_ERROR, "failed!, allocs=%d frees=%d",
				 sndbuf_pool.alloc_cnt, sndbuf_pool.free_cnt);

		tracef("%s(): failed!, allocs=%d frees=%d", 
			   __func__, sndbuf_pool.alloc_cnt, sndbuf_pool.free_cnt);
	}

	/* critical section exit */
	cm3_primask_set(primask);

	return (sndbuf_t *)blk;
}

sndbuf_t * sndbuf_clear(sndbuf_t * buf)
{
	uint64_t * ptr;

	if ((ptr = (uint64_t *)buf->data) == NULL)
		return buf;

	ptr[0] = 0;
#if SNDBUF_LEN > 8
	ptr[1] = 0;
#if SNDBUF_LEN > 16
	ptr[2] = 0;
#if SNDBUF_LEN > 24
	ptr[3] = 0;
#if SNDBUF_LEN > 32
	ptr[4] = 0;
#if SNDBUF_LEN > 40
	ptr[5] = 0;
#if SNDBUF_LEN > 48
	ptr[6] = 0;
#if SNDBUF_LEN > 56
	ptr[7] = 0;
#if SNDBUF_LEN > 64
	ptr[8] = 0;
#if SNDBUF_LEN > 72
	ptr[9] = 0;
#if SNDBUF_LEN > 80
	ptr[10] = 0;
#if SNDBUF_LEN > 88
	ptr[11] = 0;
#if SNDBUF_LEN > 96
	ptr[12] = 0;
#if SNDBUF_LEN > 104
	ptr[13] = 0;
#if SNDBUF_LEN > 112
	ptr[14] = 0;
#if SNDBUF_LEN > 120
	ptr[15] = 0;
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif

	return buf;
}

sndbuf_t * sndbuf_calloc(void)
{
	return sndbuf_clear(sndbuf_alloc());
}

int sndbuf_id(sndbuf_t * buf)
{
	return buf - (sndbuf_t *)sndbuf_pool.blk;
}


sndbuf_t * sndbuf_use(sndbuf_t * buf)
{
	uint32_t primask;

	/* critical section enter */
	primask = cm3_primask_get();
	cm3_primask_set(1);

	/* check whether the buffer is valid or not */
	if (buf == NULL) {
		DCC_LOG(LOG_PANIC, "NULL pointer!");
	} else {
		if (buf->ref == 0) {
			DCC_LOG1(LOG_WARNING, "buf=0x%08x invalid!", buf);
			buf = NULL;
	 	} else {
			DCC_LOG1(LOG_INFO, "buf=%d", buf - (sndbuf_t *)sndbuf_pool.blk);
			buf->ref++;
		}
	}

	/* critical section exit */
	cm3_primask_set(primask);

	return buf;
}

void sndbuf_free(sndbuf_t * buf)
{
	struct sndbuf_blk * blk = (struct sndbuf_blk *)buf;
	uint32_t primask;

	/* critical section enter */
	primask = cm3_primask_get();
	cm3_primask_set(1);

	if (buf == NULL) {
		DCC_LOG(LOG_PANIC, "NULL pointer!");
	} else {
		/* decrement reference counter */
		if (buf->ref == 0) {
			DCC_LOG1(LOG_WARNING, "buf=0x%08x invalid!", buf);
		} else { 
			DCC_LOG2(LOG_INFO, "buf=%d ref=%d", 
					 buf - (sndbuf_t *)sndbuf_pool.blk, buf->ref);
			if (--buf->ref == 0) {
				blk->next = sndbuf_pool.free;
				sndbuf_pool.free = blk;
				sndbuf_pool.free_cnt++;
			}
		}
	}

	/* critical section exit */
	cm3_primask_set(primask);

}

void sndbuf_pool_init(void)
{
	int i;

	DCC_LOG1(LOG_TRACE, "pool_size=%d.", SNDBUF_POOL_SIZE);

	sndbuf_pool.free = &sndbuf_pool.blk[0];

	for (i = 0; i < SNDBUF_POOL_SIZE - 1; ++i) {
		sndbuf_pool.blk[i].next = &sndbuf_pool.blk[i + 1];
		sndbuf_pool.blk[i].ref = 0;
	}

	sndbuf_pool.blk[i].next = NULL;
	sndbuf_pool.blk[i].ref = 0;
	sndbuf_pool.error = 0;
	sndbuf_pool.free_cnt = 0;
	sndbuf_pool.alloc_cnt = 0;
}

int sndbuf_pool_test(void)
{
	sndbuf_t * lst[SNDBUF_POOL_SIZE];
	sndbuf_t * buf;
	int i;
	int j = 0;

	do {
		for (i = 0; i < SNDBUF_POOL_SIZE; ++i) {
			buf = sndbuf_alloc();
			if (buf == NULL) {
				DCC_LOG1(LOG_PANIC, "%d: sndbuf_alloc() failed!", i);
				return -1;
			}
			lst[i] = buf;
		}

		buf = sndbuf_alloc();
		if (buf != NULL) {
			DCC_LOG2(LOG_PANIC, "%d: sndbuf_alloc() != NULL!", i, buf);
			return -1;
		}

		for (i = 0; i < SNDBUF_POOL_SIZE; ++i) {
			sndbuf_free(lst[i]);
		}

	} while (++j < 2);

	DCC_LOG(LOG_TRACE, "success.");

	return 0;
}

int sndbuf_cmp(sndbuf_t * a, sndbuf_t * b)
{
	int i;

	for (i = 0; i < SNDBUF_LEN; ++i) {
		if (a->data[i] != b->data[i]) {
			return i + 1;
		}
	}

	return 0;
}

const struct sndbuf sndbuf_zero;

struct sndbuf sndbuf_null;

