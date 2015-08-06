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
 * @file stdio.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STDIO_H__
#define	__STDIO_H__

#define __need_size_t
#include <stddef.h>
#include <stdarg.h>

struct file;

typedef struct file FILE;

extern FILE * stdin;
extern FILE * stdout;
extern FILE * stderr;

#define EOF	(-1)

#ifdef __cplusplus
extern "C" {
#endif

int fputc(int c, FILE * f);

int putchar(int c);

int getchar(void);

int puts(const char * s);

int vfprintf(FILE * f, const char * fmt, va_list ap);

int fprintf(FILE * f, const char * fmt, ...);

int printf(const char * fmt, ...);

int vsnprintf(char * str, size_t size, const char * fmt, va_list ap);

int snprintf(char * str, size_t size, const char *fmt, ...);

int sprintf(char * str, const char * fmt, ...);

FILE * fopen(const char * path, const char * mode);

int fflush(FILE * f);

int feof(FILE * f);

int fclose(FILE * f);

size_t fwrite(const void * ptr, size_t size, size_t nmemb, FILE * f);

size_t fread(void * ptr, size_t size, size_t  nmemb, FILE * f);

char * fgets(char * s, int size, FILE * f);

int fgetc(FILE * f);

int fputs(const char * s, FILE * f);

int scanf(const char * fmt, ...);

int fscanf(FILE * f, const char * fmt, ...);

int sscanf(const char * str, const char * fmt, ...);

int vscanf(const char * fmt, va_list ap);

int vsscanf(const char * str, const char * fmt, va_list ap);

int vfscanf(FILE * f, const char * fmt, va_list ap);

#ifdef __cplusplus
}
#endif

#define putchar(c)  fputc(c, stdout)
#define getchar()   fgetc(stdin)

#endif /* __STDIO_H__ */

