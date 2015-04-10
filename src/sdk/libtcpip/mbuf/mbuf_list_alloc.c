/* 
 * File:	mbuf_list.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2005-2010 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "__mbuf.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct mbuf * mbuf_list_alloc(int __len)
{
	struct mbuf * m;
	struct mbuf * p;
	struct mbuf * q;
	int n = (__len + (MBUF_DATA_SIZE - 1)) / MBUF_DATA_SIZE;
	int i;

	thinkos_mutex_lock(__mbufs__.mutex);

	if (n > (__mbufs__.max - __mbufs__.used))
		return NULL;

	m = __mbufs__.free.first;
	p = m;
	q = m;
	for (i = 0; i < n; i++) {
		if ((q = p) == NULL) {
			DCC_LOG2(LOG_PANIC, "mbuf pool inconsistency used=%d max=%d",
					 __mbufs__.used, __mbufs__.max);
		}
		p = p->next;
	}

	q->next = NULL;

	if ((__mbufs__.free.first = p) == NULL)
		__mbufs__.free.last = (struct mbuf *)&__mbufs__.free.first;

	__mbufs__.used += n;

	DCC_LOG3(LOG_INFO, "mbuf=%05x used=%d free=%d", (int)m, 
		__mbufs__.used, __mbufs__.max - __mbufs__.used);

	thinkos_mutex_unlock(__mbufs__.mutex);

	return (struct mbuf *)m;
}

