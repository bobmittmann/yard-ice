/* 
 * File:	 pktbuf.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <arch/cortex-m3.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef CONFIG_H
#include "config.h"
#endif

/* ----------------------------------------------------------------------
 * packet buffer pool
 * ----------------------------------------------------------------------
 */

#ifndef PKTBUF_LEN 
#define PKTBUF_LEN (512 + 16)
#endif

#ifndef PKTBUF_POOL_SIZE
#define PKTBUF_POOL_SIZE 4
#endif

struct pktbuf {
	union {
		struct pktbuf * next;
		uint32_t data[((PKTBUF_LEN) + sizeof(uint32_t) - 1) 
			/ sizeof(uint32_t)];
    };
};

struct {
	uint32_t error;
	struct pktbuf * free;
	struct pktbuf buf[PKTBUF_POOL_SIZE];
} pktbuf_pool;

const unsigned int pktbuf_len = PKTBUF_LEN;

void * pktbuf_alloc(void)
{
	struct pktbuf * buf;
	uint32_t primask;

	/* critical section enter */
	primask = cm3_primask_get();
	cm3_primask_set(1);

	if ((buf = pktbuf_pool.free) != NULL)
		pktbuf_pool.free = buf->next;
	else
		pktbuf_pool.error++;

	/* critical section exit */
	cm3_primask_set(primask);

	return (void *)buf;
}

void pktbuf_free(void * ptr)
{
	struct pktbuf * buf = (struct pktbuf *)ptr;
	uint32_t primask;

	/* critical section enter */
	primask = cm3_primask_get();
	cm3_primask_set(1);

	buf->next = pktbuf_pool.free;
	pktbuf_pool.free = buf;

	/* critical section exit */
	cm3_primask_set(primask);

}

void pktbuf_pool_init(void)
{
	int i;

	pktbuf_pool.free = &pktbuf_pool.buf[0];

	for (i = 0; i < PKTBUF_POOL_SIZE - 1; i++) {
		pktbuf_pool.buf[i].next = &pktbuf_pool.buf[i + 1];
	}

	pktbuf_pool.buf[i].next = NULL;
	pktbuf_pool.error = 0;
}


