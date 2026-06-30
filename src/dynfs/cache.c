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
 * @file cache.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <fixpt.h>

#include "dynfs-i.h"

#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>

#ifndef CACHE_BLK_BITS 
#define CACHE_BLK_BITS 5
#endif

#define CACHE_BLK_SIZE (1 << CACHE_BLK_BITS)

struct cache_chunk {
	uint16_t nblk; /* how many block in the chunk */
	uint16_t key; /* fs back reference */
	uint8_t data[];
};

struct cache_blk {
	union {
		uint8_t u8[CACHE_BLK_SIZE];
		uint32_t u32[CACHE_BLK_SIZE / 4];
		uint64_t u64[CACHE_BLK_SIZE / 8];
		struct cache_chunk chunk;
	};
};

#define CACHE_CHUNK_MAX (CACHE_BLK_SIZE / sizeof(struct cache_ctl)) 
#define CHUNK_HDR_SIZE (sizeof(struct cache_chunk)) 

void __blkcpy(struct cache_ctl * cache, int to, int from)
{
	uint64_t *src = cache->blk[to].u64;
	uint64_t *dst = cache->blk[from].u64;
	int i;
	for (i = 0; i < CACHE_BLK_SIZE/8; ++i)
		dst[i] = src[i];
}

static inline unsigned int __bytes2blk(unsigned int bytes)
{
	/* ceiling */
	return (bytes + CACHE_BLK_SIZE + CHUNK_HDR_SIZE - 1) >> CACHE_BLK_BITS;
}

static inline size_t __blk2bytes(unsigned int nblk)
{
	return (nblk << CACHE_BLK_BITS);
}

static inline int __blk2offs(struct cache_ctl * cache, struct cache_blk * blk)
{
	return blk - cache->blk;
}

size_t cache_chunk_size(struct cache_chunk * chunk)
{
	return __blk2bytes(chunk->nblk);
}

void * cache_chunk_buffer(struct cache_chunk * chunk)
{
	return (void *)chunk->data;
}

/*
 * Cache allocation
 * The blocks are allocated by first releasing enough chunks to 
 * fit the requested size. The free space is then consolidated by
 * relocating them to the top.
 * +=========+
 * |         |
 * |  Alloc  |  
 * |         |
 * +---------+ --------+
 * |  Alloc  |
 * |         |
 * +---------+
 * |  Free   | < offs  |
 * |         |        nblk
 * |         |         |
 *
 * */

int cache_init(struct cache_ctl * cache, void * cache_buf, size_t cache_size)
{
	int nblk;
	assert(cache_size > DYNFS_CACHE_MIN_SIZE);

	nblk = cache_size >> CACHE_BLK_BITS;
	cache->blk = ((struct cache_blk *)cache_buf);
	cache->mask = nblk - 1;
	DBG("cache_size=%d nblk=%d size=%d", cache_size, nblk, 
		nblk * sizeof(struct cache_blk));
	cache->nblk = nblk;
	cache->free = nblk;
	cache->head = 0;
	cache->tail = 0;
	/* just one block of size nblk in the list */
	cache->seq = 0;

	return 0;
}

/* invalidate a cache entry */
void cache_chunk_delete(struct cache_ctl * cache, struct cache_chunk * chunk)
{
	struct cache_blk * blk;
	unsigned int offs;
	uint32_t head;
	uint32_t tail;
	uint32_t mask;
	int32_t free;

	offs = __blk2offs(cache, (struct cache_blk *)chunk);
	(void)offs;
	tail = cache->tail;
	head = cache->head;
	mask = cache->mask;
	free = cache->free;

	INF("tail=%d head=%d", tail, head);
	/* get the first entry from the tail */
	blk = &cache->blk[tail & mask];
	/* discard all the entries up to ours */
	while (tail != head) {
		if (tail == head) {
			INFS("tail == head");
			break;
		}
//		int ti = (tail & mask);
		free += blk->chunk.nblk;
		tail += blk->chunk.nblk;
	//	if (offs == ti) {
	//		break;
	//	}
		blk = &cache->blk[tail & mask];
	}

	/* sanity check */
	assert(cache->nblk + tail - head == free);

	cache->tail = tail;
	cache->free = free;
}

void cache_free(struct cache_ctl * cache, void * buf)
{	
	struct cache_chunk * chunk;

	chunk = ((struct cache_chunk *)buf - 1);
	cache_chunk_delete(cache, chunk);
}

struct cache_chunk * cache_chunk_lookup(struct cache_ctl * cache, int key)
{
	struct cache_blk * blk;
	uint32_t head;
	uint32_t tail;
	uint32_t mask;

	tail = cache->tail;
	head = cache->head;
	mask = cache->mask;

	while (tail != head) {
		blk = &cache->blk[tail & mask];
		if (blk->chunk.key == key)
			return &blk->chunk;
		tail += blk->chunk.nblk;
	}

	return NULL;
}

struct cache_chunk * cache_chunk_new(struct cache_ctl * cache, 
									 size_t bytes, int key)
{
	struct cache_blk * blk;
	uint32_t head;
	uint32_t tail;
	uint32_t mask;
	int32_t free;
	int nblk;

	tail = cache->tail;
	head = cache->head;
	mask = cache->mask;
	free = cache->free;

	nblk = __bytes2blk(bytes);

	if (nblk > cache->nblk) {
		/* request is too large */
		return NULL;
	}


/*                             head    tail
 *                              |        |
 *                              v        v 
 *	 {[###########][###########]        [##########]}
 *
 *    tail                     head    
 *     |                        |        
 *     v                        v         
 *	 {[###########][###########]                    }
 * 
 *                 tail        head    
 *                  |           |        
 *                  v           v         
 *	 {            ][###########]                    }
 */
	head = 0;
	tail = 0;
	
	INF("tail=%d head=%d", tail, head);

	while ((cache->nblk + tail - head) < nblk) {
		/* discard entries until we have enough room  */
		if (tail == head) {
			INFS("tail == head");
			break;
		}
		/* discard old entries */
		blk = &cache->blk[tail & mask];
		free += blk->chunk.nblk;
		tail += blk->chunk.nblk;
	}

	int hi = (head & mask);
	int ti = (tail & mask);
	/* is the free space split by some chunks in the middle and
	 * there is no space left at the end ? */
	if ((ti < hi) && ((cache->nblk - hi) < nblk)) {
		/* move the head to the top and discard all */
		/* buffer empty, move to the top by adding to the head */
		head += cache->nblk - hi;
		tail = head;
		INFS("tail = head = top");
	}


	blk = &cache->blk[head & mask];
	blk->chunk.key = key;
	blk->chunk.nblk = nblk;
	free -= nblk;

	cache->tail = tail;
	cache->head = head;
	cache->free = free;

	return &blk->chunk;
}

void * cache_alloc(struct cache_ctl * cache, size_t bytes, int key)
{
	struct cache_chunk * chunk;
	chunk = cache_chunk_new(cache, bytes, key);
	if (chunk == NULL)
		return NULL;

	return (void*)&chunk->data;
}

