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
	tab->local = tab->top;
	/* locals are allocated bottom-up */
	tab->global = 0;
	return tab;
}

int sym_by_name(struct symtab * tab, const char * s, unsigned int len)
{
	int nm;
	int i;

	if ((nm = str_lookup(s, len)) < 0) {
		return nm;
	}

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

int sym_dump(FILE * f, struct symtab * tab)
{
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
					0, str(extern_get(ext->addr)->nm));
		} else {
			struct sym_ref * ref = (struct sym_ref *)sp;
			struct sym_obj * obj = (struct sym_obj *)&tab->sym[ref->oid];
			fprintf(f, "%04x r   .text   %04x -> %s\n", ref->addr, 
					0, str(obj->nm));

		}
	}

	return 0;
}

int sym_lookup(struct symtab * tab, const char * s, unsigned int len)
{
	return sym_by_name(tab, s, len);
}

struct sym_obj * sym_obj_lookup(struct symtab * tab, int nm)
{
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

	return NULL;
}

struct sym_obj * sym_obj_new(struct symtab * tab, 
							 const char * s, unsigned int len)
{
	struct sym_obj * obj;
	int nm;
	int id;

	if ((id = sym_by_name(tab, s, len)) >= 0)
		return NULL;

	if (tab->global == tab->local)
		return NULL;

	if ((nm = str_add(s, len)) < 0)
		return NULL;

	obj = (struct sym_obj *)&tab->sym[tab->global++];
	obj->nm = nm;
	obj->flags = SYM_OBJECT;
	obj->addr = 0;
	obj->size = 0;

	return obj;
}

struct sym_ref * sym_ref_new(struct symtab * tab, void * sym)
{
	int oid = (struct sym *)sym - tab->sym;
	struct sym_ref * ref;

	if (tab->global == tab->local)
		return NULL;

	ref = (struct sym_ref *)&tab->sym[tab->global++];
	ref->oid = oid;
	ref->flags = 0;
	ref->addr = 0;

	return ref;
}

struct sym_ext * sym_ext_new(struct symtab * tab, int nm)
{
	struct sym_ext * ext;
	int exid;

	if (tab->global == tab->local)
		return NULL;

	if ((exid = extern_lookup(nm)) < 0)
		return NULL;

	ext = (struct sym_ext *)&tab->sym[tab->global++];
	ext->addr = exid;
	ext->flags = SYM_EXTERN;

	return ext;
}

int sym_ext_id(struct symtab * tab, struct sym_ext * ext)
{
	return (struct sym *)ext - tab->sym;
}

int sym_add_local(struct symtab * tab, const char * s, unsigned int len)
{
	struct sym * sp;
	int nm;
	int id;

	if (tab->global == tab->local)
		return -1;

	if ((id = sym_by_name(tab, s, len)) >= 0)
		return id;

	if ((nm = str_add(s, len)) < 0)
		return nm;

	id = --tab->local;
	sp = &tab->sym[id];

	sp->nm = nm;
	sp->flags = 0;
	sp->addr = 0;

	return id;
}

int sym_anom_push(struct symtab * tab)
{
	struct sym * sp;
	int id;

	if (tab->global == tab->local)
		return -1;

	id = --tab->local;
	sp = &tab->sym[id];

	sp->nm = 0;
	sp->flags = 0;
	sp->addr = 0;

	return id;
}

int sym_anom_pop(struct symtab * tab)
{
	if (tab->top == tab->local)
		return -1;

	return tab->local++;
}

int sym_anom_get(struct symtab * tab, int pos)
{
	if (tab->top <= (tab->local + pos))
		return -1;

	return tab->local + pos;
}

void sym_pop(struct symtab * tab)
{
	if (tab->top > tab->local)
		tab->local++;
}

struct sym_tmp * sym_tmp_push(struct symtab * tab, 
							  const char * s, unsigned int len)
{
	struct sym_tmp * tmp;
	int nm;

	if (tab->global == tab->local)
		return NULL;

	if ((nm = str_add(s, len)) < 0)
		return NULL;

	tmp = (struct sym_tmp *)&tab->sym[--tab->local];
	tmp->nm = nm;
	tmp->flags = 0;

	return tmp;
}

struct sym_tmp * sym_tmp_get(struct symtab * tab, int pos)
{
	if (tab->top <= (tab->local + pos))
		return NULL;

	return (struct sym_tmp *)&tab->sym[tab->local + pos];
}

const char * sym_name(struct symtab * tab, int nm)
{
	return str(nm);
}

/*
const char * sym_name(struct symtab * tab, int id)
{
	return str(&tab->str, tab->sym[id].nm);
}
*/

int sym_addr_get(struct symtab * tab, int id)
{
	return tab->sym[id].addr;
}

void sym_addr_set(struct symtab * tab, int id, int addr)
{
	tab->sym[id].addr = addr;
}

