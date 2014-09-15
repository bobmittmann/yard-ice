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

/* --------------------------------------------------------------------------
   Symbol table
   -------------------------------------------------------------------------- */

struct symtab * symtab_init(uint32_t * buf, unsigned int len)
{
	struct symtab * tab = (struct symtab *)buf;

	/* top of the symbol table */
	tab->top = (len - sizeof(struct symtab)) / sizeof(struct symtab);
	/* locals are allocated top-down */
	tab->sp = tab->top;
	tab->fp = tab->top;
	tab->name = 0;
	/* locals are allocated bottom-up */
	tab->global = 0;
	return tab;
}

bool sym_push(struct symtab * tab, const void * ptr,  unsigned int len)
{
	int sp = tab->sp;
	uint8_t * dst;
	uint8_t * src;
	int i;

	sp -= len;
	if (tab->heap > sp)
		return false;

	dst = (uint8_t *)&tab->buf + sp;
	src = (uint8_t *)ptr;
	for(i = 0; i < len; ++i)
		dst[i] = src[i];

	tab->sp = sp;
	return true;
}

static bool sym_pop(struct symtab * tab, void * ptr,  unsigned int len)
{
	int sp = tab->sp;
	uint8_t * dst;
	uint8_t * src;
	int i;

	if (sp >= tab->fp)
		return false;

	if (ptr != NULL) {
		src = (uint8_t *)&tab->buf + sp;
		dst = (uint8_t *)ptr;
		for(i = 0; i < len; ++i)
			dst[i] = src[i];
	}

	tab->sp = sp + len;

	return true;
}

static bool sym_pick(struct symtab * tab, int pos, 
					 void * ptr,  unsigned int len)
{
	int sp = tab->sp + pos * len;
	uint8_t * dst;
	uint8_t * src;
	int i;

	if (sp >= tab->fp)
		return false;

	src = (uint8_t *)&tab->buf + sp;
	dst = (uint8_t *)ptr;
	for(i = 0; i < len; ++i)
		dst[i] = src[i];

	return true;
}

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

struct sym_scope {
	uint16_t prev;
	uint16_t heap;
};

/* Push the stack frame */
bool sym_scope_open(struct symtab * tab)
{
	struct sym_scope scope;

	scope.prev = tab->fp;
	scope.heap = tab->heap;

	if (!sym_push(tab, &scope, sizeof(scope)))
		return false;

	tab->fp = tab->sp;

	return true;
}

/* Pop the stack frame */
bool sym_scope_close(struct symtab * tab)
{
	struct sym_scope scope;
	int sp = tab->fp; /* use frame pointer as reference */
	uint8_t * dst;
	uint8_t * src;
	int i;

	if (sp >= tab->top)
		return false;

	src = (uint8_t *)&tab->buf + sp;
	dst = (uint8_t *)&scope;
	for(i = 0; i < sizeof(scope); ++i)
		dst[i] = src[i];

	tab->sp = sp + sizeof(scope);
	tab->fp = scope.prev;
	tab->heap = scope.heap;

	return true;
}

/* --------------------------------------------------------------------------
   Objects
   -------------------------------------------------------------------------- */

struct sym_obj * sym_obj_new(struct symtab * tab, 
							 const char * s, unsigned int len)
{
	struct sym_obj * obj;

	if (!sym_push(tab, s, len))
		return NULL;

	obj = &tab->buf[tab->heap / sizeof(struct sym_obj)];
	obj->nm = tab->sp;
	obj->flags = SYM_OBJECT;
	obj->addr = 0;
	obj->size = 0;
	tab->heap += sizeof(struct sym_obj);

	return obj;
}

struct sym_obj * sym_obj_lookup(struct symtab * tab, 
								const char * s, unsigned int len)
{
	struct sym_obj * obj;
	char * nm;
	int i;

	/* search in the local list */
	for (i = (tab->heap / sizeof(struct sym_obj)) - 1; i >= 0; --i) {
		obj = &tab->buf[tab->heap / sizeof(struct sym_obj)];
		nm = (char *)&tab->buf + obj->nm;
		if ((strncmp(nm, s, len) == 0) && (nm[len] == '\0'))
			return obj;
	}

	return NULL;
}

const char * sym_obj_name(struct symtab * tab, struct sym_obj * obj)
{
	return (char *)&tab->buf + obj->nm;
}

/* --------------------------------------------------------------------------
   References
   -------------------------------------------------------------------------- */

bool sym_ref_push(struct symtab * tab, struct sym_ref * ref) 
{
	ref->lbl = tab->tmp_lbl++;

	return sym_push(tab, ref, sizeof(struct sym_ref));
}

bool sym_ref_pop(struct symtab * tab, struct sym_ref * ref)
{
	return sym_pop(tab, ref, sizeof(struct sym_ref));
}

bool sym_ref_get(struct symtab * tab, int pos, struct sym_ref * ref)
{
	return sym_pick(tab, pos, ref, sizeof(struct sym_ref));
}

bool sym_ref_set(struct symtab * tab, int pos, struct sym_ref * ref)
{
	return sym_poke(tab, pos, ref, sizeof(struct sym_ref));
}

/* --------------------------------------------------------------------------
   Temporary symbols
   -------------------------------------------------------------------------- */

bool sym_tmp_push(struct symtab * tab, struct sym_tmp * tmp) 
{
	return sym_push(tab, tmp, sizeof(struct sym_tmp));
}

bool sym_tmp_pop(struct symtab * tab, struct sym_tmp * tmp)
{
	return sym_pop(tab, tmp, sizeof(struct sym_ref));
}

bool sym_tmp_get(struct symtab * tab, int pos, struct sym_tmp * tmp)
{
	return sym_pick(tab, pos, tmp, sizeof(struct sym_tmp));
}

/* --------------------------------------------------------------------------
   Externals (Library)
   -------------------------------------------------------------------------- */

int sym_extern_lookup(struct symtab * tab, const char * s, unsigned int len)
{
	const struct ext_entry * ep = tab->extrn;
	int i;

	/* look in the externals first */
	for (ep = tab->extrn, i = 0; ep->nm != NULL; ++ep, ++i) {
		if ((strncmp(ep->nm, s, len) == 0) && (ep->nm[len] == '\0'))
			return i;
	}

	return -ERR_STRING_NOT_FOUND;
}

struct ext_entry * sym_extern_get(struct symtab * tab, unsigned int xid)
{
	return(struct ext_entry *)&tab->extrn[xid];
}

const char * sym_extern_name(struct symtab * tab, unsigned int xid)
{
	return tab->extrn[xid].nm;
}

#if 0

const char * sym_name(struct symtab * tab, int nm)
{
	return str(nm);
}

int sym_name_lookup(struct symtab * tab, const char * s, unsigned int len)
{
	const struct ext_entry * ep;
//	struct sym_nm * nm;
//	int offs;
	int i = 0;

	/* look in the externals first */
	for (ep = tab->extrn; ep->nm != NULL; ++ep) {
		if ((strncmp(ep->nm, s, len) == 0) && (ep->nm[len] == '\0'))
			return i + 0x80;
		i++;
	}

#if 0
	for (offs = tab->name; offs != 0; offs = nm->next) {
		nm = (struct sym_nm *)&tab->buf[offs];
		if ((strncmp(nm->s, s, len) == 0) && (nm->s[len] == '\0'))
			return nm->id;
	}
#endif

	return -ERR_STRING_NOT_FOUND;
}
#endif

int sym_dump(FILE * f, struct symtab * tab)
{
#if 0
	int i;

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		struct sym * sp = &tab->sym[i];
		if (sp->flags & SYM_OBJECT) {
			struct sym_obj * obj = (struct sym_obj *)sp;
			fprintf(f, "%04x g O .data   %04x    %s\n", obj->addr,
					obj->size, str(obj->nm));
		} else if (sp->flags & SYM_EXTERN) {
			struct sym_ext * ext = (struct sym_ext *)sp;
			fprintf(f, "%04x g F .extern %04x    %s\n", ext->addr,
					0, extern_get(ext->addr)->nm);
		} else {
			struct sym_ref * ref = (struct sym_ref *)sp;
			struct sym_obj * obj = (struct sym_obj *)&tab->sym[ref->oid];
			fprintf(f, "%04x r   .text   %04x -> %s\n", ref->addr, 
					0, str(obj->nm));
		}
	}
#endif
	return 0;
}

#if 0
/* Insert a name into the stack */
bool sym_name_push(struct symtab * tab, const char * s, 
							  unsigned int len)
{
	uint8_t n = len;

	if (!sym_push(tab, s, len))
		return false;

	return sym_push(tab, &n, len);
}


struct sym_ext * sym_ext_new(struct symtab * tab, int nm)
{
	struct sym_ext * ext;
	int offs;
	int exid;

	offs = tab->global;
	if ((offs + sizeof(struct sym_obj)) >  tab->sp)
		return NULL;

	if ((exid = extern_lookup(nm)) < 0)
		return NULL;

	ext = (struct sym_ext *)&tab->buf[offs];
	ext->addr = exid;
	ext->flags = SYM_EXTERN;
	tab->global = offs + sizeof(struct sym_obj);

	return ext;
}

int sym_ext_id(struct symtab * tab, struct sym_ext * ext)
{
	return (struct sym *)ext - (struct sym *)tab->buf;
}
#endif
