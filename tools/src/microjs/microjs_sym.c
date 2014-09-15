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

int sym_name_lookup(struct symtab * tab, const char * s, unsigned int len)
{
	const struct ext_entry * ep;
	struct sym_nm * nm;
	int offs;
	int i = 0;

	/* look in the externals first */
	for (ep = tab->extrn; ep->nm != NULL; ++ep) {
		if ((strncmp(ep->nm, s, len) == 0) && (ep->nm[len] == '\0'))
			return i + 0x80;
		i++;
	}

	for (offs = tab->name; offs != 0; offs = nm->next) {
		nm = (struct sym_nm *)&tab->buf[offs];
		if ((strncmp(nm->s, s, len) == 0) && (nm->s[len] == '\0'))
			return nm->id;
	}

	return -ERR_STRING_NOT_FOUND;
}

/* Insert a name into the stack */
struct sym_nm * sym_name_push(struct symtab * tab, const char * s, 
							  unsigned int len)
{
	struct sym_nm * nm;
	int sp = tab->sp;

	nm = (struct sym_nm *)&tab->buf[sp];
	sp -= len + sizeof(struct sym_nm);
	if (tab->global > sp)
		return NULL;

	nm = (struct sym_nm *)&tab->buf[sp];
	nm->next = tab->name;
	nm->id = tab->id++;
	memcpy(nm->s, s, len);
	/* update stack */
	tab->sp = sp;
	/* update name list */
	tab->name = sp;
	return nm;
}

/* Push the stack frame */
bool sym_fp_push(struct symtab * tab)
{
	int sp = tab->sp;

	sp -= sizeof(uint16_t);
	if (tab->global > sp)
		return false;

	*(uint16_t *)&tab->buf[sp] = tab->fp;
	tab->sp = sp;
	tab->fp = sp;

	return true;
}

/* Pop the stack frame */
void sym_fp_pop(struct symtab * tab)
{
	tab->sp = tab->fp;
	tab->fp = *(uint16_t *)&tab->buf[tab->sp];
}

#if 0
int sym_by_name(struct symtab * tab, const char * s, unsigned int len)
{
	int nm;
	int i;

	if ((nm = str_lookup(s, len)) < 0)
		return nm;

	/* search in the local list */
	for (i = tab->local; i < tab->top; ++i) {
		if (tab->sym[i].nm == nm)
			return i;
	}

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		if ((tab->sym[i].flags & SYM_OBJECT) && (tab->sym[i].nm == nm))
			return i;
	}

	return -1;
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

struct sym_obj * sym_obj_lookup(struct symtab * tab, int nm)
{
#if 0
	int i;

	/* search in the local list */
	for (i = tab->local; i < tab->top; ++i) {
		if ((tab->sym[i].flags & SYM_OBJECT) && (tab->sym[i].nm == nm))
			return (struct sym_obj *)&tab->sym[i];
	}

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		if ((tab->sym[i].flags & SYM_OBJECT) && (tab->sym[i].nm == nm))
			return (struct sym_obj *)&tab->sym[i];
	}
#endif
	return NULL;
}

struct sym_obj * sym_obj_new(struct symtab * tab, 
							 const char * s, unsigned int len)
{
	struct sym_obj * obj;
	int offs;
	int nm;
//	int id;

//	if ((id = sym_by_name(tab, s, len)) >= 0)
//		return NULL;

	offs = tab->global;
	if ((offs + sizeof(struct sym_obj)) >  tab->sp)
		return NULL;

	if ((nm = str_add(s, len)) < 0)
		return NULL;

	obj = (struct sym_obj *)&tab->buf[offs];
	obj->nm = nm;
	obj->flags = SYM_OBJECT;
	obj->addr = 0;
	obj->size = 0;
	tab->global = offs + sizeof(struct sym_obj);

	return obj;
}

struct sym_ref * sym_ref_new(struct symtab * tab, void * sym)
{
	int oid = (struct sym *)sym - (struct sym *)(tab->buf);
	struct sym_ref * ref;
	int offs;

	offs = tab->global;
	if ((offs + sizeof(struct sym_obj)) >  tab->sp)
		return NULL;

	ref = (struct sym_ref *)&tab->buf[offs];
	ref->oid = oid;
	ref->flags = 0;
	ref->addr = 0;
	tab->global = offs + sizeof(struct sym_obj);

	return ref;
}

struct sym_ref * sym_ref_push(struct symtab * tab) 
{
	struct sym_ref * ref;
	int sp = tab->sp;

	ref = (struct sym_ref *)&tab->buf[sp];
	sp -= sizeof(struct sym_ref);
	if (tab->global > sp)
		return NULL;

	ref = (struct sym_ref *)&tab->buf[sp];
	ref->oid = (struct sym *)ref - (struct sym *)tab->buf;
	ref->flags = 0;
	ref->addr = 0;
	/* update name list */
	tab->name = sp;

	return ref;
}

void sym_ref_pop(struct symtab * tab)
{
	tab->sp += sizeof(struct sym_ref);
}

struct sym_ref * sym_ref_get(struct symtab * tab, int pos)
{
	struct sym_ref * ref;

	/* references are indexed by the frame pointer */
	ref = (struct sym_ref *)&tab->buf[tab->fp];

	return &ref[pos];
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

struct sym_tmp * sym_tmp_push(struct symtab * tab, 
							  const char * s, unsigned int len)
{
	struct sym_tmp * tmp;
	int sp = tab->sp;
	int nm;

	tmp = (struct sym_tmp *)&tab->buf[sp];
	sp -= sizeof(struct sym_tmp);
	if (tab->global > sp)
		return NULL;

	if ((nm = str_add(s, len)) < 0)
		return NULL;

	tmp = (struct sym_tmp *)&tab->buf[sp];
	tmp->flags = 0;
	tmp->nm = nm;
	tmp->xid= 0;
	tmp->cnt = 0;
	tmp->min= 0;
	tmp->max= 0;

	tab->name = sp;

	return tmp;
}

void sym_tmp_pop(struct symtab * tab)
{
	tab->sp += sizeof(struct sym_tmp);
}

struct sym_tmp * sym_tmp_get(struct symtab * tab, int pos)
{
	struct sym_tmp * tmp;

	tmp = (struct sym_tmp *)&tab->buf[tab->sp];

	return &tmp[pos];
}

const char * sym_name(struct symtab * tab, int nm)
{
	return str(nm);
}

