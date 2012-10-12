/*
 * File:	stdlib.h
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


#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <stddef.h>

#ifndef NULL
#define NULL    ((void*) 0 )
#endif

typedef struct {
	int quot;
	int rem;
} div_t;

#define RAND_MAX 0x7fffffff

#define ENVIRON_MAX 32

#define environ (__environ(({char * __buf[ENVIRON_MAX]; (void *)__buf;})))
char ** __environ(void **) __attribute__ ((const));

#ifdef __cplusplus
extern "C" {
#endif

long strtol(const char * nptr, char ** endptr, int base);

unsigned long strtoul(const char * nptr, char ** endptr, int base);

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

#ifdef __cplusplus
}
#endif

#endif

