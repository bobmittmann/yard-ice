/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
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
 * @file module.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <string.h>
#include "module.h"
#include "dbglog.h"

/* Maximum number of modules in the global poll */
#ifndef MODULE_MAX
#define MODULE_MAX 8
#endif

struct module_rec module_tab[MODULE_MAX];

struct module_node {
	union {
		struct module_rec rec;
		struct {
			struct module_node * next;
			int id;
		} free;
	};
};

struct module_node * module_free;

struct module_rec * get_module_rec(int mod_id)
{
	return &module_tab[mod_id];
}

char * module_name(int mod_id) 
{
	return module_tab[mod_id].def->name;
}

char * module_ptr(int mod_id) 
{
	return module_tab[mod_id].ptr;
}

int module_register(const struct module_def * def, void * ptr)
{
	struct module_node * q;
	int id;

	if (def == NULL) {
		return -1;
	}

	if ((q = module_free) == NULL) {
		DCC_LOG(LOG_ERROR, "module table is full!");
		return -1;
	}

	id = q->free.id;

	DCC_LOG2(LOG_INFO, "module='%s' id=%d", def->name, id);

	if (def->init != NULL) {
		int ret;
		if ((ret = def->init(ptr, id)) < 0) {
			DCC_LOG(LOG_ERROR, "module initialization failed!");
			return ret;
		}
	}

	module_free = q->free.next;
	q->rec.def = (struct module_def *)def;
	q->rec.ptr = ptr;

	return id;
}

int module_unregister(int id) 
{
	struct module_node * p;
	struct module_node * q;
	struct module_def * def;

	if ((id < 0) || (id >= MODULE_MAX)) {
		DCC_LOG1(LOG_WARNING, "invalid module id=%d", id);
		return -1;
	}

	q = (struct module_node *)&module_tab[id];

	def = q->rec.def;

	DCC_LOG1(LOG_INFO, "module='%s'", def->name);

	if (def->done!= NULL) {
		def->done(q->rec.ptr, id);
	}

	if ((p = module_free) != NULL) {
		while (p->free.next != NULL) {
			p = p->free.next;
		}
		p->free.next = q;
	} else {
		module_free = q;
	}

	q->free.next = NULL;
	q->free.id = id;

	return 0;
}


int modules_init(void)
{
	struct module_node * p;
	struct module_node * q;
	int id;

	q = (struct module_node *)(void *)module_tab;

	module_free = q;
	for (id = 0; id < (MODULE_MAX - 1); id++) {
		p = q++;
		p->free.id = id;
		p->free.next = q;
	}
	q->free.next = NULL;
	q->free.id = id;

	return 0;
}

#if 0
void __attribute__ ((constructor)) modules_constructor(void)
{
	modules_init();
}
#endif
