/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libtcpip.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file in_pcb.c
 * @brief Internet Protocol Control Block
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef PCB_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#include <sys/pcb.h>
#include <sys/mbuf.h>

#include <errno.h>
#include <stdlib.h>

/*
 * Insert the pcb into a list
 */
static void __insert(struct pcb_list * __list, struct pcb_link * __p)
{
	__list->last->next = __p;
	__list->last = __p;
	__p->next = NULL;
}

/*
 * Remove a pcb from a list
 */
static int __remove(struct pcb_list * __list, struct pcb_link * __p)
{
	struct pcb_link * m;
	struct pcb_link * q;

	q = (struct pcb_link *)&__list->first;

	while ((m = q->next)) {
		if (__p == m) {
			m = m->next;
			q->next = m;

			if (__list->last == __p)
				__list->last = q;

			return 0;
		}
		q = m;
	}

	DCC_LOG(LOG_WARNING, "not found!");
	return -1;
}

/*
 * Lookup for a pcb in a list
 */
static int __lookup(struct pcb_list * __l, struct pcb_link * __p)
{
	struct pcb_link * q;

	q = (struct pcb_link *)&__l->first;

	while ((q = q->next)) {
		if (q == __p)
			return 0;
	}

	DCC_LOG(LOG_WARNING, "not found!");
	return -1;
}

void pcb_list_init(struct pcb_list * __list)
{
	__list->first = NULL;
	__list->last = (struct pcb_link *)&__list->first;
}

struct pcb * pcb_alloc(void)
{
	struct pcb_link * p;

	/* get a new memory descriptor */
	if ((p = (struct pcb_link *)mbuf_alloc()) == NULL) {
		DCC_LOG(LOG_WARNING, "could not allocate a mbuf");
		return NULL;
	}

	return &p->pcb;
}

int pcb_free(struct pcb * __inp)
{
	return mbuf_free(__link(__inp));
}

/*
 * Insert the pcb in a list
 */
void pcb_insert(struct pcb * __inp, struct pcb_list * __list)
{
	__insert(__list, __link(__inp));
}


/*
 * Remove the first pcb from a list
 */
struct pcb * pcb_remove_head(struct pcb_list * __list)
{
	struct pcb_link * m;
	struct pcb_link * q;
	struct pcb_link * p;

	q = (struct pcb_link *)&__list->first;

	if ((p = q->next) == NULL) {
		DCC_LOG(LOG_WARNING, "list empty!");
		return NULL;
	} 

	m = p->next;
	q->next = m;

	if (__list->last == p)
		__list->last = q;

	return &p->pcb;
}

/*
 * Remove a pcb from a list
 */
int pcb_remove(struct pcb * __inp, struct pcb_list * __list)
{
	return __remove(__list, __link(__inp));
}

/*
 * Move a pcb between lists
 */
int pcb_move(struct pcb * __inp, struct pcb_list * __from, 
			 struct pcb_list * __to)
{
	int ret;

	if ((ret = __remove(__from, __link(__inp))) < 0)
		return ret;

	__insert(__to, __link(__inp));

	return ret;
}

/*
 * Remove a pcb from a list
 */
int pcb_release(struct pcb * __inp, struct pcb_list * __list)
{
	__remove(__list, __link(__inp));
	return mbuf_free(__link(__inp));
}

/* lookup for an partial, most restrictive, match of address and port */
struct pcb * pcb_wildlookup(in_addr_t __faddr, uint16_t __fport, 
	in_addr_t __laddr, uint16_t __lport, struct pcb_list * __list)
{
	struct pcb_link * q;
	struct pcb * match;
	struct pcb * pcb;
	int wildcard;
	int matchwild;

	DCC_LOG2(LOG_INFO, "fport=%d lport=%d", ntohs(__fport), ntohs(__lport));

	match = NULL;
	matchwild = 3;
	wildcard = 0;

	q = (struct pcb_link *)&__list->first;

	while ((q = q->next)) {
		pcb = &q->pcb;

		DCC_LOG2(LOG_INFO, "<%04x> lport=%d", (int)pcb, pcb->inp_lport);

		if (pcb->inp_lport != __lport) {
			DCC_LOG3(LOG_INFO, "<%04x> pcb->lport=%d != lport=%d", 
					 (int)pcb, ntohs(pcb->inp_lport), ntohs(__lport));
			continue;
		}

		DCC_LOG2(LOG_INFO, "<%04x> lport=%d match", 
				 (int)pcb, ntohs(__lport));

		if (pcb->inp_laddr != INADDR_ANY) {
			if (__laddr == INADDR_ANY)
				wildcard++;
			else
				if (pcb->inp_laddr != __laddr)
					continue;
		} else {
			if (__laddr != INADDR_ANY)
				wildcard++;
		}
		
		if (pcb->inp_faddr != INADDR_ANY) {
			if (__faddr == INADDR_ANY)
				wildcard++;
			else
				if ((pcb->inp_faddr != __faddr) ||
				    (pcb->inp_fport != __fport))
					continue;
		} else {
			if (__faddr != INADDR_ANY)
				wildcard++;
		}

		if (wildcard < matchwild) {
			match = pcb;
			if (wildcard == 0) {
				/* TODO: change the list entry point */
//				*__list = m; 
				break;
			}
			matchwild = wildcard;
		}
	}
	
	return match;
}

/* lookup for an exact match of address and port */
struct pcb * pcb_lookup(in_addr_t __faddr, uint16_t __fport, 
	in_addr_t __laddr, uint16_t __lport, struct pcb_list * __list)
{
	struct pcb_link * q;
	struct pcb * pcb;
//	int i;

	DCC_LOG2(LOG_INFO, "fport=%d lport=%d", ntohs(__fport), ntohs(__lport));

	q = (struct pcb_link *)&__list->first;

	while ((q = q->next)) {
		pcb = (struct pcb *)&q->pcb;

//	for (i = 0; i < __list->count; i++) {
//		pcb = (struct pcb *)mbuf_ptr(__list->mi[i]);

		if ((pcb->inp_lport == __lport) &&
		    (pcb->inp_laddr == __laddr) &&
		    (pcb->inp_faddr == __faddr) &&
		    (pcb->inp_fport == __fport)) {
			
			DCC_LOG1(LOG_INFO, "inp=0x%p", pcb);
			
			/* TODO: change the list entry point */
//			*__list = m;
			return  pcb;
		}
	}

	DCC_LOG(LOG_MSG, "not found");

	return NULL;
}

/* link an adress and port to the PCB */
int pcb_bind(struct pcb * __inp, in_addr_t __addr, uint16_t __port)
{
/* XXX: sockets deal with this already ... 
	if ((__inp->inp_lport != 0) || (__inp->inp_laddr != INADDR_ANY)) {
		DCC_LOG(LOG_WARNING, "<%04x> already bound.", (int)__inp);
		return EINVAL;
	}
*/

	DCC_LOG3(LOG_TRACE, "<%05x> %I:%d", (int)__inp, __addr, ntohs(__port));

	/* TODO: ephemeral ports */
	__inp->inp_lport = __port;
	__inp->inp_laddr = __addr;

	return 0;
}

/* connect the pcb to a remote addr and port */
int pcb_connect(struct pcb * __inp, in_addr_t __addr, uint16_t __port)
{
	if (__port == 0) {
		DCC_LOG1(LOG_WARNING, "<%04x> invalid port.", (int)__inp);
		return -EADDRNOTAVAIL;
	}

	DCC_LOG3(LOG_TRACE, "<%05x> %I:%d", (int)__inp, __addr, ntohs(__port));

	__inp->inp_faddr = __addr;
	__inp->inp_fport = __port;

	return 0;
}

int pcb_find(struct pcb * __inp, struct pcb_list * __list)
{
	return __lookup(__list, __link(__inp));
}

