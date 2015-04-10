/* $Id: mbuf.h,v 2.12 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	__mbuf.h
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment: 
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef ____MBUF_H__
#define ____MBUF_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef MBUF_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#include <sys/mbuf.h>
#include <thinkos.h>
#include <stdlib.h>

#ifndef __MBUF_SIZE
#define __MBUF_SIZE 128
#endif

#define MBUF_DATA_SIZE (__MBUF_SIZE - 4)

#ifndef MBUF_POOL_SIZE
#define MBUF_POOL_SIZE 64
#endif

#ifndef ENABLE_MBUF_STATS
#define ENABLE_MBUF_STATS 0
#endif

struct mbuf {
	struct mbuf * next;
	uint32_t data[MBUF_DATA_SIZE / 4];
};

struct mbuf_sys {
	uint8_t max;
	uint8_t used;
	uint8_t mutex; /* mbuf strucutre access mutex */
	uint8_t cond;
	/* mbuf free list */
	struct {
		struct mbuf * first;
		struct mbuf * last;
	} free;
	struct mbuf pool[MBUF_POOL_SIZE];
};

extern struct mbuf_sys __mbufs__;

#define IS_MBUF(P) ((uintptr_t)((uintptr_t)(P) - (uintptr_t)__mbufs__.pool) < \
					(MBUF_POOL_SIZE * sizeof(struct mbuf)))

static inline int __is_mbuf(void * __p) {
	return IS_MBUF(__p);
}

static inline void __mbuf_release(struct mbuf * __p) {
	struct mbuf * q = __mbufs__.free.last;
	q->next = __p;
	__mbufs__.free.last = __p;
	__p->next = NULL;
	__mbufs__.used--; /* update statistics */
}

#ifdef __cplusplus
extern "C" {
#endif
	
#ifdef __cplusplus
}
#endif

#endif /* ____MBUF_H__ */

