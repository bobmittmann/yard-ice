/* $Id: mbuf.c,v 2.5 2008/05/28 22:32:21 bob Exp $ 
 *
 * File:	mbuf.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2005-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "__mbuf.h"

int mbuf_alloc_check(void)
{
	struct mbuf * m;
	int ret = 0;
	int n;

	thinkos_mutex_lock(__mbufs__.mutex);

	n = 0;
	m = __mbufs__.free.first;

	/* count the free blocks */
	while (m != NULL) {
		n++;
		m = m->next;
	}

	if ((n + __mbufs__.used) != __mbufs__.max) {
		DCC_LOG4(LOG_ERROR, "used=%d max=%d free=%d avail=%d",
			   __mbufs__.used, __mbufs__.max, 
			   __mbufs__.max - __mbufs__.used, n);
		ret = -1;
	}

	thinkos_mutex_unlock(__mbufs__.mutex);

	return ret;
}

