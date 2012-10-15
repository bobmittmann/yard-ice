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
 * @file sprintf.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/file.h>

struct str_buf {
	char * ptr;
	char * max;
};

static int sprintf_write(struct str_buf * str, const void * buf, int len)
{
	unsigned int n;

	n = str->max - str->ptr;

	n = n < (unsigned int)len ? n : len;

	memcpy(str->ptr, buf, n);
	str->ptr += n;

	return len;
}

static const struct fileop sprintf_fileop = {
	.write = (int (*)(void *, const void *, int))sprintf_write
};

int vsnprintf(char * str, size_t size, const char *fmt, va_list ap)
{
	struct str_buf buf;
    struct file f;
	int n;

	buf.ptr = str;
	buf.max = str + size;

	f.data = (void *)&buf;
	f.op = &sprintf_fileop;

	n = vfprintf(&f, fmt, ap);

	str[n] = '\0';

	return n;
}

