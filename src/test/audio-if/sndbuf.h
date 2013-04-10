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

extern const unsigned int sndbuf_len;

struct sndbuf;

typedef struct sndbuf sndbuf_t;

#ifdef __cplusplus
extern "C" {
#endif

sndbuf_t * sndbuf_alloc(void);

void sndbuf_free(sndbuf_t * buf);

void sndbuf_pool_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __SNDBUF_H__ */

