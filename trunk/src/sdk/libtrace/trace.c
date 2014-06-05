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

#include <sys/os.h>

#include <sys/dcclog.h>

#ifndef TRACE_RING_SIZE
#define TRACE_RING_SIZE 128
#endif

#ifndef TRACE_MAX_MSG_LEN
#define TRACE_MAX_MSG_LEN 64
#endif

#ifndef TRACE_FMT_BUF_LEN 
#define TRACE_FMT_BUF_LEN ((TRACE_RING_SIZE) * (TRACE_MAX_MSG_LEN) / 2)
#endif

static struct {
	uint32_t tm;
	int mutex;
	volatile uint32_t head;
	volatile uint32_t tail;
	volatile uint32_t print_pos;
	volatile uint32_t print_tm;
	struct {
		int32_t ts;
		char * msg;
	} buf[TRACE_RING_SIZE];
} trace_ring;

static struct {
	uint32_t head;
	char buf[TRACE_FMT_BUF_LEN];
} trace_fmt;

/*---------------------------------------------------------------------------
 * Profiling timer
 *---------------------------------------------------------------------------
 */

static inline uint32_t __timer_ts2us(uint32_t tm)
{
	return tm;
}

static inline uint32_t __timer_ts(void)
{
	struct stm32f_tim * tim = STM32F_TIM5;

	return tim->cnt;
}

#define TIMESTAMP_FREQ 1000000

static void __timer_init(void)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32f_tim * tim = STM32F_TIM5;
	uint32_t div;

	/* get the total divisior */
	div = (stm32f_tim1_hz + (TIMESTAMP_FREQ / 2)) / TIMESTAMP_FREQ;

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM5EN;
	
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
	trace_ring.mutex = __os_mutex_alloc();
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

int tracef(const char *fmt, ... )
{
	unsigned int head;
	unsigned int pri;
	unsigned int pos;
	uint32_t now;
	va_list ap;
	char * msg;
	int n;

	now = __timer_ts();

	pri = cm3_basepri_get();
	cm3_basepri_set(0x01);

	head = trace_ring.head;
	if ((head - trace_ring.tail) != TRACE_RING_SIZE) {
		/* Alloc space for a formatted string */
		pos = trace_fmt.head;
		if ((pos + TRACE_MAX_MSG_LEN) >= TRACE_FMT_BUF_LEN)
			pos = 0;
		msg = &trace_fmt.buf[pos];

		va_start(ap, fmt);
		n = vsnprintf(msg, TRACE_MAX_MSG_LEN - 1, fmt, ap);
		va_end(ap);
		msg[TRACE_MAX_MSG_LEN - 1] = '\0';
		trace_fmt.head = pos + n + 1;

		trace_ring.buf[head & (TRACE_RING_SIZE - 1)].msg = msg;
		trace_ring.buf[head & (TRACE_RING_SIZE - 1)].ts = now;
		trace_ring.head = head + 1;
	} else
		n = -1;

	cm3_basepri_set(pri);

	return n;
}

void trace(const char * msg)
{
	unsigned int head;
	unsigned int pri;
	uint32_t now;

	now = __timer_ts();

	pri = cm3_basepri_get();
	cm3_basepri_set(0x01);

	head = trace_ring.head;
	if ((head - trace_ring.tail) != TRACE_RING_SIZE) {
		trace_ring.buf[head & (TRACE_RING_SIZE - 1)].msg = (char *)msg;
		trace_ring.buf[head & (TRACE_RING_SIZE - 1)].ts = now;
		trace_ring.head = head + 1;
	}

	cm3_basepri_set(pri);
}

int trace_fprint(FILE * f, unsigned int opt)
{
	int32_t dt;
	uint32_t tm;
	uint32_t ts;
	int32_t sec;
	uint32_t ms;
	uint32_t us;
	uint32_t pos;
	char * s;
	uint32_t tail;
	int ret = 0;

	if ((opt & TRACE_UNSAFE) == 0)
		__os_mutex_lock(trace_ring.mutex);

	if (opt & TRACE_ALL) {
		tail = trace_ring.tail;
		tm = trace_ring.tm;
	} else {
		tail = trace_ring.print_pos;
		tm = trace_ring.print_tm;
	}

	for (; tail != trace_ring.head; tail++) {
		pos = tail & (TRACE_RING_SIZE - 1);
		s = trace_ring.buf[pos].msg;
	
		if (opt & TRACE_ABSTIME) {
			ts = __timer_ts2us(trace_ring.buf[pos].ts);
			sec = ts / 1000000;
			ts -= (sec * 1000000);
			ms = ts / 1000;
			us = ts - (ms * 1000);
		} else {
			dt = __timer_ts2us((int32_t)(trace_ring.buf[pos].ts - tm));
			sec = dt / 1000000;
			dt -= (sec * 1000000);
			ms = dt / 1000;
			us = dt - (ms * 1000);
		}

		if (opt & TRACE_COUNT)
			ret = fprintf(f, "%5d %4d,%03d.%03d: %s\n", tail, sec, ms, us, s);
		else
			ret = fprintf(f, "%4d,%03d.%03d: %s\n", sec, ms, us, s);

		if (ret < 0) {
			break;
		}

		tm = trace_ring.buf[pos].ts;
	}

	if ((opt & TRACE_ALL) == 0)
		trace_ring.print_pos = tail;

	trace_ring.print_tm = tm;

	if (opt & TRACE_FLUSH) {
		trace_ring.tail = tail;
		trace_ring.print_pos = tail;
		trace_ring.tm = tm;
	}

	if ((opt & TRACE_UNSAFE) == 0)
		__os_mutex_unlock(trace_ring.mutex);

	if (ret > 0)
		ret = 0;

	return ret;
}

void trace_flush(void)
{
	uint32_t head;
	uint32_t tm;

	__os_mutex_lock(trace_ring.mutex);

	head = trace_ring.head;
	tm = __timer_ts();

	trace_ring.print_pos = head;
	trace_ring.tail = head;
	trace_ring.tm = tm ;
	trace_ring.print_tm  = tm;

	__os_mutex_unlock(trace_ring.mutex);
}

