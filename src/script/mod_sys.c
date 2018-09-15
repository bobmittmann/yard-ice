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
 * @file mod_sys.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "module.h"
#include "dbglog.h"

uint32_t eval_ans;

typedef struct sys_var {
	char name[11];
	uint8_t type;
	value_t * val;
} sys_var_t;

const struct sys_var sys_var_tab[] = {
	{ .name = "ans",
	.type = TYPE_UINT32,
	.val = (value_t *)&eval_ans }
};

int sys_var_get(void * arg, int var_id, value_t * val)
{
	struct sys_var * sv;
	int max_id = sizeof(sys_var_tab) / sizeof(struct sys_var);

	if (var_id >= max_id) {
		DCC_LOG1(LOG_ERROR, "var_id(%d) invalid!", var_id);
		return -1;
	}

	sv = (struct sys_var *)&sys_var_tab[var_id];

	DCC_LOG2(LOG_INFO, "var_id=%d '%s'", var_id, sv->name);

	switch (sv->type) {
	case TYPE_UINT32:
		val->uint32 = sv->val->uint32;
		break;
	default:
		return -1;
	}

	return 0;
}

int sys_var_set(void * arg, int var_id, const value_t * val)
{
	struct sys_var * sv;
	int max_id = sizeof(sys_var_tab) / sizeof(struct sys_var);

	if (var_id >= max_id) {
		DCC_LOG1(LOG_ERROR, "var_id(%d) invalid!", var_id);
		return -1;
	}
	
	sv = (struct sys_var *)&sys_var_tab[var_id];

	DCC_LOG2(LOG_TRACE, "var_id=%d '%s'", var_id, sv->name);

	switch (sv->type) {
	case TYPE_UINT32:
		sv->val->uint32 = val->uint32;
		break;
	default:
		return -1;
	}

	return 0;
}

int sys_init(void * arg, int mod_id)
{
	struct sys_var * sv;
	int n;
	int i;

	DCC_LOG1(LOG_TRACE, "mod_id=%d", mod_id);

	n = sizeof(sys_var_tab) / sizeof(struct sys_var);
	for (i = 0; i < n; i++) {
		sv = (struct sys_var *)&sys_var_tab[i];
		var_global_add(mod_id, sv->name, sv->type, i);
	}

	return 0;
}

int sys_done(void * arg, int mod_id)
{
	DCC_LOG1(LOG_TRACE, "mod_id=%d", mod_id);

	return 0;
}


const struct module_def sys_module = {
	.name = "sys",
	.init = (module_init_t)sys_init,
	.done = (module_done_t)sys_done,
	.var_get = (module_var_get_t)sys_var_get,
	.var_set = (module_var_set_t)sys_var_set
};

int sys_start(void)
{
	return module_register(&sys_module, stdout);
}

