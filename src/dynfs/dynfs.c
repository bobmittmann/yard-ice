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


#include "dynfs-i.h"
#include "cache.h"

#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>
#include <sys/dcclog.h>

int dynfs_fd_alloc(struct dynfs * fs, unsigned int flags)
{
	const struct dynfs_file_def * fsdef;
	struct dynfs_meta * stream;
	uint32_t alloc_bmp;
	int fd;

	if (fs == NULL)
		return -1;

	if ((fsdef = fs->def) == NULL)
		return -1;

	alloc_bmp = fs->alloc.bmp;
	fd = ffs(~alloc_bmp) - 1;
	if ((fd < 0) || (fd >= DYNFS_FILE_OPEN_MAX)) {
		DCC_LOG(LOG_WARNING, "can't allocate file stream");
		return -1;
	}
	
	fs->alloc.bmp = alloc_bmp | (1 << fd);
	stream = &fs->alloc.stream[fd].meta;
	stream->fd = fd;
	stream->flags = flags;
	stream->offs = 0;
	stream->fsdef = fsdef;
	stream->fs = fs;

//	DCC_LOG2(LOG_TRACE, "bmp=%08x fd=%d", fs->alloc.bmp, fd);

//	WARN("stream=%08x fsdef=%08x", (uintptr_t)stream, (uintptr_t)fsdef);

	return fd; 
}

int dynfs_meta_fd_get(struct dynfs_meta * stream)
{
	struct dynfs * fs;
	int fd;

	if (stream == NULL)
		return -1;

	if ((fs = stream->fs) == NULL)
		return -1;

	if ((fd = stream->fd) >= DYNFS_FILE_OPEN_MAX)
		return -1;

	if ((fs->alloc.bmp & (1 << fd)) == 0)
		return -1;

	return fd;
}

int dynfs_meta_release(struct dynfs_meta * stream)
{
	struct dynfs * fs;
	int fd;

	if (stream == NULL)
		return -1;

	if ((fs = stream->fs) == NULL)
		return -1;

	if ((fd = stream->fd) >= DYNFS_FILE_OPEN_MAX)
		return -1;

	stream->fs = NULL;
	stream->fd = DYNFS_FILE_OPEN_MAX;

	fs->alloc.bmp &= ~(1 << fd);

//	DCC_LOG2(LOG_TRACE, "bmp=%08x fd=%d", fs->alloc.bmp, fd);

	return fd;
}

struct dynfs_dir * dynfs_opendir(struct dynfs * fs, const char * path)
{
	struct dynfs_dir * dir;
	int fd;

	if ((fd = dynfs_fd_alloc(fs, DYNFS_STREAM_DIR)) < 0)
		return NULL;

	dir = &fs->alloc.stream[fd].dir;

//	DCC_LOG2(LOG_TRACE, "dir=%08x fd=%d", (uintptr_t)dir, fd);

	return dir; 
}

int dynfs_readdir(struct dynfs_dir * dir, struct dynfs_dirent * dirent)
{
	const struct dynfs_file_def * def;
	int idx;
	int fd;

	if ((fd = dynfs_meta_fd_get(&dir->meta)) < 0)
		return -1;

	if ((def = dir->meta.fsdef) == NULL) {
		/* empty filesystemn */
		return 0;
	}
	idx = dir->meta.offs; 
	def += idx;

//	DCC_LOG3(LOG_TRACE, "dir=%08x fd=%d idx=%d", (uintptr_t)dir, fd, idx);
	
	if (def->inode == 0) {
		return 0;
	}

	if (dirent != NULL) {
		strcpy(dirent->d_name, def->fname);
		dirent->d_size = def->size;
		dirent->d_ino = def->inode;
	}
	dir->meta.offs = idx + 1;

	return 1;
}

int dynfs_closedir(struct dynfs_dir * dir)
{
	return dynfs_meta_release(&dir->meta);
}

struct dynfs_file_def * dynfs_lookup(struct dynfs * fs, const char * path)
{
	const struct dynfs_file_def * entry;
	int i = 0;

	entry = &fs->def[i];
	while (entry->inode != 0) {
		if (strcmp(path, entry->fname) == 0) {
			return (struct dynfs_file_def *)entry;
		}
		entry = &fs->def[++i];
	}
	return NULL; 
}

uint8_t * dynfs_cache_get_buffer(struct cache_ctl * cache , size_t bytes, int inode)
{

	return NULL;
}

static uint16_t dynfs_genkey(struct dynfs * fs)
{
	fs->cache.seed = fs->cache.seed * 25385 + 25385;
    return fs->cache.seed;
}

struct dynfs_file * dynfs_fopen(struct dynfs * fs, const char * path)
{
	const struct dynfs_file_def * fsdef;
	struct dynfs_file * file;
	int fd;

	if ((fd = dynfs_fd_alloc(fs, DYNFS_STREAM_FILE)) < 0)
		return NULL;

	file = &fs->alloc.stream[fd].file;

	if ((fsdef = dynfs_lookup(fs, path)) == NULL) {
		dynfs_meta_release(&file->meta);
		return NULL;
	}

//	INF("file=%08x fd=%d", (uintptr_t)file, fd);
	
	file->cache_key  = dynfs_genkey(fs);
	file->meta.fsdef = fsdef;
	
	//file->len = fsdef->generate(fs->arg, fs->cache.buf, fs->cache.size); 
	//fs->cache.key = fs->cache.key;

//	DCC_LOG1(LOG_TRACE, "key=%d", file->cache_key);

	return file;
}

int dynfs_fclose(struct dynfs_file * f)
{
	int ret = dynfs_meta_release(&f->meta);
	if (ret >= 0) {
		f->cache_key = 0;
		f->len = 0;
	}
	return ret; 
}


int dynfs_fread(struct dynfs_file * f, void * dst, off_t offs, size_t max)
{
	struct dynfs * fs;
	char * cp;
	int rem;
	int len;
	int fd;

	if ((fd = dynfs_meta_fd_get(&f->meta)) < 0)
		return -1;

	fs = f->meta.fs;

	if (fs->cache.key != f->cache_key) {
		const struct dynfs_file_def * fdef = f->meta.fsdef;
		f->len = fdef->generate(fs->arg, fs->cache.buf, fs->cache.size); 
		fs->cache.key = f->cache_key;
	}

	cp = (char *)fs->cache.buf;
	len = f->len;
	cp += offs;
	rem = len - offs;
	rem = MIN(rem, max);
	f->meta.offs = offs + rem; 

//	DCC_LOG3(LOG_TRACE, "offs=%d len=%d rem=%d", offs, len, rem);
	memcpy(dst, cp, rem);

	return rem;
}

int dynfs_init(struct dynfs * fs, const struct dynfs_file_def * fsdef, 
			   void * arg, void * cache_buf, size_t cache_size)
{
	assert(fs != NULL);

	fs->cache.buf = cache_buf;
	fs->cache.size = cache_size;
	fs->cache.key = 0;
	fs->cache.seed = 0xbaba;

	/* bitmap of open files */
	fs->alloc.bmp = 0xffffffff << DYNFS_FILE_OPEN_MAX;
	/* bitmap of open directories */
	fs->def = fsdef;
	fs->arg = arg;

	if (fsdef == NULL) {
		DCC_LOG(LOG_WARNING, "empty filesystem!");
	}
	return 0;
}


#define DYNFS_INSTANCE_MAX 1

struct dynfs dynfs_instance[DYNFS_INSTANCE_MAX];

struct dynfs * dynfs_get_instance(unsigned int instance)
{
	struct dynfs * fs;

	if (instance >= DYNFS_INSTANCE_MAX)
		return NULL;

	fs = &dynfs_instance[instance];

	return fs;
}

