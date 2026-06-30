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
 * @file dynfs-i.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdlib.h>
#include <stdbool.h>

/*
 * [########][######]        [####]
 *
 */

struct cache_chunk;
struct cache_blk;

struct cache_ctl {
	struct cache_blk * blk;
	uint32_t head;
	uint32_t tail;
	uint16_t mask;

	uint16_t nblk; /* number of blocks in the cache buffer */
	uint16_t free; /* number of blocks free at the end */
	uint32_t seq;
};

#ifdef __cplusplus
extern "C" {
#endif

int cache_init(struct cache_ctl * cache, void * cache_buf, size_t cache_size);

/* invalidate a cache entry */
void cache_chunk_delete(struct cache_ctl * cache, struct cache_chunk * chunk);

struct cache_chunk * cache_chunk_new(struct cache_ctl * cache, size_t bytes, 
									 int key);

struct cache_chunk * cache_chunk_lookup(struct cache_ctl * cache, int key);

unsigned int cache_chunk_size(struct cache_chunk * chunk);

void * cache_chunk_buffer(struct cache_chunk * chunk);

void cache_free(struct cache_ctl * cache, void * buf);

void * cache_alloc(struct cache_ctl * cache, size_t size, int key);

#ifdef __cplusplus
}
#endif

#endif /* __CACHE_H__ */

