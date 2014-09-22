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



#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "slcdev.h"
#include "slcdev-lib.h"
#include <sys/dcclog.h>

int32_t __rand(struct microjs_env * env, int32_t argv[], int argc) 
{
	int32_t * retv = argv + argc - 1;

	retv[0] = rand();

	return 1;
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

extern const char __zeros[];
extern const char __blanks[];

extern int uint2dec(char * s, uint32_t val);
extern int uint2hex(char * s, uint32_t val);

#define BUF_LEN 12
#define PERCENT 0x01
#define WIDTH   0x02
#define ZERO    0x04
#define LEFT    0x08
#define SIGN    0x10
#define LONG    0x20

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
				fwrite(__zeros, w - n, 1, env->fout);
			} else {
				fwrite(__blanks, w - n, 1, env->fout);
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
			fwrite(__blanks, w - n, 1, env->fout);
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

int32_t __print(struct microjs_env * env, int32_t argv[], int argc)
{
	int i;
	
	for (i = argc - 1; i >= 0; --i) {
		if (i != argc - 1)
			fprintf(env->fout, ", ");
		fprintf(env->fout, "%d", argv[i]);
	}

	fprintf(env->fout, "\n");
	return 0;
}	


/* --------------------------------------------------------------------------
   Devices API
   -------------------------------------------------------------------------- */

static int32_t __state(struct microjs_env * env, int32_t argv[], int argc,
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[0];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	retv[0] = ss_dev_tab[idx].alm ? 1 : 0 |
		ss_dev_tab[idx].tbl ? 2 : 0;

	return 1; /* return the number of return values */
}

static int32_t __alarm(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val > 15)
			return -EXCEPT_INVALID_ALARM_CODE;
	
		ss_dev_tab[idx].alm = val;
	}

	retv[0] = ss_dev_tab[idx].alm;

	return 1; /* return the number of return values */
}

static int32_t __trouble(struct microjs_env * env, int32_t argv[], int argc,
						 unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val > 15)
			return -EXCEPT_INVALID_TROUBLE_CODE;
	
		ss_dev_tab[idx].tbl = val;
	}

	retv[0] = ss_dev_tab[idx].tbl;

	return 1; /* return the number of return values */
}

static int32_t __level(struct microjs_env * env, int32_t argv[], int argc,
						 unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];
	unsigned int lvl = argv[argc - 2];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (lvl >= 4)
		return -EXCEPT_INVALID_LEVEL_VARIABLE; 

	if (argc > 2) {
		unsigned int val = argv[argc - 3];

		if (val >= 256)
			return -EXCEPT_INVALID_LEVEL_VALUE;
	
		ss_dev_tab[idx].lvl[lvl] = val;
	}

	retv[0] = ss_dev_tab[idx].lvl[lvl];

	return 1; /* return the number of return values */
}

static int32_t __pw1(struct microjs_env * env, int32_t argv[], int argc,
						 unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val >= 8000)
			return -EXCEPT_INVALID_PW_VALUE;
	
		ss_dev_tab[idx].pw1 = val;
	}

	retv[0] = ss_dev_tab[idx].pw1;

	return 1; /* return the number of return values */
}

static int32_t __pw2(struct microjs_env * env, int32_t argv[], int argc,
						 unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val >= 8000)
			return -EXCEPT_INVALID_PW_VALUE;
	
		ss_dev_tab[idx].pw2 = val;
	}

	retv[0] = ss_dev_tab[idx].pw2;

	return 1; /* return the number of return values */
}

static int32_t __pw3(struct microjs_env * env, int32_t argv[], int argc,
						 unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val >= 8000)
			return -EXCEPT_INVALID_PW_VALUE;
	
		ss_dev_tab[idx].pw3 = val;
	}

	retv[0] = ss_dev_tab[idx].pw3;

	return 1; /* return the number of return values */
}

static int32_t __pw4(struct microjs_env * env, int32_t argv[], int argc,
						 unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val >= 8000)
			return -EXCEPT_INVALID_PW_VALUE;
	
		ss_dev_tab[idx].pw4 = val;
	}

	retv[0] = ss_dev_tab[idx].pw4;

	return 1; /* return the number of return values */
}

static int32_t __pw5(struct microjs_env * env, int32_t argv[], int argc,
						 unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val >= 8000)
			return -EXCEPT_INVALID_PW_VALUE;
	
		ss_dev_tab[idx].pw5 = val;
	}

	retv[0] = ss_dev_tab[idx].pw5;

	return 1; /* return the number of return values */
}

static int32_t __enabled(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val > 1)
			return -EXCEPT_INVALID_VALUE;
	
		ss_dev_tab[idx].enabled = 1;
	}

	retv[0] = ss_dev_tab[idx].enabled;

	return 1; /* return the number of return values */
}

static int32_t __tst(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val > 1)
			return -EXCEPT_INVALID_VALUE;
	
		ss_dev_tab[idx].tst = 1;
	}

	retv[0] = ss_dev_tab[idx].tst;

	return 1; /* return the number of return values */
}


static int32_t __tbias(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val >= 200)
			return -EXCEPT_INVALID_VALUE;
	
		ss_dev_tab[idx].tbias = (val * 128) / 100;
	}

	retv[0] = (ss_dev_tab[idx].tbias * 100) / 128;

	return 1; /* return the number of return values */
}

static int32_t __ilat(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val > 250)
			return -EXCEPT_INVALID_VALUE;
	
		ss_dev_tab[idx].ilat = 1;
	}

	retv[0] = ss_dev_tab[idx].ilat;

	return 1; /* return the number of return values */
}

static int32_t __imode(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];
	unsigned int icfg;

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	icfg = ss_dev_tab[idx].icfg;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val > 25)
			return -EXCEPT_INVALID_VALUE;

		icfg = (icfg & ~(0x1f)) + val;
		ss_dev_tab[idx].icfg = icfg;
	}

	retv[0] = icfg & 0x1f;

	return 1; /* return the number of return values */
}

static int32_t __irate(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];
	unsigned int icfg;

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	icfg = ss_dev_tab[idx].icfg;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val > 3)
			return -EXCEPT_INVALID_VALUE;
	
		icfg = (icfg & ~(0x07 << 5)) + (val << 5);
		ss_dev_tab[idx].icfg = icfg;
	}

	retv[0] = icfg >> 5;

	return 1; /* return the number of return values */
}

static int32_t __ipre(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	if (argc > 1) {
		unsigned int val = argv[0];

		if (val > 250)
			return -EXCEPT_INVALID_VALUE;
	
		ss_dev_tab[idx].ipre = val;
	}

	retv[0] = ss_dev_tab[idx].ipre;

	return 1; /* return the number of return values */
}

static int32_t __cfg(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	retv[0] = ss_dev_tab[idx].cfg;

	return 1; /* return the number of return values */
}

static int32_t __ap(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	retv[0] = ss_dev_tab[idx].apen;

	return 1; /* return the number of return values */
}

/* Clear group list */
static int32_t __clear(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	unsigned int idx = argv[argc - 1];
	int i;

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	for (i = 0; i < 4; ++i)
		ss_dev_tab[idx].grp[i] = 0;

	return 0; /* return the number of return values */
}

/* Insert device into groups */
static int32_t __insert(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	while (--argc) {
		unsigned int val = argv[argc];
		int i;

		if (val >= 256)
			return -EXCEPT_INVALID_GROUP;

		for (i = 0; i < 4; ++i) {
			if (ss_dev_tab[idx].grp[i] == val)
				break;
		}
		
		if (i < 4)
			continue;

		for (i = 0; i < 4; ++i) {
			if (ss_dev_tab[idx].grp[i] == 0) {
				ss_dev_tab[idx].grp[i] = val;
				break;
			}
		}

		if (i == 4)
			return -EXCEPT_TOO_MANY_GROUPS;
	}

	return 0; /* return the number of return values */
}

static int32_t __remove(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	unsigned int idx = argv[argc - 1];

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	while (--argc) {
		unsigned int val = argv[argc];
		int i;

		if (val >= 256)
			return -EXCEPT_INVALID_GROUP;

		for (i = 0; i < 4; ++i) {
			if (ss_dev_tab[idx].grp[i] == val) {
				ss_dev_tab[idx].grp[i] = 0;
				break;
			}
		}
	}

	return 0; /* return the number of return values */
}

static int32_t __belong(struct microjs_env * env, int32_t argv[], int argc, 
					   unsigned int offs)
{
	int32_t * retv = argv + argc - 1;
	unsigned int idx = argv[argc - 1];
	int cnt = 0;

	if (idx >= 160)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	idx += offs;

	while (--argc) {
		unsigned int val = argv[argc];
		int i;

		if (val >= 256)
			return -EXCEPT_INVALID_GROUP;

		for (i = 0; i < 4; ++i) {
			if (ss_dev_tab[idx].grp[i] == val)
				cnt++;
		}
	}

	retv[0] = cnt;
	return 1; /* return the number of return values */
}


/* --------------------------------------------------------------------------
   Sensors API
   -------------------------------------------------------------------------- */

int32_t __sens_state(struct microjs_env * env, int32_t argv[], int argc)
{
	return __state(env, argv, argc, 0);
}	

int32_t __sens_alarm(struct microjs_env * env, int32_t argv[], int argc)
{
	return __alarm(env, argv, argc, 0);
}

int32_t __sens_trouble(struct microjs_env * env, int32_t argv[], int argc)
{
	return __trouble(env, argv, argc, 0);
}

int32_t __sens_level(struct microjs_env * env, int32_t argv[], int argc)
{
	return __level(env, argv, argc, 0);
}

int32_t __sens_pw1(struct microjs_env * env, int32_t argv[], int argc)
{
	return __pw1(env, argv, argc, 0);
}

int32_t __sens_pw2(struct microjs_env * env, int32_t argv[], int argc)
{
	return __pw2(env, argv, argc, 0);
}

int32_t __sens_pw3(struct microjs_env * env, int32_t argv[], int argc)
{
	return __pw3(env, argv, argc, 0);
}

int32_t __sens_pw4(struct microjs_env * env, int32_t argv[], int argc)
{
	return __pw4(env, argv, argc, 0);
}

int32_t __sens_pw5(struct microjs_env * env, int32_t argv[], int argc)
{
	return __pw5(env, argv, argc, 0);
}

int32_t __sens_en(struct microjs_env * env, int32_t argv[], int argc)
{
	return __enabled(env, argv, argc, 0);
}	

int32_t __sens_tst(struct microjs_env * env, int32_t argv[], int argc)
{
	return __tst(env, argv, argc, 160);
}	

int32_t __sens_tbias(struct microjs_env * env, int32_t argv[], int argc)
{
	return __tbias(env, argv, argc, 0);
}	

int32_t __sens_ilat(struct microjs_env * env, int32_t argv[], int argc)
{
	return __ilat(env, argv, argc, 0);
}	

int32_t __sens_imode(struct microjs_env * env, int32_t argv[], int argc)
{
	return __imode(env, argv, argc, 0);
}	

int32_t __sens_irate(struct microjs_env * env, int32_t argv[], int argc)
{
	return __irate(env, argv, argc, 0);
}

int32_t __sens_ipre(struct microjs_env * env, int32_t argv[], int argc)
{
	return __ipre(env, argv, argc, 0);
}

int32_t __sens_cfg(struct microjs_env * env, int32_t argv[], int argc)
{
	return __cfg(env, argv, argc, 0);
}

int32_t __sens_ap(struct microjs_env * env, int32_t argv[], int argc)
{
	return __ap(env, argv, argc, 0);
}

int32_t __sens_clear(struct microjs_env * env, int32_t argv[], int argc)
{
	return __clear(env, argv, argc, 0);
}

int32_t __sens_insert(struct microjs_env * env, int32_t argv[], int argc)
{
	return __insert(env, argv, argc, 0);
}

int32_t __sens_remove(struct microjs_env * env, int32_t argv[], int argc)
{
	return __remove(env, argv, argc, 0);
}

int32_t __sens_belong(struct microjs_env * env, int32_t argv[], int argc)
{
	return __belong(env, argv, argc, 0);
}


/* --------------------------------------------------------------------------
   Modules API
   -------------------------------------------------------------------------- */

int32_t __mod_state(struct microjs_env * env, int32_t argv[], int argc)
{
	return __state(env, argv, argc, 160);
}	

int32_t __mod_alarm(struct microjs_env * env, int32_t argv[], int argc)
{
	return __alarm(env, argv, argc, 160);
}

int32_t __mod_trouble(struct microjs_env * env, int32_t argv[], int argc)
{
	return __trouble(env, argv, argc, 160);
}

int32_t __mod_level(struct microjs_env * env, int32_t argv[], int argc)
{
	return __level(env, argv, argc, 160);
}

int32_t __mod_pw1(struct microjs_env * env, int32_t argv[], int argc)
{
	return __pw1(env, argv, argc, 160);
}

int32_t __mod_pw2(struct microjs_env * env, int32_t argv[], int argc)
{
	return __pw2(env, argv, argc, 160);
}

int32_t __mod_pw3(struct microjs_env * env, int32_t argv[], int argc)
{
	return __pw3(env, argv, argc, 160);
}

int32_t __mod_pw4(struct microjs_env * env, int32_t argv[], int argc)
{
	return __pw4(env, argv, argc, 160);
}

int32_t __mod_pw5(struct microjs_env * env, int32_t argv[], int argc)
{
	return __pw5(env, argv, argc, 160);
}

int32_t __mod_en(struct microjs_env * env, int32_t argv[], int argc)
{
	return __enabled(env, argv, argc, 160);
}	

int32_t __mod_tst(struct microjs_env * env, int32_t argv[], int argc)
{
	return __tst(env, argv, argc, 160);
}	

int32_t __mod_tbias(struct microjs_env * env, int32_t argv[], int argc)
{
	return __tbias(env, argv, argc, 160);
}	

int32_t __mod_ilat(struct microjs_env * env, int32_t argv[], int argc)
{
	return __ilat(env, argv, argc, 160);
}	

int32_t __mod_imode(struct microjs_env * env, int32_t argv[], int argc)
{
	return __imode(env, argv, argc, 160);
}	

int32_t __mod_irate(struct microjs_env * env, int32_t argv[], int argc)
{
	return __irate(env, argv, argc, 160);
}

int32_t __mod_ipre(struct microjs_env * env, int32_t argv[], int argc)
{
	return __ipre(env, argv, argc, 160);
}

int32_t __mod_cfg(struct microjs_env * env, int32_t argv[], int argc)
{
	return __cfg(env, argv, argc, 160);
}

int32_t __mod_ap(struct microjs_env * env, int32_t argv[], int argc)
{
	return __ap(env, argv, argc, 160);
}

int32_t __mod_clear(struct microjs_env * env, int32_t argv[], int argc)
{
	return __clear(env, argv, argc, 160);
}

int32_t __mod_insert(struct microjs_env * env, int32_t argv[], int argc)
{
	return __insert(env, argv, argc, 160);
}

int32_t __mod_remove(struct microjs_env * env, int32_t argv[], int argc)
{
	return __remove(env, argv, argc, 160);
}

int32_t __mod_belong(struct microjs_env * env, int32_t argv[], int argc)
{
	return __belong(env, argv, argc, 160);
}


/* --------------------------------------------------------------------------
   Native (external) call table
   -------------------------------------------------------------------------- */

int32_t (* const microjs_extern[])(struct microjs_env *, 
								int32_t argv[], int argc) = {
	[EXT_PRINTF] = __printf,
	[EXT_PRINT] = __print,
	[EXT_RAND] = __rand,
	[EXT_SQRT] = __isqrt,
	[EXT_LOG2] = __ilog2,
	[EXT_MEMRD] = __memrd,
	[EXT_SENS_STATE] = __sens_state,
	[EXT_SENS_ALARM] = __sens_alarm,
	[EXT_SENS_TROUBLE] = __sens_trouble,
	[EXT_SENS_LEVEL] = __sens_level,
	[EXT_SENS_PW1] = __sens_pw1,
	[EXT_SENS_PW2] = __sens_pw2,
	[EXT_SENS_PW3] = __sens_pw3,
	[EXT_SENS_PW4] = __sens_pw4,
	[EXT_SENS_PW5] = __sens_pw5,
	[EXT_SENS_EN] = __sens_en,
	[EXT_SENS_TST] = __sens_tst,
	[EXT_SENS_TBIAS] = __sens_tbias,
	[EXT_SENS_ILAT] = __sens_ilat,
	[EXT_SENS_IMODE] = __sens_imode,
	[EXT_SENS_IRATE] = __sens_irate,
	[EXT_SENS_IPRE] = __sens_ipre,
	[EXT_SENS_CFG] = __sens_cfg,
	[EXT_SENS_AP] = __sens_ap,
	[EXT_SENS_CLEAR] = __sens_clear,
	[EXT_SENS_INSERT] = __sens_insert,
	[EXT_SENS_REMOVE] = __sens_remove,
	[EXT_SENS_BELONG] = __sens_belong,

	[EXT_MOD_STATE] = __mod_state,
	[EXT_MOD_ALARM] = __mod_alarm,
	[EXT_MOD_TROUBLE] = __mod_trouble,
	[EXT_MOD_LEVEL] = __mod_level,
	[EXT_MOD_PW1] = __mod_pw1,
	[EXT_MOD_PW2] = __mod_pw2,
	[EXT_MOD_PW3] = __mod_pw3,
	[EXT_MOD_PW4] = __mod_pw4,
	[EXT_MOD_PW5] = __mod_pw5,
	[EXT_MOD_EN] = __mod_en,
	[EXT_MOD_TST] = __mod_tst,
	[EXT_MOD_TBIAS] = __mod_tbias,
	[EXT_MOD_ILAT] = __mod_ilat,
	[EXT_MOD_IMODE] = __mod_imode,
	[EXT_MOD_IRATE] = __mod_irate,
	[EXT_MOD_IPRE] = __mod_ipre,
	[EXT_MOD_CFG] = __mod_cfg,
	[EXT_MOD_AP] = __mod_ap,
	[EXT_MOD_CLEAR] = __mod_clear,
	[EXT_MOD_INSERT] = __mod_insert,
	[EXT_MOD_REMOVE] = __mod_remove,
	[EXT_MOD_BELONG] = __mod_belong,
};

