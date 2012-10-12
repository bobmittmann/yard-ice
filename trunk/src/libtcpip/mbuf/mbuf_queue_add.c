/* $Id: new.c,v 2.9 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	mqueue.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2005-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "__mbuf.h"

#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

int mbuf_queue_add(struct mbuf_queue * __q, const void * __buf, int __len)
{
	struct mbuf * m;
	uint8_t * src;
	uint8_t * dst;
	int offs;
	int rem;
	int n;
	int tail;
	int i = 0;

	if (__len < 0) {
		DCC_LOG1(LOG_PANIC, "len=%d", __len);
		return 0;
	}

	if (__len == 0) {
		return 0;
	}

	DCC_LOG4(LOG_INFO, "{%05x} q.offs=%d q.len=%d, len=%d", 
			 (int)__q,  __q->offs, __q->len, __len);

	tail = __q->offs + __q->len;
	src = (uint8_t *)__buf;
	rem = __len;

	/* 
	 * Append data to the end of last buffer if there is available space
	 */
	offs = tail % MBUF_DATA_SIZE;
	if (offs != 0) {
		dst = (uint8_t *)__q->last->data + offs;
		n = MIN(rem, MBUF_DATA_SIZE - offs);
		memcpy(dst, src, n);
		__q->len += n;
		src += n;
		rem -= n;
	}

	while (rem) {
		/* 
		 * Alloc a new mbuf and append to queue tail
		 */
		if ((m = (struct mbuf *)mbuf_try_alloc()) == NULL) {
			DCC_LOG(LOG_INFO, "mbuf_try_alloc() fail!");
			break;
		} 
		i++;

		DCC_LOG2(LOG_INFO, "{%05x} new mbuf=<%05x>", (int)__q, dst);

		__q->last->next = m;
		__q->last = m;
		m->next = NULL;
		
		dst = (uint8_t *)m->data;
		n = MIN(rem, MBUF_DATA_SIZE);
		memcpy(dst, src, n);
		__q->len += n;
		src += n;
		rem -= n;
	}

	if (rem != 0) {
		DCC_LOG2(LOG_WARNING, "mbuf alloc fail, +%d (%d)", __len - rem, i);
	} else {
		DCC_LOG2(LOG_INFO, "+%d (%d)", __len, i);
	}

	return __len - rem;
}

