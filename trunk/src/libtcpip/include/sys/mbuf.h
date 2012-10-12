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

#ifndef __SYS_MBUF_H__
#define __SYS_MBUF_H__

#include <stdint.h>

struct mbuf;

struct mbuf_queue {
	uint8_t offs;
	uint16_t len;
	struct mbuf * first;
	struct mbuf * last;
};

#ifdef __cplusplus
extern "C" {
#endif

void mbuf_init(void);

/*
 * Alloc a single memory buffer (mbuf). Blocks if none available.
 */
void * mbuf_alloc(void);

/*
 * Try to alloc a single memory buffer (mbuf), do not block.
 */
void * mbuf_try_alloc(void);

/*
 * Try to alloc a single memory buffer (mbuf) or block and do not alloc.
 */
void * mbuf_try_alloc_and_wait(int __mutex);

/*
 * Release a memory buffer
 */
int mbuf_free(void * __p);

/*
 * Return the amount (bytes) of memory available..
 */
int mbuf_memavail(void);

int mbuf_wait(int __mutex);

int mbuf_avail(void);

int mbuf_used(void);

int mbuf_max(void);

int mbuf_alloc_check(void);

int __attribute__ ((const)) mbuf_size(void);


int mbuf_queue_init(struct mbuf_queue * __q);

int mbuf_queue_add(struct mbuf_queue * __q, const void * __buf, int __len);

int mbuf_queue_remove(struct mbuf_queue * __q, void * __buf, int __len);

int mbuf_queue_free(struct mbuf_queue * __q);

int mbuf_queue_get(struct mbuf_queue * __q, void * __buf, 
				   int __offs, int __len);

int mbuf_queue_trim(struct mbuf_queue * __q, int __size);


struct mbuf * mbuf_list_alloc(int __len);

void mbuf_list_free(struct mbuf * __q);

int mbuf_list_write(struct mbuf * __q, const void * __buf, int __len);

int mbuf_list_read(struct mbuf * __q, void * __buf, int __len);


#ifdef __cplusplus
}
#endif

#endif /* __SYS_MBUF_H__ */

/*! \} */
/*! \} */

