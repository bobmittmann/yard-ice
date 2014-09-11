/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	dcclog.c
 * Module:
 * Project:	ARM-DCC logger expander
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(C) 2005 Robinson Mittmann. All Rights Reserved.
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
 * 
 */

#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "calc.h"

#define DEBUG 0
#include "debug.h"

#define __DEF_CONST_STR__
#include "const_str.h"

#define CONST_STRINGS_MAX 64
#define CONST_NM (256 - CONST_STRINGS_MAX)

/* --------------------------------------------------------------------------
   Simple Strings Table
   -------------------------------------------------------------------------- */

static void str_pool_init(struct str_pool * pool)
{
	/* Add null string */
	pool->buf[0] = '\0';
	pool->offs[0] = 0;
	pool->cnt = 0;
}

int const_str_lookup(const char * s, unsigned int len)
{
	int i;

	/* look in the constant pool first */
	for (i = 0; i < const_str_pool.cnt; ++i) {
		char * cstr = const_str_pool.buf + const_str_pool.offs[i];
		if ((strncmp(cstr, s, len) == 0) && (cstr[len] == '\0'))
			return i + CONST_NM;
	}

	return -1;
}

int str_lookup(struct str_pool * pool, const char * s, unsigned int len)
{
	char *buf = (char *)pool->buf;
	int i;

	/* look in the constant pool first */
	for (i = 0; i < const_str_pool.cnt; ++i) {
		char * cstr = const_str_pool.buf + const_str_pool.offs[i];
		if ((strncmp(cstr, s, len) == 0) && (cstr[len] == '\0'))
			return i + CONST_NM;
	}

	for (i = 0; i < pool->cnt; ++i) {
		char * cstr = buf + pool->offs[i];
		if ((strncmp(cstr, s, len) == 0) && (cstr[len] == '\0'))
			return i;
	}

	return -1;
}

int str_add(struct str_pool * pool, const char * s, unsigned int len)
{
	char * dst;
	int idx;
	int offs;

	if ((idx = str_lookup(pool, s, len)) >= 0)
		return idx;

	offs = (pool->cnt > 0) ? pool->offs[pool->cnt - 1] : 0;
	offs += strlen(pool->buf + offs) + 1;

	if ((idx = pool->cnt) > STRINGS_MAX) {
		fprintf(stderr, "Too many strings.\n");
		return -1;
	}

	if ((offs + len + 1) >= STRINGS_POOL_SIZE) {
		fprintf(stderr, "Strings buffer overflow.\n");
		return -1;
	}

	/* Copy the string to the buffer */
	dst = pool->buf + offs;
	memcpy(dst, s, len);
	/* NULL terminate the string */
	dst[len] = '\0';
	pool->offs[idx] = dst - pool->buf;
	pool->cnt++;

	return idx;
}

const char * string(struct str_pool * str, int idx)
{
	return (idx >= CONST_NM) ? const_str_pool.buf + 
		const_str_pool.offs[idx - CONST_NM] : str->buf + str->offs[idx];
}

/* --------------------------------------------------------------------------
   External symbols
   -------------------------------------------------------------------------- */

const struct ext_entry externals[] = {
	[EXT_RAND] = { .nm = CONST_NM + NM_RAND, .argmin = 0, .argmax = 0 },
	[EXT_SQRT] = { .nm = CONST_NM + NM_SQRT, .argmin = 1, .argmax = 1 },
	[EXT_LOG2] = { .nm = CONST_NM + NM_LOG2, .argmin = 1, .argmax = 1 },
	[EXT_WRITE] = { .nm = CONST_NM + NM_WRITE, .argmin = 0, .argmax = 128 },
	[EXT_PRINT] = { .nm = CONST_NM + NM_PRINT, .argmin = 0, .argmax = 128 },
	[EXT_SRAND] = { .nm = CONST_NM + NM_SRAND, .argmin = 1, .argmax = 1 },
	[EXT_TIME] = { .nm = CONST_NM + NM_TIME, .argmin = 0, .argmax = 0 },
};

int extern_lookup(int nm)
{
	int i;

	for (i = 0; i < sizeof(externals) / sizeof(struct ext_entry); ++i) {
		if (externals[i].nm == nm)
			return i;
	}

	return -1;
}

struct ext_entry * extern_get(unsigned int exid)
{
	if (exid >= sizeof(externals) / sizeof(struct ext_entry))
		return NULL;
	return(struct ext_entry *)&externals[exid];
}

/* --------------------------------------------------------------------------
   Symbol table
   -------------------------------------------------------------------------- */

void sym_tab_init(struct sym_tab * tab)
{
	tab->str.buf = tab->str_buf;
	str_pool_init(&tab->str);
	tab->global = 0;
	tab->local = SYMBOLS_MAX;
}

int sym_by_name(struct sym_tab * tab, const char * s, unsigned int len)
{
	int nm;
	int i;

	if ((nm = str_lookup(&tab->str, s, len)) < 0) {
		WARN("str_lookup() failed!");
		return nm;
	}

	/* search in the local list */
	for (i = tab->local; i < SYMBOLS_MAX; ++i) {
		if (tab->sym[i].nm == nm)
			return i;
	}

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		if ((tab->sym[i].flags & SYM_OBJECT) && (tab->sym[i].nm == nm))
			return i;
	}

	WARN("nm=%d \"%s\" not found!", nm, string(&tab->str, nm));

	return -1;
}

int sym_dump(FILE * f, struct sym_tab * tab)
{
	int i;

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		struct sym * sp = &tab->sym[i];
		if (sp->flags & SYM_OBJECT) {
			struct sym_obj * obj = (struct sym_obj *)sp;
			fprintf(f, "%04x g O .data   %04x    %s\n", obj->addr,
					obj->size, string(&tab->str, obj->nm));
		} else if (sp->flags & SYM_EXTERN) {
			struct sym_ext * ext = (struct sym_ext *)sp;
			fprintf(f, "%04x g F .extern %04x    %s\n", ext->addr,
					0, string(&tab->str, externals[ext->addr].nm));
		} else {
			struct sym_ref * ref = (struct sym_ref *)sp;
			struct sym_obj * obj = (struct sym_obj *)&tab->sym[ref->oid];
			fprintf(f, "%04x r   .text   %04x -> %s\n", ref->addr, 
					0, string(&tab->str, obj->nm));
		}
	}

	return 0;
}

int sym_lookup(struct sym_tab * tab, const char * s, unsigned int len)
{
	return sym_by_name(tab, s, len);
}

struct sym_obj * sym_obj_lookup(struct sym_tab * tab, int nm)
{
	int i;

	/* search in the local list */
	for (i = tab->local; i < SYMBOLS_MAX; ++i) {
		if ((tab->sym[i].flags & SYM_OBJECT) && (tab->sym[i].nm == nm))
			return (struct sym_obj *)&tab->sym[i];
	}

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		if ((tab->sym[i].flags & SYM_OBJECT) && (tab->sym[i].nm == nm))
			return (struct sym_obj *)&tab->sym[i];
	}

	WARN("nm=%d \"%s\" not found!", nm, string(&tab->str, nm));

	return NULL;
}

struct sym_obj * sym_obj_new(struct sym_tab * tab, 
							 const char * s, unsigned int len)
{
	struct sym_obj * obj;
	int nm;
	int id;

	if ((id = sym_by_name(tab, s, len)) >= 0)
		return NULL;

	if (tab->global == tab->local)
		return NULL;

	if ((nm = str_add(&tab->str, s, len)) < 0)
		return NULL;

	obj = (struct sym_obj *)&tab->sym[tab->global++];
	obj->nm = nm;
	obj->flags = SYM_OBJECT;
	obj->addr = 0;
	obj->size = 0;

	return obj;
}

struct sym_ref * sym_ref_new(struct sym_tab * tab, void * sym)
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

struct sym_ext * sym_ext_new(struct sym_tab * tab, int nm)
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

int sym_ext_id(struct sym_tab * tab, struct sym_ext * ext)
{
	return (struct sym *)ext - tab->sym;
}

int sym_add_local(struct sym_tab * tab, const char * s, unsigned int len)
{
	struct sym * sp;
	int nm;
	int id;

	if (tab->global == tab->local)
		return -1;

	if ((id = sym_by_name(tab, s, len)) >= 0)
		return id;

	if ((nm = str_add(&tab->str, s, len)) < 0)
		return nm;

	id = --tab->local;
	sp = &tab->sym[id];

	sp->nm = nm;
	sp->flags = 0;
	sp->addr = 0;

	return id;
}

int sym_anom_push(struct sym_tab * tab)
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

int sym_anom_pop(struct sym_tab * tab)
{
	if (SYMBOLS_MAX == tab->local)
		return -1;

	return tab->local++;
}

int sym_anom_get(struct sym_tab * tab, int pos)
{
	if (SYMBOLS_MAX <= (tab->local + pos))
		return -1;

	return tab->local + pos;
}

void sym_pop(struct sym_tab * tab)
{
	if (SYMBOLS_MAX > tab->local)
		tab->local++;
}

struct sym_tmp * sym_tmp_push(struct sym_tab * tab, 
							  const char * s, unsigned int len)
{
	struct sym_tmp * tmp;
	int nm;

	if (tab->global == tab->local)
		return NULL;

	if ((nm = str_add(&tab->str, s, len)) < 0)
		return NULL;

	tmp = (struct sym_tmp *)&tab->sym[--tab->local];
	tmp->nm = nm;
	tmp->flags = 0;

	return tmp;
}

struct sym_tmp * sym_tmp_get(struct sym_tab * tab, int pos)
{
	if (SYMBOLS_MAX <= (tab->local + pos))
		return NULL;

	return (struct sym_tmp *)&tab->sym[tab->local + pos];
}

const char * sym_name(struct sym_tab * tab, int nm)
{
	return string(&tab->str, nm);
}

/*
const char * sym_name(struct sym_tab * tab, int id)
{
	return string(&tab->str, tab->sym[id].nm);
}
*/

int sym_addr_get(struct sym_tab * tab, int id)
{
	return tab->sym[id].addr;
}

void sym_addr_set(struct sym_tab * tab, int id, int addr)
{
	tab->sym[id].addr = addr;
}

