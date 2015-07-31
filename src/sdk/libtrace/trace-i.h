/*
 * Copyright(c) 2004-2014 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libhttpd.
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
 * @file trace-i
 * @brief trace library private header
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TRACE_I_H__
#define __TRACE_I_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <trace.h>
#include <thinkos.h>

#ifndef TRACE_RING_SIZE
#define TRACE_RING_SIZE 1024
#endif

#ifndef TRACE_STRING_MAX
#define TRACE_STRING_MAX 64
#endif

#ifndef TRACE_TIMER
#if defined(STM32F_TIM5)
#define TRACE_TIMER STM32F_TIM5
#elif defined(STM32F_TIM2)
#define TRACE_TIMER STM32F_TIM2
#endif
#endif

struct trace_ring {
	int mutex;
	uint64_t tm;
	volatile uint32_t head;
	volatile uint32_t tail;
	volatile uint32_t print_pos;
	volatile uint32_t print_tm;
	struct {
		union {
			const struct trace_ref * ref;
			uint32_t ts;
			uint32_t val;
		};
	} buf[TRACE_RING_SIZE];
};

extern struct trace_ring trace_ring;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t trace_ts2us(uint32_t ts);

uint64_t trace_tm2us(uint64_t tm);

int trace_fmt(struct trace_entry * entry, char * s, int max);


#ifdef __cplusplus
}
#endif

#endif /* __TRACE_I_H__ */

