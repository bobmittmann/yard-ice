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

struct symtab * symtab_init(uint32_t * buf, unsigned int len, 
							const struct ext_libdef * libdef)
{
	struct symtab * tab = (struct symtab *)buf;

	/* top of the symbol table */
	tab->top = len - sizeof(struct symtab);
	/* locals are allocated top-down */
	tab->sp = tab->top;
	tab->fp = tab->top;
	/* symbols are allocated bottom-up */
	tab->heap = 0;
	/* initialize temporary labels */
	tab->tmp_lbl = 0;

	tab->libdef = libdef;
	return tab;
}

static bool sym_push(struct symtab * tab, const void * ptr,  unsigned int len)
{
	int sp = tab->sp;
	uint8_t * dst;
	uint8_t * src;
	int i;
	
	DCC_LOG3(LOG_INFO, "heap=%d sp=%d len=%d", tab->heap, tab->sp, len);

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

static bool sym_push_str(struct symtab * tab, const char * s,  unsigned int len)
{
	int sp = tab->sp;
	uint8_t * dst;
	uint8_t * src;
	int i;
	
	DCC_LOG3(LOG_INFO, "heap=%d sp=%d len=%d", tab->heap, tab->sp, len + 1);

	sp -= len + 1;
	if (tab->heap > sp)
		return false;

	dst = (uint8_t *)&tab->buf + sp;
	src = (uint8_t *)s;
	for(i = 0; i < len; ++i)
		dst[i] = src[i];
	dst[i] = '\0';

	tab->sp = sp;

	return true;
}

static bool sym_pop(struct symtab * tab, void * ptr,  unsigned int len)
{
	int sp = tab->sp;
	uint8_t * dst;
	uint8_t * src;
	int i;

	DCC_LOG3(LOG_INFO, "heap=%d sp=%d len=%d", tab->heap, tab->sp, len);

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
	int heap = tab->heap;
	int id;
	int nm;

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

	if ((heap + sizeof(struct sym_obj)) > tab->sp) {
		DCC_LOG(LOG_WARNING, "heap overflow!");
		return NULL;
	}

	id = heap / sizeof(struct sym_obj);

	DCC_LOG2(LOG_INFO, "heap=%d id=%d", heap, id);
	DCC_LOG1(LOG_INFO, "nm=\"%s\"", (char *)&tab->buf + tab->sp); 

	obj = &tab->buf[id];
	obj->nm = nm;
	obj->flags = SYM_OBJECT;
	obj->addr = 0;
	obj->size = 0;
	tab->heap = heap + sizeof(struct sym_obj);

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
		obj = &tab->buf[i];
		nm = (char *)&tab->buf + obj->nm;

		DCC_LOG3(LOG_MSG, "id=%d nm=\"%s\" len=%d", i, nm, len);

		if ((strncmp(nm, s, len) == 0) && (nm[len] == '\0')) {
			DCC_LOG2(LOG_INFO, "id=%d nm=\"%s\"", i, nm);
			return obj;
		}
	}

	return NULL;
}

const char * sym_obj_name(struct symtab * tab, struct sym_obj * obj)
{
	return (char *)&tab->buf + obj->nm;
}

int sym_lbl_next(struct symtab * tab) 
{
	return tab->tmp_lbl++;
}

/* --------------------------------------------------------------------------
   References
   -------------------------------------------------------------------------- */

bool sym_ref_push(struct symtab * tab, struct sym_ref * ref) 
{
	return sym_push(tab, ref, sizeof(struct sym_ref));
}

bool sym_ref_pop(struct symtab * tab, struct sym_ref * ref)
{
	return sym_pop(tab, ref, sizeof(struct sym_ref));
}

/* --------------------------------------------------------------------------
   Loop descriptors
   -------------------------------------------------------------------------- */

bool sym_fld_push(struct symtab * tab, struct sym_fld * fld)
{
	return sym_push(tab, fld, sizeof(struct sym_fld));
}

bool sym_fld_pop(struct symtab * tab, struct sym_fld * fld)
{
	return sym_pop(tab, fld, sizeof(struct sym_fld));
}

/* --------------------------------------------------------------------------
   While Loop Descriptor
   -------------------------------------------------------------------------- */

bool sym_wld_push(struct symtab * tab, struct sym_wld * wld)
{
	return sym_push(tab, wld, sizeof(struct sym_wld));
}

bool sym_wld_pop(struct symtab * tab, struct sym_wld * wld)
{
	return sym_pop(tab, wld, sizeof(struct sym_wld));
}

/* --------------------------------------------------------------------------
   Function Descriptor
   -------------------------------------------------------------------------- */

bool sym_fnd_push(struct symtab * tab, struct sym_fnd * fnd)
{
	return sym_push(tab, fnd, sizeof(struct sym_fnd));
}

bool sym_fnd_pop(struct symtab * tab, struct sym_fnd * fnd)
{
	return sym_pop(tab, fnd, sizeof(struct sym_fnd));
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
	return sym_pop(tab, tmp, sizeof(struct sym_tmp));
}

/* --------------------------------------------------------------------------
   Externals (Library)
   -------------------------------------------------------------------------- */

int sym_extern_lookup(struct symtab * tab, const char * s, unsigned int len)
{
	const struct ext_libdef * libdef = tab->libdef;
	int i;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	/* look in the externals first */
	for (i = 0; i < libdef->fncnt; ++i) {
		const struct ext_fndef * fn = &libdef->fndef[i];
		if ((strncmp(fn->nm, s, len) == 0) && (fn->nm[len] == '\0')) {
			DCC_LOG2(LOG_INFO, "xid=%d nm=\"%s\"", i, fn->nm);
			return i;
		}
	}

	DCC_LOG(LOG_WARNING, "external symbol not found!");
	return -1;
}

struct ext_fndef * sym_extern_get(struct symtab * tab, unsigned int xid)
{
	const struct ext_libdef * libdef = tab->libdef;

	return (struct ext_fndef *)&libdef->fndef[xid];
}

const char * sym_extern_name(struct symtab * tab, unsigned int xid)
{
	const struct ext_libdef * libdef = tab->libdef;

	return libdef->fndef[xid].nm;
}

int sym_dump(FILE * f, struct symtab * tab)
{
	struct sym_obj * obj;
	char * nm;
	int i;

	/* search in the local list */
	for (i = 0; i < tab->heap / sizeof(struct sym_obj); ++i) {
		obj = &tab->buf[i];
		nm = (char *)&tab->buf + obj->nm;
		fprintf(f, "%04x g O .data   %04x    %s\n", obj->addr, obj->size, nm);
	}

	return 0;
}


#if 0
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
#endif