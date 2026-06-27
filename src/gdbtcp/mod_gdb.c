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
 * @file mod_gdb.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 



#include <string.h>
#include <errno.h>

#include <sys/dcclog.h>

#include "module.h"
#include "val.h"
#include "var.h"
#include "gdbtcp-i.h"


enum {
	OID_GDB_TEXT,
	OID_GDB_DATA,
	OID_GDB_NOACK,
	OID_GDB_THREAD,
	OID_GDB_SIGNAL
};

const var_entry_t gdb_var_tab[] = {
	{ "text", TYPE_UINT32, OID_GDB_TEXT },
	{ "data", TYPE_UINT32, OID_GDB_DATA },
	{ "noack", TYPE_BOOL, OID_GDB_NOACK },
	{ "thread", TYPE_INT32, OID_GDB_NOACK },
	{ "signal", TYPE_INT32, OID_GDB_NOACK }
};

int mod_gdb_var_get(struct gdbtcpd * gdb, int var_id, value_t * val)
{
	switch (var_id) {
	case OID_GDB_TEXT:
		val->uint32 = gdb->offs.text;
		break;
	case OID_GDB_DATA:
		val->uint32 = gdb->offs.data;
		break;
	case OID_GDB_NOACK:
		val->logic = gdb->noack_mode;
		break;
	case OID_GDB_THREAD:
		val->int32 = gdb->thread.id;
		break;
	case OID_GDB_SIGNAL:
		val->int32 = gdb->last_signal;
		break;
	default:
		return -1;
	}
	return 0;
}

int mod_gdb_var_set(struct gdbtcpd * gdb, int var_id, const value_t * val)
{
	switch (var_id) {
	case OID_GDB_TEXT:
		gdb->offs.text = val->uint32;
		break;
	case OID_GDB_DATA:
		gdb->offs.data = val->uint32;
		break;
	case OID_GDB_NOACK:
		gdb->noack_mode = val->logic;
		break;
	case OID_GDB_THREAD:
		gdb->thread.id = val->int32;
		break;
	case OID_GDB_SIGNAL:
		gdb->last_signal = val->int32;
		break;
	default:
		return -1;
	}
	return 0;
}

int mod_gdb_on_load(struct gdbtcpd * gdb, int mod_id)
{
	var_global_bulk_add(mod_id, gdb_var_tab, 
						sizeof(gdb_var_tab) / sizeof(var_entry_t));

	return 0;
}

int mod_gdb_on_unload(struct gdbtcpd * gdb, int mod_id)
{
	var_global_mod_del_all(mod_id);

	return 0;
}

const struct module_def gdb_module = {
	.name = "gdb",
	.init = (module_init_t)mod_gdb_on_load,
	.done = (module_done_t)mod_gdb_on_unload,
	.var_get = (module_var_get_t)mod_gdb_var_get,
	.var_set = (module_var_set_t)mod_gdb_var_set
};

int mod_gdb_register(struct gdbtcpd * gdb)
{
	return module_register(&gdb_module, gdb);
}

