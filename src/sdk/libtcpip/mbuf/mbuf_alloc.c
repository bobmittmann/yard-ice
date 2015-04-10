/* 
 * File:	mbuf_alloc.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2007-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "__mbuf.h"

/*
 * Allocate a new mbuf, blocks if there is none available.
 */
void * mbuf_alloc(void)
{
	struct mbuf * m;

	thinkos_mutex_lock(__mbufs__.mutex);

	while ((m = __mbufs__.free.first) == NULL) {
		DCC_LOG2(LOG_TRACE, "<%d> cond [%d]", thinkos_thread_self(), 
				 __mbufs__.cond);
		
		thinkos_cond_wait(__mbufs__.cond, __mbufs__.mutex);
	}

	if ((__mbufs__.free.first = m->next) == NULL)
		__mbufs__.free.last = (struct mbuf *)&__mbufs__.free.first;

	__mbufs__.used++;

	DCC_LOG3(LOG_INFO, "mbuf=%05x used=%d free=%d", (int)m, 
		__mbufs__.used, __mbufs__.max - __mbufs__.used);

	thinkos_mutex_unlock(__mbufs__.mutex);

	return (void *)m;
}

