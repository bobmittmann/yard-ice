/*
 * File:	stdio.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

int fgetc(FILE * f);

int fputc(int c, FILE * f);

int fputs(const char * s, FILE * f);

int putchar(int c);

int getchar(void);

int puts(const char * s);

int vfprintf(FILE * f, const char * fmt, va_list ap);

int fprintf(FILE * f, const char * fmt, ...);

int printf(const char * fmt, ...);

int vsnprintf(char * str, size_t size, const char * fmt, va_list ap);

int snprintf(char * str, size_t size, const char *fmt, ...);

int sprintf(char * str, const char * fmt, ...);

int fflush(FILE * f);

int feof(FILE * f);

int fclose(FILE * f);

char * fgets(char * s, int size, FILE * f);

size_t fwrite(const void * ptr, size_t size, size_t nmemb, FILE * f);

size_t fread(void * ptr, size_t size, size_t  nmemb, FILE * f);

#ifdef __cplusplus
}
#endif

extern inline int putchar(int c) {
	return fputc(c, stdout);
};

extern inline int getchar(void) {
	return fgetc(stdin);
};

#endif /* __STDIO_H__ */

