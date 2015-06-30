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
 * @file stdlib.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifndef __STDLIB_H__
#define __STDLIB_H__

#define __need_size_t
#define __need_wchar_t
#define __need_NULL
#include <stddef.h>

typedef struct {
	int quot;
	int rem;
} div_t;

#define RAND_MAX 0x7fffffff

extern char ** environ;

#ifdef __cplusplus
extern "C" {
#endif

long strtol(const char * nptr, char ** endptr, int base);

unsigned long strtoul(const char * nptr, char ** endptr, int base);

unsigned long long strtoull(const char * nptr, char ** endptr, int base);

int abs(int num);

int atoi(const char *nptr);

long atol(const char *nptr);

int rand(void);

void srand(unsigned int seed);

div_t div(int numer, int denom);

char * getenv(const char *name);

int setenv(const char *name, const char * value, int overwrite);

int unsetenv(const char *name);

int clearenv(void);

void abort(void) __attribute__ ((__noreturn__));

/* FIXME: not implemented */

void exit(int status);

void * malloc(size_t size);

void free(void *ptr);

void *calloc(size_t nmemb, size_t size);

void *realloc(void *ptr, size_t size);

double strtod(const char *nptr, char **endptr);

float strtof(const char *nptr, char **endptr);

#ifdef __cplusplus
}
#endif

#endif

