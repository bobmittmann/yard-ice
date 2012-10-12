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

#include <string.h>
#include <sys/param.h>

int mbuf_queue_get(struct mbuf_queue * __q, void * __buf, 
				   int __offs, int __len)
{
	struct mbuf * m;
	uint8_t * src;
	uint8_t * dst;
	int offs;
	int cnt;
	int rem;
	int pos;
	int n;

	if ((__offs < 0) || (__len < 0)) {
		DCC_LOG2(LOG_PANIC, "offs=%d len=%d", __offs, __len);
	}

	DCC_LOG4(LOG_INFO, "offs=%d len=%d q.offs=%d q.len=%d", 
		__offs, __len, __q->offs, __q->len);

	if ((pos = __offs) > __q->len)
		return 0;

	rem = MIN(__len, __q->len - pos);

	if (rem == 0)
		return 0;

	dst = (uint8_t *)__buf;

	m = (struct mbuf *)__q->first;
	offs = __q->offs;
	while (pos) {
		n = MIN(pos, MBUF_DATA_SIZE - offs);
		offs += n;
		if (offs == MBUF_DATA_SIZE) {
			m = m->next;
			offs = 0;
		}
		pos -= n;
	}

	cnt = 0;
	while (rem) {
		n = MIN(rem, MBUF_DATA_SIZE - offs);
		DCC_LOG5(LOG_INFO, "dst=%04x m=%04x offs=%d rem=%d n=%d", 
			(int)dst, (int)m, offs, rem, n);

		src = (uint8_t *)m->data + offs;
		offs = 0;

		memcpy(dst, src, n);

		cnt += n;
		dst += n;
		rem -= n;

		m = m->next;
	}

	return cnt;
}

