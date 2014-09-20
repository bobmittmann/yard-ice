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
	tab->bp = 0;
	/* initialize temporary labels */
	tab->tmp_lbl = 0;

	tab->libdef = libdef;

	DCC_LOG3(LOG_TRACE, "bp=%d sp=%d fp=%d", tab->bp, tab->sp, tab->fp);

	return tab;
}

struct symtab * symtab_open(uint32_t * buf, unsigned int len)
{
	struct symtab * tab = (struct symtab *)buf;

	DCC_LOG4(LOG_TRACE, "bp=%d sp=%d fp=%d top=%d", 
			 tab->bp, tab->sp, tab->fp, tab->top);

	if (tab->fp > tab->top)
		DCC_LOG(LOG_ERROR, "fp > top !");

	if (tab->sp > tab->fp)
		DCC_LOG(LOG_ERROR, "sp > fp !");

	if (tab->sp < tab->bp)
		DCC_LOG(LOG_ERROR, "sp <= pp !");

	/* remove all stach frames except the global one */
	while (tab->fp != tab->top) {
		sym_sf_pop(tab);
		DCC_LOG2(LOG_TRACE, "sp=%d fp=%d", tab->sp, tab->fp);
	}

	/* reset temporary labels */
	tab->tmp_lbl = 0;

	return tab;
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
	if (tab->bp > sp)
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
	struct sym_sf sf;
	int sp = tab->fp; /* use frame pointer as reference */
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
							 const char * s, unsigned int len)
{
	struct sym_obj * obj;
	int bp = tab->bp;
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

	if ((bp + sizeof(struct sym_obj)) > tab->sp) {
		DCC_LOG(LOG_WARNING, "bp overflow!");
		return NULL;
	}

	id = bp / sizeof(struct sym_obj);

	DCC_LOG2(LOG_INFO, "bp=%d id=%d", bp, id);
	DCC_LOG1(LOG_INFO, "nm=\"%s\"", (char *)&tab->buf + tab->sp); 

	obj = &tab->buf[id];
	obj->nm = nm;
	obj->flags = (tab->fp == tab->top) ? SYM_OBJ_GLOBAL : 0;
	obj->addr = 0;
	obj->size = 0;
	tab->bp = bp + sizeof(struct sym_obj);

	return obj;
}

struct sym_obj * sym_obj_lookup(struct symtab * tab, 
								const char * s, unsigned int len)
{
	struct sym_obj * obj;
	char * nm;
	int i;

	/* search in the local list */
	for (i = (tab->bp / sizeof(struct sym_obj)) - 1; i >= 0; --i) {
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

struct sym_obj * sym_obj_scope_lookup(struct symtab * tab, 
									  const char * s, unsigned int len)
{
	struct sym_sf sf;
	struct sym_obj * obj;
	char * nm;
	int i;
	int begin;
	int end;

	sym_sf_get(tab, &sf);

	begin = sf.bp / sizeof(struct sym_obj);
	end = tab->bp / sizeof(struct sym_obj);

	/* search in the current scope */
	for (i = begin; i < end; ++i) {
		obj = &tab->buf[i];
		nm = (char *)&tab->buf + obj->nm;
		DCC_LOG3(LOG_INFO, "id=%d nm=\"%s\" len=%d", i, nm, len);

		if ((strncmp(nm, s, len) == 0) && (nm[len] == '\0')) {
			DCC_LOG2(LOG_INFO, "id=%d nm=\"%s\"", i, nm);
			return obj;
		}
	}

	return NULL;
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

#if MICROJS_DEBUG_ENABLED
int sym_dump(FILE * f, struct symtab * tab)
{
	struct sym_obj * obj;
	char * nm;
	int i;

	/* search in the local list */
	for (i = 0; i < tab->bp / sizeof(struct sym_obj); ++i) {
		obj = &tab->buf[i];
		nm = (char *)&tab->buf + obj->nm;
		fprintf(f, "%04x %c O .data   %04x    %s\n", obj->addr, 
				(obj->flags & SYM_OBJ_GLOBAL) ? 'g' : 'l',
				obj->size, nm);
	}

	fprintf(f, "FP=%04x, SP=%04x\n", tab->fp, tab->sp);

	return 0;
}
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
