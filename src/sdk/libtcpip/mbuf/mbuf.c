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

struct mbuf_sys __mbufs__;

void mbuf_init(void)
{
	struct mbuf * p;
	struct mbuf * q;
	int i;

	__mbufs__.cond = thinkos_cond_alloc();
	__mbufs__.mutex = thinkos_mutex_alloc();
	__mbufs__.used = 0;
	__mbufs__.max = MBUF_POOL_SIZE;

	/* create a linked list of memory blocks */
	q = __mbufs__.pool;
	p = (struct mbuf *)&__mbufs__.free.first;
	for (i = 0; i < MBUF_POOL_SIZE; i++) {
		p->next = q;
		p = q;
		q++;
	}

	p->next = NULL;
	__mbufs__.free.last = p;


	DCC_LOG3(LOG_TRACE, "cond=%d mutex=%d max=%d", 
		__mbufs__.cond, __mbufs__.mutex, __mbufs__.max);
}

