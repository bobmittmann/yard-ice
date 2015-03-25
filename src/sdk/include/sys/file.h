/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file sys/file.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_FILE_H__
#define __SYS_FILE_H__

struct file;

struct fileop {
	int (* write)(void * arg, const void * buf, int count);
	int (* read)(void * arg, void * buf, int count);
	int (* flush)(void * arg);
	int (* close)(void * arg);
};

struct file {
	void * data;
	const struct fileop * op;
};

#ifdef __cplusplus
extern "C" {
#endif

int file_free(struct file * f);

struct file * file_alloc(void * dev, const struct fileop * op);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_FILE_H__ */

