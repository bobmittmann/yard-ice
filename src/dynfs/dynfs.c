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
 * @file dynfs.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <fixpt.h>

#include "dynfs-i.h"
#include "cache.h"

#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>


/* Cache line */
struct dynfs_cache_entry {
	uint8_t offs;
	uint8_t nblk;
	uint8_t rem;
	uint8_t seq;
};

struct dynfs_file {
	const struct dynfs_dirent * dirent;
	void * buf;
	void * arg;
	uint32_t len;
	uint32_t chunk_size;
	int key;
	struct cache_ctl * cache;
};

struct dynfs {
	struct cache_ctl cache;
	struct dynfs_file file[DYNFS_FILE_OPEN_MAX];
	struct {
		uint8_t cnt;
		uint32_t bmp;
	} file_open;

	const struct dynfs_dirent * dir;
};

int dynfs_init(struct dynfs * fs, const struct dynfs_dirent * dir, 
			   void * cache_buf, size_t cache_size)
{
	assert(fs != NULL);
	assert(dir != NULL);
	assert(cache_buf != NULL);
	assert(cache_size > DYNFS_CACHE_MIN_SIZE);

	cache_init(&fs->cache, cache_buf, cache_size);
	/* number of open files */
	fs->file_open.cnt = 0;
	/* bitmap of open files */
	fs->file_open.bmp = 0;

	return 0;
}

struct dynfs_dirent * dynfs_lookup(struct dynfs * fs, const char * path)
{
	const struct dynfs_dirent * dirent;
	int i = 0;

	dirent = &fs->dir[i];
	while (dirent->inode != 0) {
		if (strcmp(path, dirent->fname) == 0) {
			return (struct dynfs_dirent *)dirent;
		}
		dirent = &fs->dir[++i];
	}
	return NULL; 
}

uint8_t * dynfs_cache_get_buffer(struct cache_ctl * cache , size_t bytes, int inode)
{

	return NULL;
}

struct dynfs_file * dynfs_fopen(struct dynfs * fs, const char * path)
{
	struct dynfs_file * file;
	struct dynfs_dirent * dirent;
	uint32_t free_bmp;
	int i;

	if ((dirent = dynfs_lookup(fs, path)) == NULL) {
		return NULL;
	}
	free_bmp = ~fs->file_open.bmp;

	if ((i = __builtin_ffs(free_bmp)) == 32) {
		return NULL;	
	}

	fs->file_open.bmp |= (1 << i);
	file = &fs->file[i];

	file->buf = NULL;
	file->arg = NULL;
	file->chunk_size = 0;
	file->len = 0;
	file->dirent = dirent;
	file->cache = &fs->cache;
	file->key = dirent->inode;
	return file;
}

int dynfs_fread(struct dynfs_file * f, void * dst, off_t offs, size_t max)
{
	struct cache_ctl * cache = f->cache;
	struct cache_chunk * chunk;
	bool generate = false;
	size_t size = f->dirent->size;
	int rem;
	int len;

	if ((chunk = cache_chunk_lookup(cache, f->key)) == NULL) {
		if ((chunk = cache_chunk_new(cache, size, f->key)) == NULL)
			return 0;
		generate = true;
	} else if (cache_chunk_size(chunk) < size) {
		/* too small, discarding */
		cache_chunk_delete(cache, chunk);
		if ((chunk = cache_chunk_new(cache, size, f->key)) == NULL)
			return 0;
		generate = true;
	}
	char * cp = cache_chunk_buffer(chunk);
	if (generate) {
		len = f->dirent->generate(f->arg, cp, size); 
		f->len = len;
	} else {
		len = f->len;
	}
	
	cp += offs;
	len -= offs;
	rem = MIN(len, max);
	memcpy(dst, cp, rem);

	return rem;
}

#define DYNFS_INSTANCE_MAX 1

struct dynfs dynfs_instance[DYNFS_INSTANCE_MAX];

struct dynfs * dynfs_alloc(unsigned int instance)
{
	struct dynfs * fs;

	if (instance >= DYNFS_INSTANCE_MAX)
		return NULL;

	fs = &dynfs_instance[instance];
	memset(fs, 0, sizeof(struct dynfs));

	return fs;
}

