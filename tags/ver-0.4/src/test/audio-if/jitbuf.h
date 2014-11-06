/* 
 * sndbuf.h
 *
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
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

#ifndef __JITBUF_H__
#define __JITBUF_H__

#include "sndbuf.h"

#ifndef JITBUF_FIFO_LEN
#define JITBUF_FIFO_LEN 64
#endif

struct ratio32 {
	int32_t num;
	uint32_t den;
};

typedef struct ratio32 ratio32_t;

uint32_t tsclk_rate;

struct jitbuf {
	/* Rate of the TSCLK (timestamp clock) [Hz] */
	uint32_t tsclk_rate;
	/* Sample rate [Hz] */
	uint32_t sample_rate;
	/* delay time (TSCLK periods) */
	uint32_t delay;
	/* period of a sound buffer (TSCLK periods) */
	uint32_t tbuf;
	/* time stamp at the head of the queue (TSCLK periods) */
	volatile uint32_t head_ts;
	volatile uint32_t head;
	volatile uint32_t tail;
	sndbuf_t * fifo[JITBUF_FIFO_LEN];
};


typedef struct jitbuf jitbuf_t;

#ifdef __cplusplus
extern "C" {
#endif

int jitbuf_init(jitbuf_t *jb, unsigned int tsclk_rate, 
				 unsigned int sample_rate, unsigned int delay_ms);

int jitbuf_enqueue(jitbuf_t * jb, sndbuf_t * buf, uint32_t ts);

sndbuf_t * jitbuf_dequeue(jitbuf_t * jb);

#ifdef __cplusplus
}
#endif

#endif /* __JITBUF_H__ */

