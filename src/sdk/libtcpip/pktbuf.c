/* 
 * Copyright(C) 2015 Bob Mittmann. All Rights Reserved.
 *
 * This file is part of the libtcpip.
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
 * @file pktbuf.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <arch/cortex-m3.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/pktbuf.h>
#include <sys/dcclog.h>

/* ----------------------------------------------------------------------
 * packet buffer pool
 * ----------------------------------------------------------------------
 */

#ifndef PKTBUF_LEN 
#define PKTBUF_LEN         (1008 + 16)
#endif

#ifndef PKTBUF_POOL_SIZE
#define PKTBUF_POOL_SIZE   6
#endif

#ifndef PKTBUF_ENABLE_STAT
#define PKTBUF_ENABLE_STAT 0
#endif

#ifndef PKTBUF_ENABLE_PARAM_CHECK
#define PKTBUF_ENABLE_PARAM_CHECK 0
#endif

struct pktbuf {
	union {
		struct pktbuf * next;
		uint32_t data[((PKTBUF_LEN) + 3) / 4]; 
    };
};

struct {
#if PKTBUF_ENABLE_STAT
	struct {
		uint32_t error_cnt;
		uint32_t alloc_cnt;
		uint32_t free_cnt;
	} stat;
#endif
	struct {
		struct pktbuf * first;
		struct pktbuf * last;
	} free;
	struct pktbuf pool[PKTBUF_POOL_SIZE];
} __pktbuf__;

const uint16_t pktbuf_len = (((PKTBUF_LEN) + 3) / 4) * 4;

void * pktbuf_alloc(void)
{
	struct pktbuf * p;
	uint32_t primask;

	/* critical section enter */
	primask = cm3_primask_get();
	cm3_primask_set(1);

	if ((p = __pktbuf__.free.first) != NULL) {
		if ((__pktbuf__.free.first = p->next) == NULL)
			__pktbuf__.free.last = (struct pktbuf *)&__pktbuf__.free.first;
#if PKTBUF_ENABLE_STAT
		__pktbuf__.stat.alloc_cnt++;
//		DCC_LOG1(LOG_TRACE, "alloc=%d", __pktbuf__.stat.alloc_cnt);
		DCC_LOG1(LOG_TRACE, "rem=%d", 
				 __pktbuf__.stat.alloc_cnt - __pktbuf__.stat.free_cnt);
	} else {
		__pktbuf__.stat.error_cnt++;
		DCC_LOG1(LOG_WARNING, "errors=%d", __pktbuf__.stat.error_cnt);
#endif
	}
	/* critical section exit */
	cm3_primask_set(primask);

	return (void *)p;
}

#if 0
static inline int __is_pktbuf(void * __p) {
	return ((uintptr_t)(__p) - (uintptr_t)__pktbuf__.pool) < 
		PKTBUF_POOL_SIZE * sizeof(struct pktbuf);
}
#endif

void pktbuf_free(void * ptr)
{
	struct pktbuf * p = (struct pktbuf *)ptr;
	struct pktbuf * q;
	uint32_t primask;

#if PKTBUF_ENABLE_PARAM_CHECK
	if (ptr == NULL) {
		DCC_LOG(LOG_ERROR, "NULL pointer!");
		return;
	}
#endif

	/* critical section enter */
	primask = cm3_primask_get();
	cm3_primask_set(1);

	p->next = NULL;
	q = __pktbuf__.free.last;
	__pktbuf__.free.last = p;
	q->next = p;

#if PKTBUF_ENABLE_STAT
	__pktbuf__.stat.free_cnt++;
//	DCC_LOG1(LOG_TRACE, "free=%d", __pktbuf__.stat.free_cnt);
	DCC_LOG1(LOG_TRACE, "rem=%d", 
			 __pktbuf__.stat.alloc_cnt - __pktbuf__.stat.free_cnt);
#endif

	/* critical section exit */
	cm3_primask_set(primask);
}

void pktbuf_pool_init(void)
{
	struct pktbuf * p;
	struct pktbuf * q;
	int i;

	DCC_LOG1(LOG_TRACE, "pool_size=%d", PKTBUF_POOL_SIZE);

	/* create a linked list of memory blocks */
	q = __pktbuf__.pool;
	p = (struct pktbuf *)&__pktbuf__.free.first;
	for (i = 0; i < PKTBUF_POOL_SIZE; ++i) {
		DCC_LOG1(LOG_JABBER, "pktbuf=%p", q);
		p->next = q;
		p = q;
		q++;
	}
	p->next = NULL;
	__pktbuf__.free.last = p;

#if PKTBUF_ENABLE_STAT
	__pktbuf__.stat.error_cnt = 0;
	__pktbuf__.stat.alloc_cnt = 0;
	__pktbuf__.stat.free_cnt = 0;
#endif
}

