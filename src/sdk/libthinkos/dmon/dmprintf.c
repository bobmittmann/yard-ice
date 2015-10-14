/* 
 * File:	 usb-cdc.c
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

#include <stdarg.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>
#include <sys/param.h>

#ifndef DMPRINTF_ENABLE_LEFT_ALIGN
#define DMPRINTF_ENABLE_LEFT_ALIGN    0
#endif

#ifndef DMPRINTF_ENABLE_LARGE_PADDING
#define DMPRINTF_ENABLE_LARGE_PADDING 0
#endif

#ifndef DMPRINTF_ENABLE_FAST
#define DMPRINTF_ENABLE_FAST          1
#endif

#ifndef DMPRINTF_ENABLE_UNSIGNED
#define DMPRINTF_ENABLE_UNSIGNED      1
#endif

#ifndef DMPRINTF_ENABLE_POINTER
#define DMPRINTF_ENABLE_POINTER       0
#endif

#ifndef DMPRINTF_ENABLE_LONG
#define DMPRINTF_ENABLE_LONG          0
#endif

#ifndef DMPRINTF_ENABLE_ARG_WIDTH
#define DMPRINTF_ENABLE_ARG_WIDTH    0
#endif


int uint2dec(char * s, unsigned int val);
int uint2hex(char * s, unsigned int val);

int ull2dec(char * s, unsigned long long val);
int ull2hex(char * s, unsigned long long val);

#if DMPRINTF_ENABLE_LONG
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


#if (DMPRINTF_ENABLE_LONG)
#undef DMPRINTF_ENABLE_LARGE_PADDING
#define DMPRINTF_ENABLE_LARGE_PADDING 1
#endif

static const char __zeros[]  = {  
#if (DMPRINTF_ENABLE_LARGE_PADDING)
	'0', '0', '0', '0', '0', '0', '0', '0', 
	'0', '0', '0', '0', '0', '0', '0', '0', 
	'0', '0', '0', '0', 
#endif
	'0', '0', '0', '0', '0', '0', '0', '0', 
	'0', '0', '0', '0', };
	
static const char __blanks[] = {
#if (DMPRINTF_ENABLE_LARGE_PADDING)
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
#endif
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };


int dmprintf(struct dmon_comm * comm, const char *fmt, ... )
{
	char buf[BUF_LEN];
	va_list ap;
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
#if (DMPRINTF_ENABLE_LONG)
		unsigned long long ull;
		long long ll;
#endif
	} val;

	va_start(ap, fmt);

	n = 0;
	w = 0;
	cnt = 0;
#if (DMPRINTF_ENABLE_FAST)
	cp = (char *)fmt;
#endif
	for (flags = 0; (c = *fmt++); ) {
		if (flags == 0) {
			if (c == '%') {
				w = 0;
				flags = PERCENT;
#if (DMPRINTF_ENABLE_FAST)
				if (n) {
					n = dmon_comm_send(comm, cp, n);
					cp = (char *)fmt;
					cnt += n;;
					n = 0;
				}
#endif
				continue;
			}

#if (DMPRINTF_ENABLE_FAST)
			n++;
#else
			buf[0] = c;
			cnt += dmon_comm_send(comm, buf, 1);
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

#if (DMPRINTF_ENABLE_LEFT_ALIGN)
		if (c == '-') {
			flags |= LEFT;
			continue;
		}
#else
		if (c == '-')
			continue;
#endif

#if (DMPRINTF_ENABLE_ARG_WIDTH)
		if (c == '*') {
			w = va_arg(ap, int);
			continue;
		}
#endif

#if (DMPRINTF_ENABLE_LONG)
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
#if DMPRINTF_ENABLE_LONG
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
#if DMPRINTF_ENABLE_LONG
			if (flags & LONG2) {
				val.ull = va_arg(ap, unsigned long long);
				n = ull2hex(cp, val.ull);
			} else
#endif
			{
				val.n = va_arg(ap, unsigned int);
#if (DMPRINTF_ENABLE_POINTER)
hexadecimal:
#endif
				n = uint2hex(cp, val.n);
			}
			goto print_buf;
		}

		if (c == 's') {
			cp = va_arg(ap, char *);
			n = 0;
			while (cp[n] != '\0')
				n++;
			goto print_buf;
		}

#if (DMPRINTF_ENABLE_UNSIGNED)
		if (c == 'u') {
			cp = buf;
#if DMPRINTF_ENABLE_LONG
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

#if (DMPRINTF_ENABLE_POINTER)
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
#if (DMPRINTF_ENABLE_LEFT_ALIGN)
		if (!(flags & LEFT) && (w > n)) {
#else
		if (w > n) {
#endif
			if (flags & ZERO) {
				if (flags & SIGN) {
					flags &= ~SIGN;
					cnt += dmon_comm_send(comm, buf, 1);
				}
				r = dmon_comm_send(comm, __zeros, w - n);
			} else {
				r = dmon_comm_send(comm, __blanks, w - n);
			}
			cnt += r;
		}

		if (flags & SIGN) {
			cnt += dmon_comm_send(comm, buf, 1);
		}

		cnt += dmon_comm_send(comm, cp, n);

#if (DMPRINTF_ENABLE_LEFT_ALIGN)
		if ((flags & LEFT) && (w > n)) {
			r = dmon_comm_send(comm, __blanks, w - n);
			cnt += r;
		}
#endif

		flags = 0;
		w = 0;

#if (DMPRINTF_ENABLE_FAST)
		cp = (char *)fmt;
		n = 0;
#endif
	}

#if (DMPRINTF_ENABLE_FAST)
	if (n) {
		r = dmon_comm_send(comm, cp, n);
		cnt+= r;;
	}
#endif

	va_end(ap);

	return cnt;
}
