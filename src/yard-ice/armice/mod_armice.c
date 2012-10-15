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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/at91.h>

#include "jtag_arm.h"
#include "armice.h"
#include "module.h"
#include "dbglog.h"

#include "val.h"
#include "var.h"


extern struct armice_ctrl armice;

enum {
	OID_NULL,
	OID_ARM_R0,
	OID_ARM_R1,
	OID_ARM_R2,
	OID_ARM_R3,
	OID_ARM_R4,
	OID_ARM_R5,
	OID_ARM_R6,
	OID_ARM_R7,
	OID_ARM_R8,
	OID_ARM_R9,
	OID_ARM_R10,
	OID_ARM_R11,
	OID_ARM_R12,
	OID_ARM_R13,
	OID_ARM_R14,
	OID_ARM_R15,
	OID_ARM_CPSR,
	OID_ARM_SPSR,
	OID_THUMB_MODE,
	OID_ARM_MODE,
	OID_TARGET_ID_CODE,
	OID_WP0,
	OID_WP1
};

const var_entry_t armice_var_tab[] = {
	{ "r0", TYPE_UINT32, OID_ARM_R0 },
	{ "r1", TYPE_UINT32, OID_ARM_R1 },
	{ "r2", TYPE_UINT32, OID_ARM_R2 },
	{ "r3", TYPE_UINT32, OID_ARM_R3 },
	{ "r4", TYPE_UINT32, OID_ARM_R4 },
	{ "r5", TYPE_UINT32, OID_ARM_R5 },
	{ "r6", TYPE_UINT32, OID_ARM_R6 },
	{ "r7", TYPE_UINT32, OID_ARM_R7 },
	{ "r8", TYPE_UINT32, OID_ARM_R8 },
	{ "r9", TYPE_UINT32, OID_ARM_R9 },
	{ "r10", TYPE_UINT32, OID_ARM_R10 },
	{ "r11", TYPE_UINT32, OID_ARM_R11 },
	{ "r12", TYPE_UINT32, OID_ARM_R12 },
	{ "r13", TYPE_UINT32, OID_ARM_R13 },
	{ "r14", TYPE_UINT32, OID_ARM_R14 },
	{ "r15", TYPE_UINT32, OID_ARM_R15 },
	{ "sl", TYPE_UINT32, OID_ARM_R10 },
	{ "fp", TYPE_UINT32, OID_ARM_R11 },
	{ "ip", TYPE_UINT32, OID_ARM_R12 },
	{ "sp", TYPE_UINT32, OID_ARM_R13 },
	{ "lr", TYPE_UINT32, OID_ARM_R14 },
	{ "pc", TYPE_UINT32, OID_ARM_R15 },
	{ "cpsr", TYPE_UINT32, OID_ARM_CPSR },
	{ "spsr", TYPE_UINT32, OID_ARM_SPSR },
	{ "thumb", TYPE_UINT32, OID_THUMB_MODE },
	{ "arm", TYPE_UINT32, OID_ARM_MODE },
	{ "idcode", TYPE_UINT32, OID_TARGET_ID_CODE },
	{ "wp0", TYPE_RANGE32, OID_WP0 },
	{ "wp1", TYPE_RANGE32, OID_WP1 }
};

int armice_var_get(armice_ctrl_t * ctrl, int var_id, value_t * val)
{
	int reg;
	int ret;
	uint32_t cpsr;

	DCC_LOG1(LOG_TRACE, "var_id=%d", var_id);

	switch (var_id) {

	case OID_ARM_R0 ... OID_ARM_R15:
	case OID_ARM_CPSR:
	case OID_ARM_SPSR:
		reg = var_id - OID_ARM_R0;
		return armice_reg_get(ctrl, reg, &val->uint32);

	case OID_THUMB_MODE:
		if ((ret = armice_thumb_mode(ctrl, 1)) < 0)
			return ret;
		if ((ret = armice_reg_get(ctrl, ARM_CPSR, &cpsr)) < 0)
			return ret;
		val->logic = cpsr & ARM_STATE_THUMB ? 1 : 0;
		break;

	case OID_ARM_MODE:
		if ((ret = armice_thumb_mode(ctrl, 0)) < 0)
			return ret;
		if ((ret = armice_reg_get(ctrl, ARM_CPSR, &cpsr)) < 0)
			return ret;
		val->logic = cpsr & ARM_STATE_THUMB ? 0 : 1;
		break;

	default:
		return -1;
	}

	return 0;
}

int armice_var_set(armice_ctrl_t * ctrl, int var_id, const value_t * val)
{
	int reg;

	DCC_LOG1(LOG_TRACE, "var_id=%d", var_id);

	switch (var_id) {

	case OID_ARM_R0 ... OID_ARM_R15:
	case OID_ARM_CPSR:
	case OID_ARM_SPSR:
		reg = var_id - OID_ARM_R0;
		return armice_reg_set(ctrl, reg, val->uint32);

	case OID_THUMB_MODE:
		armice_thumb_mode(ctrl, val->logic); 
		break;

	case OID_ARM_MODE:
		armice_thumb_mode(ctrl, !val->logic); 
		break;

	default:
		return -1;
	}

	return 0;
}

int armice_on_load(armice_ctrl_t * ctrl, int mod_id)
{
	DCC_LOG1(LOG_TRACE, "mod_id=%d", mod_id);

	var_global_bulk_add(mod_id, armice_var_tab, 
						sizeof(armice_var_tab) / sizeof(var_entry_t));

	return 0;
}

int armice_on_unload(armice_ctrl_t * ctrl, int mod_id)
{
	DCC_LOG1(LOG_TRACE, "mod_id=%d", mod_id);

	var_global_mod_del_all(mod_id);

	return 0;
}

const struct module_def armice_module = {
	.name = "armice",
	.init = (module_init_t)armice_on_load,
	.done = (module_done_t)armice_on_unload,
	.var_get = (module_var_get_t)armice_var_get,
	.var_set = (module_var_set_t)armice_var_set
};

