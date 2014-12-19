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

int mbuf_queue_trim(struct mbuf_queue * __q, int __len)
{
	struct mbuf * m;
	struct mbuf * p;
	int cnt;
	int rem;
	int n;
	int i = 0;

	if (__len < 0) {
		DCC_LOG1(LOG_PANIC, "len=%d", __len);
	}

	rem = MIN(__len, __q->len);

	DCC_LOG3(LOG_INFO, "len=%d q.offs=%d q.len=%d", 
			 __len, __q->offs, __q->len);

	/* queue empty or __len == 0 */
	if (rem == 0)
		return 0;

	p = (struct mbuf *)&__q->first;
	m = p->next;
	cnt = 0;

	while (rem) {

		n = MIN(rem, MBUF_DATA_SIZE - __q->offs);
		DCC_LOG3(LOG_INFO, "q.offs=%d rem=%d n=%d", __q->offs, rem, n);

		cnt += n;
		__q->len -= n;
		if (__q->len == 0) {
			DCC_LOG(LOG_INFO, "remove all");
			/* remove all */
			__q->offs = 0;
			__q->last = p;
			p->next = m->next;
			mbuf_free(m);
			i++;
			break;
		}

		rem -= n;
		__q->offs += n;

		if (__q->offs == MBUF_DATA_SIZE) {

			DCC_LOG(LOG_INFO, "remove mbuf");

			/* remove the head mbuf */
			__q->offs = 0;

			p->next = m->next;
			mbuf_free(m);
			i++;
			m = p->next;
		}
	}

	DCC_LOG2(LOG_INFO, "-%d (%d)", cnt, i);

	return cnt;
}

