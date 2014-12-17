/* 
 * File:	 mempool.c
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

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#ifdef CONFIG_H
#include "config.h"
#endif

struct mempool;

#define SIZEOF_META (__SIZEOF_POINTER__ * 2)
#define META_OFFS (SIZEOF_META / sizeof(uint64_t))

struct block_meta {
	union {
		struct {
			struct mempool * pool;
			uint32_t ref;
		};
		
		uint64_t res[SIZEOF_META / 8];
	};
};

struct block {
	struct block_meta meta;
	union {
		struct block * next;
		uint64_t data[1];
    };
};

struct mempool {
	union {
		struct {
			uint32_t error;
#if 0
			struct block * tail;
			struct block * head;
#endif
			struct block * free;

#ifdef BFIN
			volatile uint32_t * spinlock;
#else
			pthread_spinlock_t spinlock;
#endif
		};
		/* dummy field to ensure 64bits alignment */
		uint64_t dummy[2];
	};
	uint64_t buf[0];
};

#ifdef BFIN

/* ------------------------------------------------------------------
 * XXX: Non portable code
 * ------------------------------------------------------------------
 *  
 * These functions implement a 'timestamp' timer and a 'spinlock'
 * They are platform dependent as there is no portable way, that I'm 
 * aware of, to get these two functionalities with high performance. 
 *
 */

static void __spinlock_create(volatile uint32_t ** lock)
{
    uint32_t * ptr;
	ptr = sram_alloc(sizeof(uint32_t), L1_DATA_SRAM);
	if (ptr == NULL) {
		fprintf(stderr, "%s: sram_alloc() failed: %s.\n", 
				__func__, strerror(errno));

		return;
	}
	*lock = ptr;
	**lock = 0;
}

static void __spinlock_destroy(volatile uint32_t * lock)
{
	sram_free((uint32_t *)lock);
}

static inline void __spin_lock(volatile uint32_t * lock)
{
	while (bfin_atomic_xchg32((uint32_t *)lock, 1)) {
		 sched_yield();
	}
}

static inline void __spin_unlock(volatile uint32_t * lock)
{
	*lock = 0;
}

#else

static inline void __spinlock_create(pthread_spinlock_t * spinlock)
{
	pthread_spin_init(spinlock, PTHREAD_PROCESS_PRIVATE);
}

static inline void __spin_lock(pthread_spinlock_t * spinlock)
{
	pthread_spin_lock(spinlock);
}

static inline void __spin_unlock(pthread_spinlock_t * spinlock)
{
	pthread_spin_unlock(spinlock);
}

static void __spinlock_destroy(pthread_spinlock_t * spinlock)
{
	pthread_spin_destroy(spinlock);
}

#endif

void * memblk_alloc(struct mempool * pool)
{
	struct block * blk;
	void * ret = NULL;

	__spin_lock(&pool->spinlock);

#if 0
	if ((blk = pool->head) != NULL) {
		if (blk == pool->tail) {
			poll->tail = NULL;
			poll->head = NULL;
		} else {
			pool->head = blk->next;
		}
#endif

	if ((blk = pool->free) != NULL) {
		pool->free = blk->next;

#if 0
		fprintf(stderr, "%s: blk=%p\n", __func__, blk);
		fflush(stderr);
#endif
		assert(blk->meta.ref == 0);
		assert(blk->meta.pool == pool);

		blk->meta.ref = 1; /* initialize reference counter */

		ret = (void *)blk->data;
	} else {
		pool->error++;
#if 0
		fprintf(stderr, "%s: memory alloc failed!\n", __func__);
		fflush(stderr);
#endif
	}

	__spin_unlock(&pool->spinlock);

	return ret;
}

void memblk_incref(void * ptr)
{
	struct block * blk = (struct block *)((uint64_t *)ptr - META_OFFS);
	struct mempool * pool = blk->meta.pool;

#if 0
	fprintf(stderr, "%s: blk=%p ref=%d\n", __func__, blk, blk->meta.ref);
	fflush(stderr);
#endif

	__spin_lock(&pool->spinlock);

	assert(blk->meta.ref > 0);

	blk->meta.ref++;

	__spin_unlock(&pool->spinlock);


}

void memblk_decref(void * ptr)
{
	struct block * blk = (struct block *)((uint64_t *)ptr - META_OFFS);
	struct mempool * pool = blk->meta.pool;

#if 0
	fprintf(stderr, "%s: blk=%p ref=%d\n", __func__, blk, blk->meta.ref);
	fflush(stderr);
#endif

	__spin_lock(&pool->spinlock);

	assert(blk->meta.ref > 0);

	if (--blk->meta.ref == 0) { 
		blk->next = pool->free;
		pool->free = blk;
#if 0
		fprintf(stderr, "%s: blk=%p released!\n", __func__, blk);
		fflush(stderr);
#endif
#if 0
		struct block * tail = pool->tail;

		if (tail == NULL) {
			pool->head = blk;
		} else {
			tail->next = blk;
		}
		pool->tail = blk;
#endif
	} 

	__spin_unlock(&pool->spinlock);
}

bool memblk_free(struct mempool * pool, void * ptr)
{
	struct block * blk = (struct block *)((uint64_t *)ptr - META_OFFS);
	bool ret = false;

#if 0
	fprintf(stderr, "%s: blk=%p ref=%d\n", __func__, blk, blk->meta.ref);
	fflush(stderr);
#endif

	__spin_lock(&pool->spinlock);

	assert(blk->meta.ref > 0);
	assert(blk->meta.pool == pool);

	if (--blk->meta.ref == 0) { 
		blk->next = pool->free;
		pool->free = blk;
#if 0
		fprintf(stderr, "%s: blk=%p released!\n", __func__, blk);
		fflush(stderr);
#endif

#if 0
		struct block * tail = pool->tail;

		if (tail == NULL) {
			pool->head = blk;
		} else {
			tail->next = blk;
		}
		pool->tail = blk;
#endif
		ret = true;
	}

	__spin_unlock(&pool->spinlock);

	return ret;
}

/* block length */
static inline size_t __blklen(size_t size) {
	return ((size + (sizeof(uint64_t) - 1)) / sizeof(uint64_t) + META_OFFS) * 
		sizeof(uint64_t);
}

void mempool_init(struct mempool * pool, size_t nmemb, size_t size)
{
	struct block * blk = (struct block *)pool->buf;
	int d;
	int i;
	int j;

	j = 0;
	d = __blklen(size) / sizeof(uint64_t);
	pool->free = blk;
	for (i = 0; i < nmemb - 1; i++) {
		j += d;
		blk->meta.pool = pool;
		blk->meta.ref = 0;
		blk->next = (struct block *)&pool->buf[j];
		blk = blk->next;
	}

	blk->meta.pool = pool;
	blk->meta.ref = 0;
	blk->next = NULL;
	pool->error = 0;
	__spinlock_create(&pool->spinlock);
}


/* Allocate a new memory pool of 'nmemb' elements of 'size' bytes. */
/* The members are 64bits aligned */
struct mempool * mempool_alloc(size_t nmemb, size_t size)
{
	struct mempool * pool;
	size_t blklen;

	fprintf(stderr, "%s: memory meta block size is %d (%d) bytes.\n", 
			__func__, sizeof(struct block_meta), SIZEOF_META); 

	assert(sizeof(struct block_meta) == SIZEOF_META);

	blklen = __blklen(size);

	pool = (struct mempool *)malloc(sizeof(struct mempool) + (nmemb * blklen));

	fprintf(stderr, "%s: memory pool: %d objects of %d bytes.\n", 
			__func__, nmemb, blklen);

	fflush(stderr);
	mempool_init(pool, nmemb, size);

	return pool;
}

void mempool_free(struct mempool * pool)
{
	__spinlock_destroy(&pool->spinlock);
}

