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

int mbuf_list_write(struct mbuf * __q, const void * __buf, int __len)
{
	struct mbuf * p;
	uint8_t * src;
	int rem;

	DCC_LOG1(LOG_INFO, "{%05x}", (int)__q);

	src = (uint8_t *)__buf;
	p = (struct mbuf *)__q;

	rem = __len;

	while ((rem >= MBUF_DATA_SIZE) && (p != NULL)) {
		memcpy(p->data, src, MBUF_DATA_SIZE);
		p = p->next;
		src += MBUF_DATA_SIZE;
		rem -= MBUF_DATA_SIZE;
	}

	if ((rem > 0) && (p != NULL)) {
		memcpy(p->data, src, rem);
		return __len;
	}

	return __len - rem;
}

