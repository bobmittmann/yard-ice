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
 * @file null.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/file.h>
#include <sys/null.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/dcclog.h>

int null_write(void * arg, const void * buf, int len)
{
	return len;
}

int null_read(void * arg, void * buf, int len)
{
	return 0;
}

int null_flush(void * arg)
{
	return 0;
}

int null_close(void * arg)
{
	DCC_LOG(LOG_TRACE, "...");
	return 0;
}

const struct fileop null_fileop = {
	.write = (int (*)(void *, const void *, int))null_write,
	.read = (int (*)(void *, void *, int))null_read,
	.flush = (int (*)(void *))null_flush,
	.close = (int (*)(void *))null_close
};

const struct file null_file = {
	.data = NULL, 
	.op = &null_fileop
};

FILE * null_fopen(const char * mode)
{
	return (FILE *)&null_file;
}

