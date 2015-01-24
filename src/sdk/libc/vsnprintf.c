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
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/file.h>

#include <sys/dcclog.h>

struct str_buf {
	char * ptr;
	char * end;
};

static int sprintf_write(struct str_buf * str, const void * buf, int len)
{
	char * src = (char *)buf;
	char * dst = (char *)str->ptr;
	char * end;

	end = MIN(str->end, str->ptr + len);

	DCC_LOG2(LOG_INFO, "%08x %08x", end, str->end);

	while (dst != end)
		*dst++ = *src++;

	str->ptr += len;

	return len;
}

static const struct fileop sprintf_fileop = {
	.write = (int (*)(void *, const void *, int))sprintf_write
};

/*
   The functions snprintf() and vsnprintf() do not write more than size 
   bytes (including the terminating null byte ('\0')). If the output was 
   truncated due to this limit then the return value is the number of 
   characters (excluding the terminating null byte) which would have been 
   written to the final string if enough space had been available. Thus, 
   a return value of size or more means that the output was 
   truncated. (See also below under NOTES.)
   */

int vsnprintf(char * str, size_t size, const char *fmt, va_list ap)
{
	struct str_buf buf;
    struct file f;
	int n;

	if (str == NULL)
		return -EINVAL;

	if (size == 0)
		return -EINVAL;

	buf.ptr = str;
	buf.end = str + size - 1; /* reserve space for '\0' */

	DCC_LOG3(LOG_INFO, "size=%d str=%08x end=%08x", size, str, buf.end);

	f.data = (void *)&buf;
	f.op = &sprintf_fileop;

	n = vfprintf(&f, fmt, ap);

	str[n] = '\0';

	return n;
}

