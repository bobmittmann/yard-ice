/* $Id: snprintf.c,v 2.2 2008/04/17 18:24:49 bob Exp $ 
 *
 * File:	sprintf.c
 * Module:	ulibc
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2008 BORESTE (www.boreste.com). All Rights Reserved.
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

