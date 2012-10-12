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

int mbuf_list_read(struct mbuf * __q, void * __buf, int __len)
{
	struct mbuf * p;
	uint8_t * dst;
	int rem;

	DCC_LOG1(LOG_INFO, "{%05x}", (int)__q);

	dst = (uint8_t *)__buf;
	p = (struct mbuf *)__q;

	rem = __len;

	while ((rem >= MBUF_DATA_SIZE) && (p != NULL)) {
		memcpy(dst, p->data, MBUF_DATA_SIZE);
		p = p->next;
		dst += MBUF_DATA_SIZE;
		rem -= MBUF_DATA_SIZE;
	}

	if ((rem > 0) && (p != NULL)) {
		memcpy(dst, p->data, rem);
		return __len;
	}

	return __len - rem;
}

