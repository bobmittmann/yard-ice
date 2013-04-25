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
 * @file sys/null.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_NULL_H__
#define __SYS_NULL_H__

#include <sys/file.h>

extern struct file * null;

const struct fileop null_fileop;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * File IO operations
 */
int null_write(void * arg, const void * buf, int len);

int null_read(void * arg, void * buf, int len);

int null_close(void * arg);

int null_flush(void * arg);

/*
 * Auxiliary open function
 */

#include <stdio.h>

FILE * null_fopen(const char * mode);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_NULL_H__ */

