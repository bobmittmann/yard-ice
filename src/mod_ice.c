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
 * @file mod_ice.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 



#include <string.h>
#include <errno.h>

#include "target.h"
#include "ice_drv.h"
#include "armice.h"
#include "cm3ice.h"
#include "debugger.h"

#include <sys/dcclog.h>


const struct ice_drv_info * const ice_drv_lut[] = {
	&ice_drv_null,
	&armice_drv,
	&cm3ice_drv,
	NULL
};

const ice_drv_info_t * ice_drv_lookup(const char * name)
{
	int i;

	for (i = 0; i < sizeof(ice_drv_lut) / sizeof (ice_drv_info_t *); ++i) {
		if (strcmp(ice_drv_lut[i]->name, name) == 0) {
			return ice_drv_lut[i];
		};
	}

	return NULL;
}

enum {
	VAR_ID_ICE_NONE,
	VAR_ID_ICE_DRV
};

int mod_ice_var_get(struct debugger * dbg, int var_id, value_t * val)
{
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	DCC_LOG1(LOG_TRACE, "var_id=%d", var_id);

	switch (var_id) {
	case VAR_ID_ICE_DRV:
		val_encode(&type_def_tag, val, ice->info->name);
		break;
	default:
		return -1;
	}
	return 0;
}

int ice_drv_select(struct debugger * dbg, const ice_drv_info_t * info);

int mod_ice_var_set(struct debugger * dbg, int var_id, const value_t * val)
{
	const ice_drv_info_t * info;
	char name[12];

	DCC_LOG1(LOG_TRACE, "var_id=%d", var_id);

	switch (var_id) {
	case VAR_ID_ICE_DRV:
		val_decode(&type_def_tag, val, name);
		if ((info = ice_drv_lookup(name)) == NULL)
			return -EINVAL;
		return ice_drv_select(dbg, info);
	default:
		return -1;
	}
	return 0;
}

int mod_ice_on_load(struct debugger * dbg, int mod_id)
{
	var_global_add(mod_id, "drv", TYPE_TAG, VAR_ID_ICE_DRV);

	return 0;
}

int mod_ice_on_unload(struct debugger * dbg, int mod_id)
{
	var_global_del(mod_id, VAR_ID_ICE_DRV);

	return 0;
}

const struct module_def ice_module = {
	.name = "ice",
	.init = (module_init_t)mod_ice_on_load,
	.done = (module_done_t)mod_ice_on_unload,
	.var_get = (module_var_get_t)mod_ice_var_get,
	.var_set = (module_var_set_t)mod_ice_var_set
};

int mod_ice_register(struct debugger * dbg)
{
	return module_register(&ice_module, dbg);
}

