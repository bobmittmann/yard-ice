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

#ifndef __DYNFS_I_H__
#define __DYNFS_I_H__

#include "dynfs.h"

#include "cache.h"
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fixpt.h>

#define DYNFS_FILE_OPEN_MAX 8

#define DYNFS_STREAM_RW   (1 << 0)
#define DYNFS_STREAM_FILE (1 << 1)
#define DYNFS_STREAM_DIR  (1 << 2)

struct dynfs_meta {
	uint8_t fd;
	uint8_t flags;
	uint16_t offs;
	struct dynfs * fs;
	const struct dynfs_file_def * fsdef;
};

struct dynfs_file {
	struct dynfs_meta meta;
	uint16_t len;
	uint16_t cache_key;
};

struct dynfs_dir {
	struct dynfs_meta meta;
};

struct dynfs {
	struct {
		uint16_t size;
		uint16_t key;
		uint32_t seed;
		char * buf;
	} cache;

	struct {
		union {
			struct dynfs_meta meta;
			struct dynfs_dir dir;
			struct dynfs_file file;
		} stream[DYNFS_FILE_OPEN_MAX];
		uint32_t bmp;
	} alloc;

	void * arg;
	uint8_t ndef;
	const struct dynfs_file_def * def;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DYNFS_I_H__ */

