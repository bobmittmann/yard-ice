/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file trace.c
 * @brief Real-time trace
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "trace-i.h"

#ifndef PRINTF_ENABLE_LEFT_ALIGN
#define PRINTF_ENABLE_LEFT_ALIGN 1
#endif

#ifndef PRINTF_ENABLE_LARGE_PADDING
#define PRINTF_ENABLE_LARGE_PADDING 1
#endif

#ifndef PRINTF_ENABLE_UNSIGNED
#define PRINTF_ENABLE_UNSIGNED 1
#endif

#ifndef PRINTF_ENABLE_IPADDR
#define PRINTF_ENABLE_IPADDR 1
#endif

#ifndef PRINTF_ENABLE_POINTER
#define PRINTF_ENABLE_POINTER 1
#endif

#if (PRINTF_ENABLE_IPADDR)
#include <tcpip/in.h>
#endif

int uint2dec(char * s, unsigned int val);
int uint2hex(char * s, unsigned int val);

#define PERCENT 0x01
#define WIDTH 0x02
#define ZERO 0x04
#define LEFT 0x08
#define SIGN 0x10

int trace_fmt(struct trace_entry * entry, char * s, int max)
{
	uint32_t tail = entry->idx + 2;
	const char * fmt = entry->ref->fmt;
	char buf[TRACE_STRING_MAX];
	char * cp;
	int flags;
	int cnt;
	int c;
	int w;
	int n;
	int r;
	int i;
	union {
		void * ptr;
		unsigned int n;
		int i;
		uint32_t u32;
	} val;

	/* leave room for null at the end */
	max--;

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
					if ((cnt + n) > max)
						n = max - cnt;
					for (i = 0; i < n; ++i, ++cnt)
						s[cnt] = cp[i];
					cp = (char *)fmt;
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

#if (PRINTF_ENABLE_LEFT_ALIGN)
		if (c == '-') {
			flags |= LEFT;
			continue;
		}
#else
		if (c == '-')
			continue;
#endif

		if (c == 'c') {
			c = (int)trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].val;
			goto print_char;
		}

		if (c == 'd') {
			cp = buf;
			val.i = (int)trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].val;
			if (val.i < 0) {
				buf[0] = '-';
				val.i = -val.i;
				flags |= SIGN;
				cp++;
				w--;
			}
			n = uint2dec(cp, val.n);
			goto print_buf;
		}


		if (c == 'x') {
			cp = buf;
			val.n = (unsigned int)trace_ring.buf[tail++ & 
				(TRACE_RING_SIZE - 1)].val;
#if (PRINTF_ENABLE_POINTER)
hexadecimal:
#endif
			n = uint2hex(cp, val.n);
			goto print_buf;
		}

		if (c == 's') {
			uint32_t v;
			cp = buf;
			n = 0;

			do {
				v = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].val;
				if ((c = (v & 0xff)) != 0) {
					cp[n++] = c;
					if ((c = ((v >> 8) & 0xff)) != 0) {
						cp[n++] = c;
						if ((c = ((v >> 16) & 0xff)) != 0) {
							cp[n++] = c;
							if ((c = ((v >> 24) & 0xff)) != 0) {
								cp[n++] = c;
							}
						}
					}
				}
			} while ((v & 0xff000000) && (n < TRACE_STRING_MAX));

			goto print_buf;
		}

#if (PRINTF_ENABLE_UNSIGNED)
		if (c == 'u') {
			cp = buf;
			val.n = (unsigned int)trace_ring.buf[tail++ & 
				(TRACE_RING_SIZE - 1)].val;
			n = uint2dec(cp, val.n);
			goto print_buf;
		}
#endif

#if	(PRINTF_ENABLE_IPADDR)
		if (c == 'I') {
			cp = buf;
			val.u32 = (uint32_t)trace_ring.buf[tail++ & 
				(TRACE_RING_SIZE - 1)].val;
			cp += uint2dec(cp, IP4_ADDR1(val.u32));
			*cp++ = '.';
			cp += uint2dec(cp, IP4_ADDR2(val.u32));
			*cp++ = '.';
			cp += uint2dec(cp, IP4_ADDR3(val.u32));
			*cp++ = '.';
			cp += uint2dec(cp, IP4_ADDR4(val.u32));
			n = cp - buf;
			cp = buf;
			goto print_buf;
		}
#endif

#if (PRINTF_ENABLE_POINTER)
		if (c == 'p') {
			val.ptr = (void *)trace_ring.buf[tail++ & 
				(TRACE_RING_SIZE - 1)].val;
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
					if (cnt < max)
						s[cnt++] = buf[0];
				}
				r = w - n;
				for (i = 0; (i < r) && (cnt < max); ++i, ++cnt)
					s[cnt] = '0';
			} else {
				r = w - n;
				for (i = 0; (i < r) && (cnt < max); ++i, ++cnt)
					s[cnt] = ' ';
			}
//			cnt += r;
		}

		if (flags & SIGN) {
			if (cnt < max)
				s[cnt++] = buf[0];
		}

		for (i = 0; (i < n) && (cnt < max); ++i, ++cnt)
			s[cnt] = cp[i];

#if (PRINTF_ENABLE_LEFT_ALIGN)
		if ((flags & LEFT) && (w > n)) {
			r = w - n;
			for (i = 0; (i < r) && (cnt < max); ++i, ++cnt)
				s[cnt] = ' ';

		}
#endif

		cp = (char *)fmt;
		flags = 0;
		w = 0;
		n = 0;
	}

	for (i = 0; (i < n) && (cnt < max); ++i, ++cnt)
		s[cnt] = cp[i];

	s[cnt] = '\0';

	entry->idx = tail;

	return cnt;
}

const char * const trace_lvl_nm[] = {
		" NONE",
		"ERROR",
		" WARN",
		" INFO",
		"DEBUG",
		"  XTR"
};

