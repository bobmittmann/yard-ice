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

#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <trace.h>
#include <thinkos.h>

#ifndef TRACE_RING_SIZE
#define TRACE_RING_SIZE 1024
#endif

#ifndef TRACE_TIMER
#define TRACE_TIMER STM32F_TIM5
#endif

static struct {
	uint32_t tm;
	int mutex;
	volatile uint32_t head;
	volatile uint32_t tail;
	volatile uint32_t print_pos;
	volatile uint32_t print_tm;
	struct {
		union {
			uint32_t ts;
			const char * fmt;
			uint32_t val;
		};
	} buf[TRACE_RING_SIZE];
} trace_ring;

/*---------------------------------------------------------------------------
 * Profiling timer
 *---------------------------------------------------------------------------
 */

/* This implementation requires a 32bits free running timer.
 * The STM32 family provide the following 32bits timers:
 *  - STM32F_TIM2
 *  - STM32F_TIM5
 */


#define TIMESTAMP_FREQ 1000000

static inline uint32_t __timer_ts2us(uint32_t tm)
{
	return tm;
}

static inline uint32_t __timer_ts(void)
{
	struct stm32f_tim * tim = TRACE_TIMER;

	return tim->cnt;
}

static void __timer_init(void)
{
	struct stm32f_tim * tim = TRACE_TIMER;
	uint32_t div;

	/* Timer clock enable */
	if (TRACE_TIMER == STM32F_TIM5)
			stm32_clk_enable(STM32_RCC, STM32_CLK_TIM5);
	else if  (TRACE_TIMER == STM32F_TIM2)
		stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);

	/* get the total divisior */
	div = (stm32f_tim1_hz + (TIMESTAMP_FREQ / 2)) / TIMESTAMP_FREQ;
	/* Timer configuration */
	tim->psc = div - 1;
	tim->arr = 0xffffffff;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = 0;
	tim->cr2 = 0;
	tim->cr1 = TIM_CEN; /* Enable counter */

	tim->egr = TIM_UG; /* Update registers */
}

/*---------------------------------------------------------------------------
 * Trace API
 *---------------------------------------------------------------------------
 */

uint32_t trace_timestamp(void)
{
	return __timer_ts();
}


uint32_t trace_ts2us(uint32_t tm)
{
	return __timer_ts2us(tm);
}


void trace_init(void)
{
	__timer_init();
	trace_ring.mutex = thinkos_mutex_alloc();
	trace_ring.print_pos = 0;
	trace_ring.print_tm = 0;
	trace_ring.head = 0;
	trace_ring.tail = 0;
	trace_ring.tm = __timer_ts();
}

int32_t trace_dt(uint32_t * prev)
{
	uint32_t now;
	int32_t dt;

	now = __timer_ts();

	dt = now - *prev;
	*prev = now;

	return dt;
}

/*---------------------------------------------------------------------------
 * Trace format
 *---------------------------------------------------------------------------
 */

void vtracef(const char *fmt, uint32_t ts, va_list ap)
{
	unsigned int head;
	unsigned int tail;
	uint32_t val;
	char * cp;
	int flags;
	int c;
	int i;

	head = trace_ring.head;
	tail = trace_ring.tail;

	if ((TRACE_RING_SIZE + tail - head) < 2)
		return;

	trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].ts = ts;
	trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].fmt = fmt;

	for (flags = 0; (c = *fmt) != '\0'; ++fmt) {
		if (flags == 0) {
			if (c == '%')
				flags = c;
		} else {
			switch (c) {
			case '%':
				flags = 0;
				break;

			case 'c':
			case 'd':
			case 'u':
			case 'x':
			case 'X':
			case 'p':
				val = va_arg(ap, uint32_t);
				if ((TRACE_RING_SIZE + tail - head) == 0)
					return;
				trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].val = val;
				flags = 0;
				break;

			case 's':
				cp = va_arg(ap, char *);
				i = 0;
				do {
					val = 0;
					if ((c = cp[i]) != '\0') {
						val = c;
						if ((c = cp[i + 1]) != '\0') {
							val |= c << 8;
							if ((c = cp[i + 2]) != '\0') {
								val |= c << 16;
								if ((c = cp[i + 3]) != '\0') {
									val |= c << 24;
								}
							}
						}
					}
					if ((TRACE_RING_SIZE + tail - head) == 0)
						return;
					trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].val = val;
					i += 4;
				} while (c != '\0');
				flags = 0;
				break;

			default:
				break;
			}
		}
	}

	trace_ring.head = head;
}

void tracef(const char *fmt, ... )
{
	unsigned int pri;
	uint32_t now;
	va_list ap;

	now = __timer_ts();
	va_start(ap, fmt);

	pri = cm3_primask_get();
	cm3_primask_set(1);

	vtracef(fmt, now, ap);

	cm3_primask_set(pri);

	va_end(ap);
}

void trace(const char * msg)
{
	unsigned int head;
	unsigned int pri;
	uint32_t now;

	now = __timer_ts();

	pri = cm3_primask_get();
	cm3_primask_set(1);

	head = trace_ring.head;
	if ((TRACE_RING_SIZE + trace_ring.tail - head) >= 2) {
		trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].ts = now;
		trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].fmt = msg;
		trace_ring.head = head;
	}

	cm3_primask_set(pri);
}

int trace_decode(struct file * f, const char * fmt, uint32_t * tail_ptr);

int trace_fprint(FILE * f, unsigned int opt)
{
	int32_t dt;
	uint32_t ts_prev;
	uint32_t ts;
	int32_t sec;
	uint32_t ms;
	uint32_t us;
	const char * fmt;
	uint32_t tail;
	int ret = 0;

	if ((opt & TRACE_UNSAFE) == 0)
		thinkos_mutex_lock(trace_ring.mutex);

	if (opt & TRACE_ALL) {
		tail = trace_ring.tail;
		ts_prev = trace_ring.tm;
	} else {
		tail = trace_ring.print_pos;
		ts_prev = trace_ring.print_tm;
	}

	for (; tail != trace_ring.head; tail++) {
		ts = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].ts;
		if (opt & TRACE_ABSTIME) {
			us = __timer_ts2us(ts);
			sec = us / 1000000;
			us -= (sec * 1000000);
			ms = us / 1000;
			us = us - (ms * 1000);
		} else {
			dt = __timer_ts2us((int32_t)(ts - ts_prev));
			sec = dt / 1000000;
			dt -= (sec * 1000000);
			ms = dt / 1000;
			us = dt - (ms * 1000);
		}
		ts_prev = ts;

		fmt = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].fmt;

		if (opt & TRACE_COUNT)
			ret = fprintf(f, "%5d %4d,%03d.%03d: n", tail, sec, ms, us);
		else
			ret = fprintf(f, "%4d,%03d.%03d: ", sec, ms, us);

		if (ret < 0)
			goto done;

		ret = trace_decode(f, fmt, &tail);

		if (ret < 0)
			goto done;
	}

	if ((opt & TRACE_ALL) == 0)
		trace_ring.print_pos = tail;

	trace_ring.print_tm = ts_prev;

	if (opt & TRACE_FLUSH) {
		trace_ring.tail = tail;
		trace_ring.print_pos = tail;
		trace_ring.tm = ts_prev;
	}

done:
	if ((opt & TRACE_UNSAFE) == 0)
		thinkos_mutex_unlock(trace_ring.mutex);

	if (ret > 0)
		ret = 0;

	return ret;
}

void trace_flush(void)
{
	uint32_t head;
	uint32_t tm;

	thinkos_mutex_lock(trace_ring.mutex);

	head = trace_ring.head;
	tm = __timer_ts();

	trace_ring.print_pos = head;
	trace_ring.tail = head;
	trace_ring.tm = tm ;
	trace_ring.print_tm  = tm;

	thinkos_mutex_unlock(trace_ring.mutex);
}

#ifndef PRINTF_ENABLE_LEFT_ALIGN
#define PRINTF_ENABLE_LEFT_ALIGN 1
#endif

#ifndef PRINTF_ENABLE_LARGE_PADDING
#define PRINTF_ENABLE_LARGE_PADDING 1
#endif

#ifndef PRINTF_ENABLE_UNSIGNED
#define PRINTF_ENABLE_UNSIGNED 1
#endif

#ifndef PRINTF_ENABLE_POINTER
#define PRINTF_ENABLE_POINTER 1
#endif


int uint2dec(char * s, unsigned int val);
int uint2hex(char * s, unsigned int val);

#define BUF_LEN 64

#define PERCENT 0x01
#define WIDTH 0x02
#define ZERO 0x04
#define LEFT 0x08
#define SIGN 0x10

extern const char __zeros[];
extern const char __blanks[];

int trace_decode(struct file * f, const char * fmt, uint32_t * tail_ptr)
{
	uint32_t tail = *tail_ptr;
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
		uint32_t u32;
	} val;

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
					n = f->op->write(f->data, cp, n);
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
			{
				val.i = (int)trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].val;
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
			{
				val.n = (unsigned int)trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].val;
#if (PRINTF_ENABLE_POINTER)
hexadecimal:
#endif
				n = uint2hex(cp, val.n);
			}
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
			} while ((v & 0xff000000) && (n < BUF_LEN));

			goto print_buf;
		}

#if (PRINTF_ENABLE_UNSIGNED)
		if (c == 'u') {
			cp = buf;
			{
				val.n = (unsigned int)trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].val;
				n = uint2dec(cp, val.n);
			}
			goto print_buf;
		}
#endif

#if (PRINTF_ENABLE_POINTER)
		if (c == 'p') {
			val.ptr = (void *)trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].val;
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
		cp = (char *)fmt;
		n = 0;
	}

	if (n) {
		r = f->op->write(f->data, cp, n);
		cnt+= r;;
	}

	*tail_ptr = tail;

	return cnt;
}

