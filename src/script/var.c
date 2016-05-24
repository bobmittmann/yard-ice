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
 * @file var.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#include <stdlib.h>
#include <string.h>
#include "var.h"
#include "module.h"
#include "dbglog.h"

#ifndef VAR_GLOBAL_MAX
#define VAR_GLOBAL_MAX 64
#endif

struct var_def var_tab[VAR_GLOBAL_MAX];

int var_cnt = 0;

int var_global_add(int mod_id, const char * name, int type, int var_id)
{
	struct var_def * var;

	if (var_cnt == VAR_GLOBAL_MAX) {
		DCC_LOG(LOG_WARNING, "no more room for global variables!");
		return -1;
	}

	var = &var_tab[var_cnt++];

	var->name = (char *)name;
	var->type = type;
	var->mod_id = mod_id;
	var->id = var_id;

	DCC_LOG4(LOG_MSG, "name:'%s.%s' type:%s id:%d", module_name(mod_id),
		  name, type_name(type), var_id);

	return 0;
}

int var_global_bulk_add(int mod_id, const var_entry_t * tab, int cnt)
{
	struct var_entry * ep;
	int i;

	for (i = 0; i < cnt; i++) {
		ep = (struct var_entry *)&tab[i];
		if (var_global_add(mod_id, ep->name, ep->type, ep->id) < 0) {
			break;
		}
	}

	return i;
}

int var_global_del(int mod_id, int var_id)
{
	struct var_def * var;
	int i;

	DCC_LOG2(LOG_TRACE, "mod_id=%d var_id=%d", mod_id, var_id);

	for (i = 0; i < var_cnt; i++) {
		var = &var_tab[i];
		if ((var->mod_id == mod_id) && (var->id == var_id)) {
			var_cnt--;
			if (i != var_cnt) {
				/* move the last entry to the deleted position */
				*var = var_tab[var_cnt];
			}
			DCC_LOG(LOG_INFO, "ok.");
			return 0;
		}
	}

	DCC_LOG(LOG_WARNING, "invalid!");

	return -1;
}

int var_global_mod_del_all(int mod_id)
{
	struct var_def * var;
	int n = 0;
	int i;

	DCC_LOG1(LOG_TRACE, "mod_id=%d", mod_id);

	for (i = 0; i < var_cnt; i++) {
		var = &var_tab[i];
		while (var->mod_id == mod_id) {
			n++;
			var_cnt--;
			if (i == var_cnt)
				break;
				/* move the last entry to the deleted position */
			*var = var_tab[var_cnt];
		}
	}

	return n;
}

var_def_t * var_global_lookup(const char * name)
{
	struct var_def * var;
	int i;

	for (i = 0; i < var_cnt; i++) {
		var = &var_tab[i];
		if (strcmp(var->name, name) == 0) {
			DCC_LOG(LOG_INFO, "ok.");

			return var;
		}
	}

	DCC_LOG(LOG_INFO, "fail!");
		
	return NULL;
}

int var_set(var_def_t * var, const value_t * val)
{
	return mod_var_set(get_module_rec(var->mod_id), var->id, val);
}

int var_get(var_def_t * var, value_t * val)
{
	return mod_var_get(get_module_rec(var->mod_id), var->id, val);
}

var_def_t * var_global_next(var_def_t * var)
{
	int pos;

	if (var == NULL)
		return var_tab;

	var++;
	pos = var - var_tab;
	if (pos >= var_cnt)
		return NULL;

	return &var_tab[pos];
}

