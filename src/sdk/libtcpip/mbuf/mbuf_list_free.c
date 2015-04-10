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

void mbuf_list_free(struct mbuf * __q)
{
	struct mbuf * m;
	struct mbuf * p;

	DCC_LOG1(LOG_INFO, "{%05x}", (int)__q);

	m = (struct mbuf *)__q;

	thinkos_mutex_lock(__mbufs__.mutex);

	while (m != NULL) {
		p = m->next;
		__mbuf_release(m);
		m = p;
	}

	thinkos_cond_signal(__mbufs__.cond);
	thinkos_mutex_unlock(__mbufs__.mutex);

	return;
}

