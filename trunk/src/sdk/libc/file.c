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

#ifndef FILE_MAX
#define FILE_MAX 8
#endif

static struct file __file[FILE_MAX];

struct file * file_alloc(void * dev, const struct fileop * op)
{
	struct file * f;
	int i;

	for (i = 0; i < FILE_MAX; ++i) {
		f = &__file[i];
		if (f->data == NULL) {
			f->data = dev;
			f->op = op;
			return f;
		}
	}

	return NULL;
}

int file_free(struct file * f)
{
	if (f == NULL)
		return -1;

	if (f->data != NULL) {
		f->data = NULL;
		f->op = &null_fileop;
	}

	return 0;
}

