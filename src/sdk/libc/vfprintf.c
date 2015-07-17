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
 * @file vfprintf.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <stdlib.h>
#include <assert.h>

#ifndef PRINTF_ENABLE_LEFT_ALIGN
#define PRINTF_ENABLE_LEFT_ALIGN 1
#endif

#ifndef PRINTF_ENABLE_LARGE_PADDING
#define PRINTF_ENABLE_LARGE_PADDING 1
#endif

#ifndef PRINTF_ENABLE_FAST
#define PRINTF_ENABLE_FAST 1
#endif

#ifndef PRINTF_ENABLE_UNSIGNED
#define PRINTF_ENABLE_UNSIGNED 1
#endif

#ifndef PRINTF_ENABLE_POINTER
#define PRINTF_ENABLE_POINTER 1
#endif

#ifndef PRINTF_ENABLE_LONG
#define PRINTF_ENABLE_LONG 1
#endif

#ifndef PRINTF_ENABLE_ARG_WIDTH
#define PRINTF_ENABLE_ARG_WIDTH 1
#endif

int uint2dec(char * s, unsigned int val);
int uint2hex(char * s, unsigned int val);

int ull2dec(char * s, unsigned long long val);
int ull2hex(char * s, unsigned long long val);

#if PRINTF_ENABLE_LONG
#define BUF_LEN 22
#else
#define BUF_LEN 12
#endif

#define PERCENT 0x01
#define WIDTH 0x02
#define ZERO 0x04
#define LEFT 0x08
#define SIGN 0x10
#define LONG 0x20
#define LONG2 0x40


#if (PRINTF_ENABLE_LONG)
#undef PRINTF_ENABLE_LARGE_PADDING
#define PRINTF_ENABLE_LARGE_PADDING 1
#endif

const char __zeros[]  = {  
#if (PRINTF_ENABLE_LARGE_PADDING)
	'0', '0', '0', '0', '0', '0', '0', '0', 
	'0', '0', '0', '0', '0', '0', '0', '0', 
	'0', '0', '0', '0', 
#endif
	'0', '0', '0', '0', '0', '0', '0', '0', 
	'0', '0', '0', '0', };
	
const char __blanks[] = {
#if (PRINTF_ENABLE_LARGE_PADDING)
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
#endif
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

int vfprintf(struct file * f, const char * fmt, va_list ap)
{
	char buf[BUF_LEN];
	int flags;
	int cnt;
	int c;
	int w;
	int n;
	int r;
	char * cp;
	union {
		void * ptr;
		unsigned int n;
		int i;
#if (PRINTF_ENABLE_LONG)
		unsigned long long ull;
		long long ll;
#endif
	} val;

	assert(f != NULL);
	assert(fmt != NULL);

	n = 0;
	w = 0;
	cnt = 0;
#if (PRINTF_ENABLE_FAST)
	cp = (char *)fmt;
#endif
	for (flags = 0; (c = *fmt++); ) {
		if (flags == 0) {
			if (c == '%') {
				w = 0;
				flags = PERCENT;
#if (PRINTF_ENABLE_FAST)
				if (n) {
					n = f->op->write(f->data, cp, n);
					cp = (char *)fmt;
					cnt += n;;
					n = 0;
				}
#endif
				continue;
			}

#if (PRINTF_ENABLE_FAST)
			n++;
#else
			buf[0] = c;
			cnt += f->op->write(f->data, buf, 1);
#endif
			continue;
		}

		if ((c >= '0') && (c <= '9')) {
			if (!(flags & WIDTH)) {
				flags |= WIDTH;
				if (c == '0') {
					flags |= ZERO;
					continue;
				}
			}
			/* w = w * 10 + c - '0' */
			w = (((w << 2) + w) << 1) + (c - '0');
			continue;
		}

#if (PRINTF_ENABLE_LEFT_ALIGN)
		if (c == '-') {
			flags |= LEFT;
			continue;
		}
#else
		if (c == '-')
			continue;
#endif

#if (PRINTF_ENABLE_ARG_WIDTH)
		if (c == '*') {
			w = va_arg(ap, int);
			continue;
		}
#endif

#if (PRINTF_ENABLE_LONG)
		if (c == 'l') {
			flags |= (flags & LONG) ? LONG2 : LONG;
			continue;
		}
#endif

		if (c == 'c') {
			c = va_arg(ap, int);
			goto print_char;
		}

		if (c == 'd') {
			cp = buf;
#if PRINTF_ENABLE_LONG
			if (flags & LONG2) {
				val.ll = va_arg(ap, long long);
				if (val.ll < 0) {
					buf[0] = '-';
					val.ll = -val.ll;
					flags |= SIGN;
					cp++;
					w--;
				}
				n = ull2dec(cp, val.ull);
			} else
#endif
			{
				val.i = va_arg(ap, int);
				if (val.i < 0) {
					buf[0] = '-';
					val.i = -val.i;
					flags |= SIGN;
					cp++;
					w--;
				}
				n = uint2dec(cp, val.n);
			}
			goto print_buf;
		}

		if (c == 'x') {
			cp = buf;
#if PRINTF_ENABLE_LONG
			if (flags & LONG2) {
				val.ull = va_arg(ap, unsigned long long);
				n = ull2hex(cp, val.ull);
			} else
#endif
			{
				val.n = va_arg(ap, unsigned int);
#if (PRINTF_ENABLE_POINTER)
hexadecimal:
#endif
				n = uint2hex(cp, val.n);
			}
			goto print_buf;
		}

		if (c == 's') {
			cp = va_arg(ap, char *);
			n = 0;
			if (cp == NULL) {
				/* XXX: Null pointer! */
				goto print_buf;
			}
			while (cp[n] != '\0')
				n++;
			goto print_buf;
		}

#if (PRINTF_ENABLE_UNSIGNED)
		if (c == 'u') {
			cp = buf;
#if PRINTF_ENABLE_LONG
			if (flags & LONG2) {
				val.ull = va_arg(ap, unsigned long long);
				n = ull2dec(cp, val.ull);
			} else
#endif
			{
				val.n = va_arg(ap, unsigned int);
				n = uint2dec(cp, val.n);
			}
			goto print_buf;
		}
#endif

#if (PRINTF_ENABLE_POINTER)
		if (c == 'p') {
			val.ptr = va_arg(ap, void *);
			w = 8;
			flags |= ZERO;
			goto hexadecimal;
		}
#endif
	
		flags = 0;
		w = 0;

print_char:
		cp = buf;
		buf[0] = c;
		n = 1;

print_buf:
#if (PRINTF_ENABLE_LEFT_ALIGN)
		if (!(flags & LEFT) && (w > n)) {
#else
		if (w > n) {
#endif
			if (flags & ZERO) {
				if (flags & SIGN) {
					flags &= ~SIGN;
					cnt += f->op->write(f->data, buf, 1);
				}
				r = f->op->write(f->data, __zeros, w - n);
			} else {
				r = f->op->write(f->data, __blanks, w - n);
			}
			cnt += r;
		}

		if (flags & SIGN) {
			cnt += f->op->write(f->data, buf, 1);
		}

		cnt += f->op->write(f->data, cp, n);

#if (PRINTF_ENABLE_LEFT_ALIGN)
		if ((flags & LEFT) && (w > n)) {
			r = f->op->write(f->data, __blanks, w - n);
			cnt += r;
		}
#endif

		flags = 0;
		w = 0;

#if (PRINTF_ENABLE_FAST)
		cp = (char *)fmt;
		n = 0;
#endif
	}

#if (PRINTF_ENABLE_FAST)
	if (n) {
		r = f->op->write(f->data, cp, n);
		cnt+= r;;
	}
#endif

	return cnt;
}

