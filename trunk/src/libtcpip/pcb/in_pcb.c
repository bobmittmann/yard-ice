/* $Id: in_pcb.c,v 2.9 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	in_pcb.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment: Internet protocol control blocks
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
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

#if 0
static void pcb_list_init(struct pcb_list * __l) {
	__l->first = NULL;
	__l->last = (struct pcb_link *)&__l->first;
}

void pcb_list_insert(struct pcb_list * __l, void * __p) {
	struct pcb_link * m;

	m = (struct pcb_link *)__p - 1;

	__l->last->next = m;
	__l->last = m;

	m->next = NULL;
}

int pcb_list_lookup(struct pcb_list * __l, void * __p)
{
	struct pcb_link * p;
	struct pcb_link * q;

	p = (struct pcb_link *)__p - 1;
	q = (struct pcb_link *)&__l->first;

	while ((q = q->next)) {
		if (q == p)
			return 0;
	}

	DCC_LOG(LOG_WARNING, "not found!");
	return -1;
}

int pcb_list_remove(struct pcb_list * __l, void * __p)
{
	struct pcb_link * m;
	struct pcb_link * q;
	struct pcb_link * p;

	p = (struct pcb_link *)__p - 1;
	q = (struct pcb_link *)&__l->first;

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
#endif

/*
 * Insert the pcb in a list
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

static inline struct pcb_link * __link(struct pcb * __inp) {
	return (struct pcb_link *)((uintptr_t)__inp - sizeof(struct pcb_link *));
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

int pcb_enum(int (* __callback)(struct pcb *, void *), 
			   void * __parm, struct pcb_list * __list)
{
	struct pcb_link * q;
	struct pcb * pcb;

	q = (struct pcb_link *)&__list->first;

	while ((q = q->next)) {
		pcb = &q->pcb;

		if (__callback != NULL) {
			if (__callback(pcb, __parm))
				break;
		}
	}

	return 0;
}


