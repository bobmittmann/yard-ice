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

#ifdef __cplusplus
extern "C" {
#endif

struct jitbuf {
	uint32_t head_ts;
	uint32_t delay;
	volatile uint32_t head;
	volatile uint32_t tail;
	sndbuf_t * fifo[JITBUF_FIFO_LEN];
};


typedef struct jitbuf jitbuf_t;

#ifdef __cplusplus
}
#endif

#endif /* __JITBUF_H__ */

