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

#ifndef __SNDBUF_H__
#define __SNDBUF_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#ifndef SNDBUF_LEN 
#define SNDBUF_LEN 64
#endif

#ifndef SNDBUF_POOL_SIZE
#define SNDBUF_POOL_SIZE 128
#endif

struct sndbuf {
	uint32_t ref;
	int16_t data[SNDBUF_LEN];
};

extern const unsigned int sndbuf_len;

typedef struct sndbuf sndbuf_t;

extern const sndbuf_t sndbuf_zero;

extern sndbuf_t sndbuf_null;

#ifdef __cplusplus
extern "C" {
#endif

sndbuf_t * sndbuf_alloc(void);

sndbuf_t * sndbuf_use(sndbuf_t * buf);

void sndbuf_free(sndbuf_t * buf);

void sndbuf_pool_init(void);

int sndbuf_pool_test(void);

int sndbuf_id(sndbuf_t * buf);

#ifdef __cplusplus
}
#endif

#endif /* __SNDBUF_H__ */

