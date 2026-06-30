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

#ifndef __DYNFS_H__
#define __DYNFS_H__

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#define DYNFS_FNAME_MAX 51
#define DYNFS_CACHE_MIN_SIZE 256

struct dynfs_file_def {
	uint8_t inode;
	uint8_t flags;
	uint16_t size;
	char fname[DYNFS_FNAME_MAX + 1];
	uint8_t * data;
	int (* generate)(void * arg, char * dst, size_t size);
};

struct dynfs;

struct dynfs_dirent {
	uint16_t d_size;
	uint8_t d_ino;
	char d_name[DYNFS_FNAME_MAX];
};

struct dynfs_dir;

struct dynfs_file;

#ifdef __cplusplus
extern "C" {
#endif

int dynfs_init(struct dynfs * fs, const struct dynfs_file_def * dir, 
			   void * arg, void * cache_buf, size_t cache_size);

struct dynfs_dir * dynfs_opendir(struct dynfs * fs, const char * path);

int dynfs_readdir(struct dynfs_dir * dir, struct dynfs_dirent * dirent);

int dynfs_closedir(struct dynfs_dir * dir);

struct dynfs_file * dynfs_fopen(struct dynfs * fs, const char * path);

int dynfs_fread(struct dynfs_file * f, void * dst, off_t offs, size_t max);

int dynfs_fclose(struct dynfs_file * f);

struct dynfs * dynfs_get_instance(unsigned int instance);

#ifdef __cplusplus
}
#endif

#endif /* __DYNFS_H__ */

