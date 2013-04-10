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
#include <arch/cortex-m3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef TRACE_RING_SIZE
#define TRACE_RING_SIZE 64
#endif

#ifndef TRACE_MAX_MSG_LEN
#define TRACE_MAX_MSG_LEN 64
#endif

#ifndef TRACE_FMT_BUF_LEN 
#define TRACE_FMT_BUF_LEN (((TRACE_RING_SIZE) * (TRACE_MAX_MSG_LEN)) / 2)
#endif

static struct {
	int tm;
	volatile uint32_t head;
	volatile uint32_t tail;
	struct {
		int32_t dt;
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

static void __timer_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM5;
	unsigned int freq = 1000000;
	uint32_t div;

	/* get the total divisior */
	div = ((2 * stm32f_apb1_hz) + (freq / 2)) / freq;

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM5EN;
	
	/* Timer configuration */
	tim->psc = div - 1;
	tim->arr = 0;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = 0;
	tim->cr2 = 0;
	tim->cr1 = TIM_CEN; /* Enable counter */
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
	int32_t dt;
	va_list ap;
	char * msg;
	int n;

	now = __timer_ts();

	pri = cm3_basepri_get();
	cm3_basepri_set(0x01);

	dt = now - trace_ring.tm;
	trace_ring.tm = now;

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
		msg[n] = '\0';
		trace_fmt.head = pos + n + 1;

		trace_ring.buf[head & (TRACE_RING_SIZE - 1)].msg = msg;
		trace_ring.buf[head & (TRACE_RING_SIZE - 1)].dt = dt;
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
	int32_t dt;

	now = __timer_ts();

	pri = cm3_basepri_get();
	cm3_basepri_set(0x01);

	dt = now - trace_ring.tm;
	trace_ring.tm = now;

	head = trace_ring.head;
	if ((head - trace_ring.tail) != TRACE_RING_SIZE) {
		trace_ring.buf[head & (TRACE_RING_SIZE - 1)].msg = (char *)msg;
		trace_ring.buf[head & (TRACE_RING_SIZE - 1)].dt = dt;
		trace_ring.head = head + 1;
	}

	cm3_basepri_set(pri);

}

void trace_print(FILE * f, int flush)
{
	int dt;
	int ms;
	int us;
	char * s;
	short tail;

	for (tail = trace_ring.tail; tail != trace_ring.head; tail++) {
		s = trace_ring.buf[tail & (TRACE_RING_SIZE - 1)].msg;
		dt = __timer_ts2us(trace_ring.buf[tail & (TRACE_RING_SIZE - 1)].dt);
		ms = dt / 1000;
		us = dt - (ms * 1000);

		fprintf(f, "%7d.%03d: %s\n", ms, us, s);
	}
	
	if (flush) {
		trace_ring.tail = tail;
	}
}

