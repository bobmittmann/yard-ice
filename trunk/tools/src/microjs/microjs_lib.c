/* 
 * Copyright(C) 2014 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the MicroJs
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
 * @file microjs-i.h
 * @brief Syntax-directed translation compiler
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#define __MICROJS_I__
#include "microjs-i.h"

#include <string.h>
#include <time.h>

int32_t __rand(struct microjs_env * env, int32_t argv[], int argc) 
{
	int32_t * retv = argv + argc - 1;

	retv[0] = rand();

	return 1;
};

int32_t __srand(struct microjs_env * env, int32_t argv[], int argc) 
{
	srand(argv[0]);
	return 0;
};

int32_t __isqrt(struct microjs_env * env, int32_t argv[], int argc)
{
	int32_t * retv = argv + argc - 1;
	uint32_t x = argv[0];
	uint32_t rem = 0;
	uint32_t root = 0;
	int i;

	for (i = 0; i < 16; ++i) {
		root <<= 1;
		rem = ((rem << 2) + (x >> 30));
		x <<= 2;
		root++;
		if (root <= rem) {
			rem -= root;
			root++;
		} else
			root--;
	}

	retv[0] = root >> 1;

	return 1;
}	

int32_t __memrd(struct microjs_env * env, int32_t argv[], int argc)
{
	int32_t * retv = argv + argc - 1;
	uint32_t addr = argv[0];

	if (addr >= 256)
		return -1;

	retv[0] = env->data[addr];

	return 1;
}	

int32_t __ilog2(struct microjs_env * env, int32_t argv[], int argc)
{
	int32_t * retv = argv + argc - 1;
	const uint8_t log2_debruijn_index[32] = {
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9 };
	int32_t x = argv[0];

	x |= x >> 1; 
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x = (x >> 1) + 1;
	x = (x * 0x077cb531UL) >> 27;
	retv[0] = log2_debruijn_index[x];
	return 1;
}	

int32_t __write(struct microjs_env * env, int32_t argv[], int argc)
{
	int i;

	for(i = argc - 1; i >= 0; --i) {
		if (i < argc - 1)
			printf(", ");
		printf("%d", argv[i]);
	}
	printf("\n");
	return 0;
}	

int32_t __time(struct microjs_env * env, int32_t argv[], int argc)
{
	int32_t * retv = argv + argc - 1;
	retv[0] = (int32_t)time(NULL);
	return 1;
}	

#define BUF_LEN 12
#define PERCENT 0x01
#define WIDTH   0x02
#define ZERO    0x04
#define LEFT    0x08
#define SIGN    0x10
#define LONG    0x20

static const char zeros[]  = {  
	'0', '0', '0', '0', '0', '0', '0', '0', 
	'0', '0', '0', '0', '0', '0', '0', '0', 
	'0', '0', '0', '0', '0', '0', '0', '0', 
	'0', '0', '0', '0', '0', '0', '0', '0', };
	
static const char blanks[] = {
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static const uint32_t __div_tab[11] = {
	4000000000U, 
	800000000, 
	80000000, 
	8000000, 
	800000, 
	80000, 
	8000, 
	800, 
	80, 
	8, 
	0 };

static int uint2dec(char * s, uint32_t val)
{
	uint32_t * dvp;
	uint32_t dv;
	int n;
	int c;
	int i;

	if (!val) {
		*s++ = '0';
		*s = '\0';
		return 1;
	}

	dvp = (uint32_t *)__div_tab;
	n = 0;
	c = 0;
	i = 4;
	while ((dv = *dvp++)) {	
		c = 0;
		for (; (i); i >>= 1) {
			if (val >= dv) {
				val -= dv;
				c += i;
			}
			dv >>= 1;
		}
		if ((c) || (n)) {
			*s++ = c + '0';
			n++;
		}
		i = 8;
	};
	*s = '\0';

	return n;
}

static const char __hextab[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

static int uint2hex(char * s, uint32_t val)
{
	int n;
	int c;
	int i;

	/* value is zero ? */
	if (!val) {
		*s++ = '0';
		*s = '\0';
		return 1;
	}

	n = 0;
	for (i = 0; i < (sizeof(uint32_t) * 2); i++) {
		c = val >> ((sizeof(uint32_t) * 8) - 4);
		val <<= 4;
		if ((c != 0) || (n != 0)) {
			s[n++] = __hextab[c];
		}
	}

	s[n] = '\0';

	return n;
}


int32_t __printf(struct microjs_env * env, int32_t argv[], int argc)
{
	char buf[BUF_LEN];
	const char * fmt;
	int flags;
	int cnt;
	int c;
	int w;
	int n;
	char * cp;
	union {
		void * ptr;
		uint32_t n;
		int i;
	} val;
	int i = argc;

	fmt = str(argv[--i]);
	#define _va_arg(AP, TYPE) ((i > 0) ? argv[--i] : 0)

	n = 0;
	w = 0;
	cnt = 0;
	cp = (char *)fmt;
	for (flags = 0; (c = *fmt++); ) {
		if (flags == 0) {
			if (c == '%') {
				w = 0;
				flags = PERCENT;
				if (n) {
					fwrite(cp, n, 1, env->fout);
					cp = (char *)fmt;
					cnt += n;;
					n = 0;
				}
				continue;
			}
			n++;
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

		if (c == '-') {
			flags |= LEFT;
			continue;
		}

		if (c == '*') {
			w = _va_arg(ap, int);
			continue;
		}

		if (c == 'c') {
			c = _va_arg(ap, int);
			goto print_char;
		}

		if (c == 'd') {
			cp = buf;
			{
				val.i = _va_arg(ap, int);
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
			val.n = _va_arg(ap, unsigned int);
			n = uint2hex(cp, val.n);
			goto print_buf;
		}

		if (c == 's') {
			cp = (char *)str(_va_arg(ap, char *));
			n = strlen(cp);
			goto print_buf;
		}

		if (c == 'u') {
			cp = buf;
			val.n = _va_arg(ap, unsigned int);
			n = uint2dec(cp, val.n);
			goto print_buf;
		}

		flags = 0;
		w = 0;

print_char:
		cp = buf;
		buf[0] = c;
		n = 1;

print_buf:
		if (!(flags & LEFT) && (w > n)) {
			if (flags & ZERO) {
				if (flags & SIGN) {
					flags &= ~SIGN;
					fwrite(buf, 1, 1, env->fout);
				}
				fwrite(zeros, w - n, 1, env->fout);
			} else {
				fwrite(blanks, w - n, 1, env->fout);
			}
			cnt += w - n;
		}

		if (flags & SIGN) {
			fwrite(buf, 1, 1, env->fout);
			cnt++;
		}

		fwrite(cp, n, 1, env->fout);
		cnt += n;

		if ((flags & LEFT) && (w > n)) {
			fwrite(blanks, w - n, 1, env->fout);
			cnt += w - n;
		}

		flags = 0;
		w = 0;

		cp = (char *)fmt;
		n = 0;
	}

	if (n) {
		fwrite(cp, n, 1, env->fout);
		cnt+= n;;
	}

	return 0;
}

/* --------------------------------------------------------------------------
   Native (external) call table
   -------------------------------------------------------------------------- */

int32_t (* extern_call[])(struct microjs_env *, int32_t argv[], int argc) = {
	[EXT_RAND] = __rand,
	[EXT_SQRT] = __isqrt,
	[EXT_LOG2] = __ilog2,
	[EXT_WRITE] = __write,
	[EXT_TIME] = __time,
	[EXT_SRAND] = __srand,
	[EXT_PRINT] = __write,
	[EXT_PRINTF] = __printf,
	[EXT_MEMRD] = __memrd,
};

