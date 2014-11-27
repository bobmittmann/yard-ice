/* 
 * Copyright(C) 2014 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the MicroJs
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
 * @file microjs_sym.c
 * @brief Symbol table
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#define __MICROJS_I__
#include "microjs-i.h"

#include <string.h>

#include <sys/dcclog.h>

/* --------------------------------------------------------------------------
   Symbol table
   -------------------------------------------------------------------------- */

struct symtab * symtab_init(uint32_t sym_buf[], 
							unsigned int buf_len)
{
	struct symtab * tab = (struct symtab *)sym_buf;

	tab->rt.stack_sz = 0; /* runtime stack limit */
	tab->rt.data_sz = 0; /* runtime data limit */
	/* top of the symbol table */
	tab->top = buf_len - sizeof(struct symtab);
	/* references are allocated top-down */
	tab->sp = tab->top;
	tab->fp = tab->top;
	/* symbols are allocated bottom-up */
	tab->bp = 0;

#if MICROJS_TRACE_ENABLED
	/* initialize temporary labels */
	tab->tmp_lbl = 0;
#endif
	/* insert the end of list dummy object */
	tab->buf[0].prev = 0;
	tab->buf[0].next = 0;

	DCC_LOG3(LOG_INFO, "bp=%d sp=%d fp=%d", tab->bp, tab->sp, tab->fp);

	return tab;
}

struct microjs_rt * symtab_rt_get(struct symtab * tab)
{
	return &tab->rt;
}

struct symstat symtab_state_save(struct symtab * tab)
{
	struct symstat st;

	st.sp = tab->sp;
	st.bp = tab->bp;

	return st;
}

void symtab_state_rollback(struct symtab * tab, struct symstat st)
{
	/* remove all stack frames except the global one */
	while (tab->fp != tab->top) {
		sym_sf_pop(tab);
		DCC_LOG2(LOG_INFO, "sp=%d fp=%d", tab->sp, tab->fp);
	}

	tab->sp = st.sp;
	tab->bp = st.bp;
}

/* --------------------------------------------------------------------------
   Symbol table stack
   -------------------------------------------------------------------------- */

int sym_push(struct symtab * tab, const void * ptr,  unsigned int len)
{
	int sp = tab->sp;
	uint8_t * dst;
	uint8_t * src;
	int i;
	
	DCC_LOG3(LOG_INFO, "bp=%d sp=%d len=%d", tab->bp, tab->sp, len);

	sp -= len;
	/* compute 2 bytes for the end of list marker */
	if ((tab->bp + 2) > sp)
		return -ERR_SYM_PUSH_FAIL;

	dst = (uint8_t *)&tab->buf + sp;
	src = (uint8_t *)ptr;
	for(i = 0; i < len; ++i)
		dst[i] = src[i];

	tab->sp = sp;
	return 0;
}

void sym_pick(struct symtab * tab, int pos, void * ptr,  unsigned int len)
{
	uint8_t * dst;
	uint8_t * src;
	int i;

	src = (uint8_t *)&tab->buf + tab->fp - len;
	dst = (uint8_t *)ptr;
	for(i = 0; i < len; ++i)
		dst[i] = src[i];
}

int sym_pop(struct symtab * tab, void * ptr,  unsigned int len)
{
	int sp = tab->sp;
	uint8_t * dst;
	uint8_t * src;
	int i;

	DCC_LOG3(LOG_INFO, "bp=%d sp=%d len=%d", tab->bp, tab->sp, len);

	if (sp >= tab->fp)
		return -ERR_SYM_POP_FAIL;

	if (ptr != NULL) {
		src = (uint8_t *)&tab->buf + sp;
		dst = (uint8_t *)ptr;
		for(i = 0; i < len; ++i)
			dst[i] = src[i];
	}

	tab->sp = sp + len;

	return 0;
}

#if 0
static bool sym_push_str(struct symtab * tab, const char * s,  unsigned int len)
{
	int sp = tab->sp;
	uint8_t * dst;
	uint8_t * src;
	int i;
	
	DCC_LOG3(LOG_INFO, "bp=%d sp=%d len=%d", tab->bp, tab->sp, len + 1);

	sp -= len + 1;
	if (tab->bp > sp)
		return false;

	dst = (uint8_t *)&tab->buf + sp;
	src = (uint8_t *)s;
	for(i = 0; i < len; ++i)
		dst[i] = src[i];
	dst[i] = '\0';

	tab->sp = sp;

	return true;
}
#endif

/* --------------------------------------------------------------------------
   Stack frame
   -------------------------------------------------------------------------- */

/* Push the stack frame */
int sym_sf_push(struct symtab * tab)
{
	struct sym_sf sf;
	int ret;

	sf.prev = tab->fp;
	sf.bp = tab->bp;

	if ((ret = sym_push(tab, &sf, sizeof(sf))) == 0)
		tab->fp = tab->sp;

	return ret;
}

/* Pop the stack frame */
int sym_sf_pop(struct symtab * tab)
{
	struct sym_obj * obj;
	int sp = tab->fp; /* use frame pointer as reference */
	struct sym_sf sf;
	uint8_t * dst;
	uint8_t * src;
	int i;

	if (sp >= tab->top)
		return ERR_SYM_POP_FAIL;

	src = (uint8_t *)&tab->buf + sp;
	dst = (uint8_t *)&sf;
	for(i = 0; i < sizeof(sf); ++i)
		dst[i] = src[i];

	tab->sp = sp + sizeof(sf);
	tab->fp = sf.prev;
	tab->bp = sf.bp;

	/* insert the end of list dummy */
	obj = (struct sym_obj *)((void *)tab->buf + tab->bp);
	obj->next = 0;

	return 0;
}

/* Pick a stack frame */
static bool sym_sf_get(struct symtab * tab, struct sym_sf * sf)
{
	if (tab->fp >= tab->top) {
		sf->prev = 0;
		sf->bp = 0;
	} else {
		uint8_t * dst;
		uint8_t * src;
		int i;

		src = (uint8_t *)&tab->buf + tab->fp;
		dst = (uint8_t *)sf;
		for(i = 0; i < sizeof(sf); ++i)
			dst[i] = src[i];
	}

	return true;
}


/* --------------------------------------------------------------------------
   Objects
   -------------------------------------------------------------------------- */

struct sym_obj * sym_obj_new(struct symtab * tab, 
							 const char * nm, unsigned int nm_len)
{
	struct sym_obj * obj;
	struct sym_obj * next;
	int obj_len;

#if 0
	if ((obj = sym_obj_lookup(tab, s, len)) != NULL) {
		/* reuse existing name */
		nm = obj->nm;
		DCC_LOG1(LOG_INFO, "reusing name: \"%s\"", (char *)&tab->buf + nm); 
	} else {
		if (!sym_push_str(tab, s, len)) {
			DCC_LOG(LOG_WARNING, "sym_push_str() failed!");
			return NULL;
		}
		nm = tab->sp;
	}
#endif
	obj = (struct sym_obj *)((void *)tab->buf + tab->bp);
	obj_len = sizeof(struct sym_obj) + nm_len + 1;
	if ((tab->bp + obj_len + 2) > tab->sp) {
		DCC_LOG(LOG_WARNING, "bp overflow!");
		return NULL;
	}

	strncpy(obj->nm, nm, nm_len);
	obj->nm[nm_len] = '\0';
	obj->flags = (tab->fp == tab->top) ? SYM_OBJ_GLOBAL : 0;
	obj->addr = 0;
	obj->size = 0;
	obj->next = obj_len;
	
	/* insert the end of list dummy */
	next = (struct sym_obj *)((void *)obj + obj_len);
	next->prev = obj_len;
	next->next = 0;

	tab->bp += obj_len;

	return obj;
}

struct sym_obj * sym_obj_lookup(struct symtab * tab, 
								const char * s, unsigned int len)
{
	struct sym_obj * obj = (struct sym_obj *)((void *)tab->buf + tab->bp); 

	/* search from the inner scope out */
	while (obj->prev != 0) { 
		obj = (struct sym_obj *)((void *)obj - obj->prev);
		if ((strncmp(obj->nm, s, len) == 0) && (obj->nm[len] == '\0'))
			return obj;
	}

	return NULL;
}

struct sym_obj * sym_obj_scope_lookup(struct symtab * tab, 
									  const char * s, unsigned int len)
{
	struct sym_obj * obj;
	struct sym_sf sf;

	sym_sf_get(tab, &sf);
	obj = (struct sym_obj *)((void *)tab->buf + sf.bp); 

	/* search in the current scope only */
	while (obj->next != 0) { 
		if ((strncmp(obj->nm, s, len) == 0) && (obj->nm[len] == '\0'))
			return obj;
		obj = (struct sym_obj *)((void *)obj + obj->next);
	}

	return NULL;
}

int symtab_dump(FILE * f, struct symtab * tab)
{
	struct sym_obj * obj = (void *)tab->buf;

	while (obj->next != 0) { 
		fprintf(f, "%04x %c O .data   %04x    %s\n", obj->addr, 
				(obj->flags & SYM_OBJ_GLOBAL) ? 'g' : 'l',
				obj->size, obj->nm);
		obj = (struct sym_obj *)((void *)obj + obj->next);
	}

	return 0;
}

#if 0
int symtab_dump(FILE * f, struct symtab * tab)
{
	struct sym_obj * obj = (struct sym_obj *)((void *)tab->buf + tab->bp); 

	fprintf(f, "----------------------\n");
				
	/* search from the inner scope out */
	while (obj->prev != 0) { 
		obj = (struct sym_obj *)((void *)obj - obj->prev);
		fprintf(f, "%04x %c O .data   %04x    %s\n", obj->addr, 
				(obj->flags & SYM_OBJ_GLOBAL) ? 'g' : 'l',
				obj->size, obj->nm);
	}

	return 0;
}
#endif

/* return the total data size of the objects in the table */
int symtab_data_size(struct symtab * tab)
{
	struct sym_obj * obj = (void *)tab->buf;
	int size = 0;

	while (obj->next != 0) { 
		size += obj->size;
		obj = (struct sym_obj *)((void *)obj + obj->next);
	}

	return size;
}

/* --------------------------------------------------------------------------
   Externals (Library)
   -------------------------------------------------------------------------- */

int lib_lookup(const struct ext_libdef * libdef, 
			   const char * s, unsigned int len)
{
	int i;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	/* look in the externals first */
	for (i = 0; i < libdef->xcnt; ++i) {
		const struct extdef * fn = &libdef->xdef[i];
		if ((strncmp(fn->nm, s, len) == 0) && (fn->nm[len] == '\0')) {
			DCC_LOG2(LOG_INFO, "xid=%d nm=\"%s\"", i, fn->nm);
			return i;
		}
	}

	DCC_LOG(LOG_WARNING, "external symbol not found!");
	return -1;
}

int lib_member_lookup(const struct ext_libdef * libdef, 
					  unsigned int cid, const char * s, unsigned int len)
{
	struct classdef * cdef;
	int i;

	cdef = lib_classdef_get(libdef, cid);

	/* look in the class members list */
	for (i = cdef->first; i <= cdef->last; ++i) {
		const struct extdef * fn = &libdef->xdef[i];
		if ((strncmp(fn->nm, s, len) == 0) && (fn->nm[len] == '\0')) {
			DCC_LOG2(LOG_INFO, "xid=%d nm=\"%s\"", i, fn->nm);
			return i;
		}
	}

	DCC_LOG(LOG_WARNING, "class memeber not found!");
	return -ERR_EXTERN_NOT_MEMBER;

}

#if MICROJS_DEBUG_ENABLED
#endif

#if 0

static bool sym_poke(struct symtab * tab, int pos, 
					 void * ptr,  unsigned int len)
{
	int sp = tab->sp + pos * len;
	uint8_t * dst;
	uint8_t * src;
	int i;

	if (sp >= tab->fp)
		return false;

	dst = (uint8_t *)&tab->buf + sp;
	src = (uint8_t *)ptr;
	for(i = 0; i < len; ++i)
		dst[i] = src[i];

	return true;
}
#endif
