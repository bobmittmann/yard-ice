/* $Id: mbuf.h,v 2.12 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	sys/mbuf.h
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment: 
 * Copyright(c) 2005-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

/*! \defgroup sys system
 * \{
 */
/*! \defgroup sys_mbuf sys/mbuf.h
 *	\brief MBUF
 *
 * \{
 */
/*! \file
 *
 *	Revision tag: $Revision: 2.12 $
 *	\author Robinson Mittmann (bob@boreste.com)
 */

#ifndef __SYS_MBUF_H__
#define __SYS_MBUF_H__

#include <stdint.h>

struct mbuf {
	int16_t next;
	uint8_t len;
	uint8_t offs;
	uint32_t data[76 / 4];
};

struct mlink {
	struct mlink * next;
	uint32_t data[0];
};

struct mlink_list {
	struct mlink * first;
	struct mlink * last;
};

#if 0
struct mbqueue {
	/* number of bytes enqueued */
	uint16_t len;
	/* number of mbufs */
	uint8_t count;
	/* offset of free bytes in the last mbuf */
	uint8_t offs;
	struct mbuf * first;
};
#endif

struct mbuf_queue {
	uint8_t offs;
	uint16_t len;
	struct mbuf * first;
	struct mbuf * last;
};

#if 0
extern inline int mbuf_avail(void) {
	return __mbufs__.max - __mbufs__.used;
};

extern inline int mbuf_used(void) {
	return __mbufs__.used;
};

extern inline int mbuf_max(void) {
	return __mbufs__.max;
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Alloc a single memory buffer (mbuf). Blocks if none available.
 */
void * mbuf_alloc(void);

/*
 * Release a memory buffer
 */
int mbuf_free(void * __p);

#if 0
/*
 * Try to alloc a single memory buffer (mbuf), do not block.
 */
void * mbuf_try_alloc(void);

/*
 * Try to alloc a single memory buffer (mbuf) or block and do not alloc.
 */
void * mbuf_try_alloc_and_wait(int __mutex);

/*
 * Return the amount (bytes) of memory available..
 */
int mbuf_memavail(void);

int mbuf_wait(int mutex);

int __attribute__ ((const)) mbuf_size(void);
#endif

void mlink_list_init(struct mlink_list * __l);

void mlink_list_insert(struct mlink_list * __l, void * __p);

int mlink_list_remove(struct mlink_list * __l, void * __p);

int mlink_list_lookup(struct mlink_list * __l, void * __p);

#if 0

int mlink_list_dump(struct mlink_list * __l);

int mbuf_queue_init(struct mbuf_queue * __q);

int mbuf_queue_add(struct mbuf_queue * __q, const void * __buf, int __len);

int mbuf_queue_remove(struct mbuf_queue * __q, void * __buf, int __len);

int mbuf_queue_free(struct mbuf_queue * __q);

int mbuf_queue_get(struct mbuf_queue * __q, void * __buf, 
				   int __offs, int __len);

int mbuf_queue_trim(struct mbuf_queue * __q, int __size);
#endif


struct mlink * mbuf_list_alloc(int __len);

void mbuf_list_free(struct mlink * __q);

int mbuf_list_write(struct mlink * __q, const void * __buf, int __len);

int mbuf_list_read(struct mlink * __q, void * __buf, int __len);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_MBUF_H__ */

/*! \} */
/*! \} */

