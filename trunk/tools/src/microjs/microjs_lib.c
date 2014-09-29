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


#include <microjs-stdlib.h>
#include <microjs.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/dcclog.h>

#ifndef MICROJS_STDLIB_ENABLED 
#define MICROJS_STDLIB_ENABLED 1
#endif

#ifndef MICROJS_STRINGS_ENABLED
#define MICROJS_STRINGS_ENABLED 1
#endif

#if MICROJS_STDLIB_ENABLED 

int32_t __rand(void * env, int32_t argv[], int argc) 
{
	int32_t * retv = argv;

	retv[0] = rand();
	DCC_LOG1(LOG_MSG, "%d", retv[0]);

	return 1;
};

int32_t __srand(void * env, int32_t argv[], int argc) 
{
	srand(argv[0]);
	return 0;
};

int32_t __isqrt(void * env, int32_t argv[], int argc)
{
	int32_t * retv = argv;
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

int32_t __memrd(void * env, int32_t argv[], int argc)
{
	int32_t * retv = argv;
	uint32_t addr = argv[0];

	if (addr >= 256)
		return -1;

	retv[0] = 0;

	return 1;
}	

int32_t __ilog2(void * env, int32_t argv[], int argc)
{
	int32_t * retv = argv;
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

int32_t __time(void * env, int32_t argv[], int argc)
{
	int32_t * retv = argv;
	retv[0] = (int32_t)time(NULL);
	return 1;
}	

#if MICROJS_STRINGS_ENABLED 

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

#endif /* MICROJS_STRINGS_ENABLED */

static int32_t __vprintf(void * env, int32_t argv[], int argc, const char * fmt)
{
#if MICROJS_STRINGS_ENABLED 
	char buf[BUF_LEN];
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
	int i = 0;

	#define _va_arg(AP, TYPE) ((i < argc) ? argv[i++] : 0)

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
					fwrite(cp, n, 1, stdout);
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
					fwrite(buf, 1, 1, stdout);
				}
				fwrite(zeros, w - n, 1, stdout);
			} else {
				fwrite(blanks, w - n, 1, stdout);
			}
			cnt += w - n;
		}

		if (flags & SIGN) {
			fwrite(buf, 1, 1, stdout);
			cnt++;
		}

		fwrite(cp, n, 1, stdout);
		cnt += n;

		if ((flags & LEFT) && (w > n)) {
			fwrite(blanks, w - n, 1, stdout);
			cnt += w - n;
		}

		flags = 0;
		w = 0;

		cp = (char *)fmt;
		n = 0;
	}

	if (n) {
		fwrite(cp, n, 1, stdout);
		cnt+= n;;
	}

	return 0;
#else
	return -ERR_STRINGS_UNSUPORTED;
#endif /* MICROJS_STRINGS_ENABLED */
}

int32_t __printf(void * env, int32_t argv[], int argc)
{
#if MICROJS_STRINGS_ENABLED 
	const char * fmt = str(argv[0]);

	return __vprintf(env, &argv[1], argc - 1, fmt);
#else
	return -ERR_STRINGS_UNSUPORTED;
#endif /* MICROJS_STRINGS_ENABLED */
}

int32_t __print(void * env, int32_t argv[], int argc)
{
	int i;

	DCC_LOG1(LOG_MSG, "%d", argv[0]);
	
	for (i = 0; i < argc; ++i) {
		if (i != 0)
			__vprintf(env, argv, 0, ", ");
		__vprintf(env, &argv[i], 1, "%d");
	}

	__vprintf(env, argv, 0, "\n");
	return 0;
}	

struct {
	uint8_t alm;
	uint8_t tbl;
	uint8_t lvl[4];
} device[320];

int32_t __sens_state(void * env, int32_t argv[], int argc)
{
	int32_t * retv = argv;
	unsigned int addr = argv[0];

	if (addr > 159)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */

	retv[0] = (device[addr].alm ? 1 : 0) + (device[addr].tbl ? 2 : 0);

	return 1; /* return the number of return values */
}	

int32_t __sens_alarm(void * env, int32_t argv[], int argc)
{
	unsigned int addr = argv[0];
	unsigned int val = argv[1];

	if (addr > 159)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */

	if (val > 15)
		return -EXCEPT_INVALID_ALARM_CODE;

	device[addr].alm = val;

	return 0; /* return the number of return values */
}

int32_t __sens_trouble(void * env, int32_t argv[], int argc)
{
	unsigned int addr = argv[0];
	unsigned int val = argv[1];

	if (addr > 159)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */

	if (val > 15)
		return -EXCEPT_INVALID_TROUBLE_CODE;

	device[addr].tbl = val;

	return 0; /* return the number of return values */
}


int32_t __mod_state(void * env, int32_t argv[], int argc)
{
	int32_t * retv = argv;
	unsigned int addr = argv[0];

	if (addr > 159)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */

	addr += 160;

	retv[0] = (device[addr].alm ? 1 : 0) + (device[addr].tbl ? 2 : 0);

	return 1; /* return the number of return values */
}	

int32_t __mod_alarm(void * env, int32_t argv[], int argc)
{
	unsigned int addr = argv[0];
	unsigned int val = argv[1];

	if (addr > 159)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */

	addr += 160;

	if (val > 10)
		return -EXCEPT_INVALID_ALARM_CODE;

	device[addr].alm = val;

	return 0; /* return the number of return values */
}

int32_t __mod_trouble(void * env, int32_t argv[], int argc)
{
	unsigned int addr = argv[0];
	unsigned int val = argv[1];

	if (addr > 159)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */

	addr += 160;

	if (val > 10)
		return -EXCEPT_INVALID_TROUBLE_CODE;

	device[addr].tbl = val;

	return 0; /* return the number of return values */
}


/* Array index translator */
int32_t __sensor(void * env, int32_t argv[], int argc)
{
	/* just check for bounds */
	if ((uint32_t)argv[0] > 159)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	return 1; /* return the number of return values */
}	

/* Array index translator */
int32_t __module(void * env, int32_t argv[], int argc)
{
	if ((uint32_t)argv[0] > 159)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	argv[0] = argv[0] + 160; /* add the offset */
	return 1; /* return the number of return values */
}	

/* This is a read only atttribute */
int32_t __dev_state(void * env, int32_t argv[], int argc)
{
	int32_t * retv = argv;
	unsigned int oid = argv[0];

	retv[0] = (device[oid].alm ? 1 : 0) + (device[oid].tbl ? 2 : 0);

	return 1; /* return the number of return values */
}	

/* This is a read/write atttribute */
int32_t __dev_alarm(void * env, int32_t argv[], int argc)
{
	int32_t * retv = argv;
	unsigned int oid = argv[0];

	if (argc > 1) { /* set */
		unsigned int val = argv[1];

		if (val >= 16)
			return -EXCEPT_INVALID_ALARM_CODE;

		device[oid].alm = val;

		return 0; /* number of return values */
	}

	/* get */
	retv[0] = device[oid].alm;

	return 1; /* number of return values */
}

int32_t __dev_trouble(void * env, int32_t argv[], int argc)
{
	int32_t * retv = argv;
	unsigned int oid = argv[0];

	if (argc > 1) { /* set */
		unsigned int val = argv[1];

		if (val >= 16)
			return -EXCEPT_INVALID_ALARM_CODE;

		device[oid].tbl = val;

		return 0; /* number of return values */
	}

	/* get */
	retv[0] = device[oid].tbl;

	return 1; /* number of return values */
}

int32_t __dev_level(void * env, int32_t argv[], int argc)
{
	int32_t * retv = argv;
	unsigned int oid = argv[0];
	unsigned int idx = argv[1];

	if (argc > 2) { /* set */
		unsigned int val = argv[2];

		if (val >= 256)
			return -EXCEPT_INVALID_LEVEL;

		device[oid].lvl[idx] = val;

		return 0; /* number of return values */
	}

	/* get */
	retv[0] = device[oid].lvl[idx];

	return 1; /* number of return values */
}


/* --------------------------------------------------------------------------
   LED
   -------------------------------------------------------------------------- */

int led[6];

/* Array index translator */
int32_t __led(void * env, int32_t argv[], int argc)
{
	/* just check for bounds */
	if ((uint32_t)argv[0] >= 6)
		return -EXCEPT_BAD_ADDR; /* Throw an exception */
	return 1; /* return the number of return values */
}	


int32_t js_led_on(void * env, int32_t argv[], int argc)
{
	unsigned int id = argv[0];

	if (id >= 6)
		return -EXCEPT_INVALID_LED; /* Throw an exception */

	if (argc > 1) {
		unsigned int val = argv[1];

		if (val > 1)
			return -EXCEPT_INVALID_VALUE;

		led[id] = val;

		return 0;
	}

	DCC_LOG2(LOG_TRACE, "id=%d val=%d", id, led[id]);

	argv[0] = led[id];

	return 1; /* return the number of return values */
}

int32_t js_led_flash(void * env, int32_t argv[], int argc)
{
	unsigned int id = argv[0];
	unsigned int ms = argv[1];


	if (id >= 6)
		return -EXCEPT_INVALID_LED; /* Throw an exception */

	if (ms >= (256 * 16))
		return -EXCEPT_INVALID_VALUE;

	DCC_LOG2(LOG_TRACE, "id=%d ms=%d", id, ms);

	led[id] = ms;

	return 0; /* return no values */
}

/* --------------------------------------------------------------------------
   Native (external) call table
   -------------------------------------------------------------------------- */

int32_t (* const microjs_extern[])(void *, int32_t [], int) = {
	[EXT_RAND] = __rand,
	[EXT_SQRT] = __isqrt,
	[EXT_LOG2] = __ilog2,
	[EXT_WRITE] = __print,
	[EXT_TIME] = __time,
	[EXT_SRAND] = __srand,
	[EXT_PRINT] = __print,
	[EXT_PRINTF] = __printf,
	[EXT_MEMRD] = __memrd,
	[EXT_SENS_STATE] = __sens_state,
	[EXT_SENS_ALARM] = __sens_alarm,
	[EXT_SENS_TROUBLE] = __sens_trouble,
	[EXT_MOD_STATE] = __mod_state,
	[EXT_MOD_ALARM] = __mod_alarm,
	[EXT_MOD_TROUBLE] = __mod_trouble,

	[EXT_SENSOR] = __sensor,
	[EXT_MODULE] = __module,
	[EXT_LED] = __led,

	[EXT_DEV_STATE] = __dev_state,
	[EXT_DEV_ALARM] = __dev_alarm,
	[EXT_DEV_TROUBLE] = __dev_trouble,
	[EXT_DEV_LEVEL] = __dev_level,

	[EXT_LED_ON] = js_led_on,
	[EXT_LED_FLASH] = js_led_flash,
};

#endif /* MICROJS_STDLIB_ENABLED  */


