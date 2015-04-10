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

int mbuf_queue_free(struct mbuf_queue * __q)
{
	struct mbuf * m;
	struct mbuf * p;

	DCC_LOG3(LOG_INFO, "{%05x} q.offs=%d q.len=%d", 
			 (int)__q,  __q->offs, __q->len);

	m = __q->first;

	thinkos_mutex_lock(__mbufs__.mutex);

	while (m) {
		p = m->next;
		__mbuf_release(m);
		m = p;
	}

	thinkos_cond_signal(__mbufs__.cond);
	thinkos_mutex_unlock(__mbufs__.mutex);

	__q->first = NULL;
	__q->last = (struct mbuf *)&__q->first;
	__q->offs = 0;
	__q->len = 0;

	return 0;
}

