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
 * @file string.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifndef __STRING_H__
#define	__STRING_H__

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

long int strtol(const char *nptr, char **endptr, int base);
	
size_t strlen(const char * s);

char * strcat(char * dst, const char * src);

char * strncat(char * dst, const char * src, size_t n);

size_t strlcat(char * dst, const char * src, size_t size);

int strcmp(const char * s1, const char * s2);

int strcasecmp(const char * s1, const char * s2);

int strncmp(const char *s1, const char *s2, size_t n);

char * strchr(const char * s, int c);

char * strrchr(const char * s, int c);

char * strstr(const char * haystack, const char * needle);

char * strcpy(char * dst, const char * src);

char * strncpy(char * dst, const char * src, size_t n);

size_t strlcpy(char * dst, const char * src, size_t size);

char * strtok(char *, const char *);

void * memcpy(void * dst, const void * src, size_t n);

void * memset(void * s, int c, size_t n);

int memcmp(const void * s1, const void * s2, size_t n);

void *memmove(void *dest, const void *src, size_t n);

int ffs(int i);

/* FIXME: not implemented */

char *strerror(int errnum);

#ifdef __cplusplus
}
#endif
#endif /* __STRING_H__ */

