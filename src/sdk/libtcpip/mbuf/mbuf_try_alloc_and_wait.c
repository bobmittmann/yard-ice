/* 
 * File:	mbuf_try_alloc_and_wait.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "__mbuf.h"

void * mbuf_try_alloc_and_wait(int mutex)
{
	struct mbuf * m;
	void * p;

	thinkos_mutex_lock(__mbufs__.mutex);

	if ((m = __mbufs__.free.first) != NULL) {

		if ((__mbufs__.free.first = m->next) == NULL)
			__mbufs__.free.last = (struct mbuf *)&__mbufs__.free.first;

		__mbufs__.used++;

		DCC_LOG3(LOG_TRACE, "mbuf=%05x used=%d free=%d", (int)m, 
			__mbufs__.used, __mbufs__.max - __mbufs__.used);

		p = (void *)m;
	} else {
		thinkos_mutex_unlock(mutex);
		thinkos_cond_wait(__mbufs__.cond, __mbufs__.mutex);
		thinkos_mutex_lock(mutex);
		p = NULL;
	}

	thinkos_mutex_unlock(__mbufs__.mutex);

	return p;
}

