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

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdbool.h>

#define DYNFS_FNAME_MAX 51
#define DYNFS_FILE_OPEN_MAX 1
#define DYNFS_CACHE_MIN_SIZE 256

struct dynfs_dirent {
	uint8_t inode;
	uint8_t flags;
	uint16_t size;
	char fname[DYNFS_FNAME_MAX + 1];
	uint8_t * data;
	int (* generate)(void * arg, char * dst, size_t size);
};

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __DYNFS_H__ */

