/* $Id: mbuf.h,v 2.12 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	sys/link.h
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment: 
 * Copyright(c) 2005-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __SYS_MLINK_H__
#define __SYS_MLINK_H__

#include <stdint.h>

struct mlink {
	struct mlink * next;
};

struct mlink_list {
	struct mlink * first;
	struct mlink * last;
};

#ifdef __cplusplus
extern "C" {
#endif


void mlink_list_insert(struct mlink_list * __l, void * __p);

int mlink_list_lookup(struct mlink_list * __l, void * __p);

int mlink_list_remove(struct mlink_list * __l, void * __p);


#ifdef __cplusplus
}
#endif

#endif /* __SYS_MLINK_H__ */

static void mlink_list_init(struct mlink_list * __l) {
	__l->first = NULL;
	__l->last = (struct mlink *)&__l->first;
}

void mlink_list_insert(struct mlink_list * __l, void * __p) {
	struct mlink * m;

	m = (struct mlink *)__p - 1;

	__l->last->next = m;
	__l->last = m;

	m->next = NULL;
}

int mlink_list_lookup(struct mlink_list * __l, void * __p)
{
	struct mlink * p;
	struct mlink * q;

	p = (struct mlink *)__p - 1;
	q = (struct mlink *)&__l->first;

	while ((q = q->next)) {
		if (q == p)
			return 0;
	}

	DCC_LOG(LOG_WARNING, "not found!");
	return -1;
}

int mlink_list_remove(struct mlink_list * __l, void * __p)
{
	struct mlink * m;
	struct mlink * q;
	struct mlink * p;

	p = (struct mlink *)__p - 1;
	q = (struct mlink *)&__l->first;

	while ((m = q->next)) {
		if (p == m) {
			m = m->next;
			q->next = m;

			if (__l->last == p)
				__l->last = q;

			return 0;
		}
		q = m;
	}

	DCC_LOG(LOG_WARNING, "not found!");
	return -1;
}

