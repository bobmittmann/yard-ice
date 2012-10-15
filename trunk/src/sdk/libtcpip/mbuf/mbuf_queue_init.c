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

int mbuf_queue_init(struct mbuf_queue * __q)
{
	DCC_LOG1(LOG_INFO, "{%05x}", (int)__q);

	__q->first = NULL;
	__q->last = (struct mbuf *)&__q->first;
	__q->len = 0;
	__q->offs = 0;

	return 0;
}

