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
#include <stdarg.h>

struct trace_ring trace_ring;

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

static inline uint32_t __timer_ts2us(uint32_t ts)
{
	return ts;
}

static inline uint64_t __timer_tm2us(uint32_t tm)
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

#ifdef STM32F_TIM5
	/* Timer clock enable */
	if (TRACE_TIMER == STM32F_TIM5)
			stm32_clk_enable(STM32_RCC, STM32_CLK_TIM5);
#endif

#ifdef STM32F_TIM2
	if  (TRACE_TIMER == STM32F_TIM2)
		stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);
#endif

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


uint32_t trace_ts2us(uint32_t ts)
{
	return __timer_ts2us(ts);
}

uint64_t trace_tm2us(uint64_t tm)
{
	return __timer_tm2us(tm);
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
 * Trace initialization
 *---------------------------------------------------------------------------
 */

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

/*---------------------------------------------------------------------------
 * Trace format
 *---------------------------------------------------------------------------
 */

void vtracef(const struct trace_ref * ref, uint32_t ts, va_list ap)
{
	const char * fmt = ref->fmt;
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

	trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].ref = ref;
	trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].ts = ts;

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
			case 'I':
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
					i += 4;
					/* Limit the length of the string */
					if (i >= TRACE_STRING_MAX) {
						val &= 0x00ffffff;
						c = '\0';
					}
					trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].val = val;
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

void tracef(const struct trace_ref * ref, ... )
{
	unsigned int pri;
	uint32_t now;
	va_list ap;

	now = __timer_ts();
	va_start(ap, ref);

	pri = cm3_primask_get();
	cm3_primask_set(1);

	vtracef(ref, now, ap);

	cm3_primask_set(pri);

	va_end(ap);
}

void trace(const struct trace_ref * ref)
{
	unsigned int head;
	unsigned int pri;
	uint32_t now;

	now = __timer_ts();

	pri = cm3_primask_get();
	cm3_primask_set(1);

	head = trace_ring.head;
	if ((TRACE_RING_SIZE + trace_ring.tail - head) >= 2) {
		trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].ref = ref;
		trace_ring.buf[head++ & (TRACE_RING_SIZE - 1)].ts = now;
		trace_ring.head = head;
	}

	cm3_primask_set(pri);
}

