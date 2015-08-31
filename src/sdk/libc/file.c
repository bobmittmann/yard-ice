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
 * @file file.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <sys/file.h>
#include <sys/null.h>
#include <arch/cortex-m3.h>

#ifdef CONFIG_H
#include "config.h"
#endif

#ifndef FILE_DEV_MAX
#define FILE_DEV_MAX 8
#endif

static struct file __file_dev_[FILE_DEV_MAX];

struct file * file_alloc(void * __dev, const struct fileop * __op)
{
	struct file * f;
	int i;

	if (__op == NULL)
		return NULL;

	for (i = 0; i < FILE_DEV_MAX; ++i) {
		f = &__file_dev_[i];
		while ((void *)__ldrex((uint32_t *)(&f->op)) == NULL) {
			if (__strex((uint32_t *)(&f->op), (uint32_t)__op) == 0) {
				f->data = __dev;
				return f;
			}
		}
	}

	return NULL;
}

int file_free(struct file * __f)
{
	if (__f == NULL)
		return -1;

	if (__f->data != NULL) {
		__f->data = NULL;
		__f->op = &null_fileop;
	}

	return 0;
}

