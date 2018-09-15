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
 * @file mod_cm3ice.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "jtag_arm.h"
#include "cm3ice.h"
#include "module.h"
#include "dbglog.h"

#include "val.h"
#include "var.h"


extern struct cm3ice_ctrl cm3ice;

enum {
	OID_NULL,
	OID_CM3_R0,
	OID_CM3_R1,
	OID_CM3_R2,
	OID_CM3_R3,
	OID_CM3_R4,
	OID_CM3_R5,
	OID_CM3_R6,
	OID_CM3_R7,
	OID_CM3_R8,
	OID_CM3_R9,
	OID_CM3_R10,
	OID_CM3_R11,
	OID_CM3_R12,
	OID_CM3_R13,
	OID_CM3_R14,
	OID_CM3_R15,
	OID_CM3_XPSR,
	OID_CM3_MSP,
	OID_CM3_PSP,
	OID_CM3_CTRL,
	OID_CM4_FPSCR,
	OID_CM4_S0,
	OID_CM4_S1,
	OID_CM4_S2,
	OID_CM4_S3,
	OID_CM4_S4,
	OID_CM4_S5,
	OID_CM4_S6,
	OID_CM4_S7,
	OID_CM4_S8,
	OID_CM4_S9,
	OID_CM4_S10,
	OID_CM4_S11,
	OID_CM4_S12,
	OID_CM4_S13,
	OID_CM4_S14,
	OID_CM4_S15,
	OID_CM4_S16,
	OID_CM4_S17,
	OID_CM4_S18,
	OID_CM4_S19,
	OID_CM4_S20,
	OID_CM4_S21,
	OID_CM4_S22,
	OID_CM4_S23,
	OID_CM4_S24,
	OID_CM4_S25,
	OID_CM4_S26,
	OID_CM4_S27,
	OID_CM4_S28,
	OID_CM4_S29,
	OID_CM4_S30,
	OID_CM4_S31,
	OID_CM3_DFSR,
	OID_CM3_DHCSR,
	OID_CM3_DEMCR,
	OID_CM3_CFSR,
	OID_CM3_CONTROL,
	OID_CM3_FAULTMASK,
	OID_CM3_BASEPRI,
	OID_CM3_PRIMASK
};

const var_entry_t cm3ice_var_tab[] = {
	{ "r0", TYPE_UINT32, OID_CM3_R0 },
	{ "r1", TYPE_UINT32, OID_CM3_R1 },
	{ "r2", TYPE_UINT32, OID_CM3_R2 },
	{ "r3", TYPE_UINT32, OID_CM3_R3 },
	{ "r4", TYPE_UINT32, OID_CM3_R4 },
	{ "r5", TYPE_UINT32, OID_CM3_R5 },
	{ "r6", TYPE_UINT32, OID_CM3_R6 },
	{ "r7", TYPE_UINT32, OID_CM3_R7 },
	{ "r8", TYPE_UINT32, OID_CM3_R8 },
	{ "r9", TYPE_UINT32, OID_CM3_R9 },
	{ "r10", TYPE_UINT32, OID_CM3_R10 },
	{ "r11", TYPE_UINT32, OID_CM3_R11 },
	{ "r12", TYPE_UINT32, OID_CM3_R12 },
	{ "r13", TYPE_UINT32, OID_CM3_R13 },
	{ "r14", TYPE_UINT32, OID_CM3_R14 },
	{ "r15", TYPE_UINT32, OID_CM3_R15 },
	{ "sl", TYPE_UINT32, OID_CM3_R10 },
	{ "fp", TYPE_UINT32, OID_CM3_R11 },
	{ "ip", TYPE_UINT32, OID_CM3_R12 },
	{ "sp", TYPE_UINT32, OID_CM3_R13 },
	{ "lr", TYPE_UINT32, OID_CM3_R14 },
	{ "pc", TYPE_UINT32, OID_CM3_R15 },
	{ "xpsr", TYPE_UINT32, OID_CM3_XPSR },
	{ "msp", TYPE_UINT32, OID_CM3_MSP },
	{ "psp", TYPE_UINT32, OID_CM3_PSP },
	{ "ctrl", TYPE_UINT32, OID_CM3_CTRL },
	{ "fpscr", TYPE_UINT32, OID_CM4_FPSCR },
	{ "s0", TYPE_FLOAT, OID_CM4_S0 },
	{ "s1", TYPE_FLOAT, OID_CM4_S1 },
	{ "s2", TYPE_FLOAT, OID_CM4_S2 },
	{ "s3", TYPE_FLOAT, OID_CM4_S3 },
	{ "s4", TYPE_FLOAT, OID_CM4_S4 },
	{ "s5", TYPE_FLOAT, OID_CM4_S5 },
	{ "s6", TYPE_FLOAT, OID_CM4_S6 },
	{ "s7", TYPE_FLOAT, OID_CM4_S7 },
	{ "s8", TYPE_FLOAT, OID_CM4_S8 },
	{ "s9", TYPE_FLOAT, OID_CM4_S9 },
	{ "s10", TYPE_FLOAT, OID_CM4_S10 },
	{ "s11", TYPE_FLOAT, OID_CM4_S11 },
	{ "s12", TYPE_FLOAT, OID_CM4_S12 },
	{ "s13", TYPE_FLOAT, OID_CM4_S13 },
	{ "s14", TYPE_FLOAT, OID_CM4_S14 },
	{ "s15", TYPE_FLOAT, OID_CM4_S15 },
	{ "s16", TYPE_FLOAT, OID_CM4_S16 },
	{ "s17", TYPE_FLOAT, OID_CM4_S17 },
	{ "s18", TYPE_FLOAT, OID_CM4_S18 },
	{ "s19", TYPE_FLOAT, OID_CM4_S19 },
	{ "s20", TYPE_FLOAT, OID_CM4_S20 },
	{ "s21", TYPE_FLOAT, OID_CM4_S21 },
	{ "s22", TYPE_FLOAT, OID_CM4_S22 },
	{ "s23", TYPE_FLOAT, OID_CM4_S23 },
	{ "s24", TYPE_FLOAT, OID_CM4_S24 },
	{ "s25", TYPE_FLOAT, OID_CM4_S25 },
	{ "s26", TYPE_FLOAT, OID_CM4_S26 },
	{ "s27", TYPE_FLOAT, OID_CM4_S27 },
	{ "s28", TYPE_FLOAT, OID_CM4_S28 },
	{ "s29", TYPE_FLOAT, OID_CM4_S29 },
	{ "s30", TYPE_FLOAT, OID_CM4_S30 },
	{ "s31", TYPE_FLOAT, OID_CM4_S31 },
	{ "dfsr", TYPE_UINT32, OID_CM3_DFSR },
	{ "dhcsr", TYPE_UINT32, OID_CM3_DHCSR },
	{ "demcr", TYPE_UINT32, OID_CM3_DEMCR },
	{ "cfsr", TYPE_UINT32, OID_CM3_CFSR },
	{ "control", TYPE_UINT32, OID_CM3_CONTROL },
	{ "faultmask", TYPE_UINT32, OID_CM3_FAULTMASK },
	{ "basepri", TYPE_UINT32, OID_CM3_BASEPRI },
	{ "primask", TYPE_UINT32, OID_CM3_PRIMASK }
};

int cm3ice_var_get(cm3ice_ctrl_t * ctrl, int var_id, value_t * val)
{
	jtag_tap_t * tap = ctrl->tap;
	int reg;

	DCC_LOG1(LOG_TRACE, "var_id=%d", var_id);

	switch (var_id) {

	case OID_CM3_R0 ... OID_CM3_CTRL:
		reg = var_id - OID_CM3_R0;
		return cm3ice_reg_get(ctrl, reg, &val->uint32);

	case OID_CM4_FPSCR ... OID_CM4_S31:
		reg = var_id - OID_CM4_FPSCR;
		return cm3ice_reg_get(ctrl, reg, &val->uint32);

	case OID_CM3_DFSR:
		jtag_mem_ap_rd32(tap, ARMV7M_DFSR, &val->uint32);
		break;

	case OID_CM3_DHCSR:
		jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, &val->uint32);
		break;

	case OID_CM3_DEMCR:
		jtag_mem_ap_rd32(tap, ARMV7M_DEMCR, &val->uint32);
		break;

	case OID_CM3_CFSR:
		jtag_mem_ap_rd32(tap, ARMV7M_CFSR, &val->uint32);
		break;

	default:
		return -1;
	}

	return 0;
}

int cm3ice_var_set(cm3ice_ctrl_t * ctrl, int var_id, const value_t * val)
{
	jtag_tap_t * tap = ctrl->tap;
	int reg;

	DCC_LOG1(LOG_TRACE, "var_id=%d", var_id);

	switch (var_id) {

	case OID_CM3_R0 ... OID_CM3_CTRL:
		reg = var_id - OID_CM3_R0;
		return cm3ice_reg_set(ctrl, reg, val->uint32);

	case OID_CM4_FPSCR ... OID_CM4_S31:
		reg = var_id - OID_CM4_FPSCR;
		return cm3ice_reg_set(ctrl, reg, val->uint32);

	case OID_CM3_DFSR:
		jtag_mem_ap_wr32(tap, ARMV7M_DFSR, val->uint32);
		break;

	case OID_CM3_DHCSR:
		jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, val->uint32);
		break;

	case OID_CM3_DEMCR:
		jtag_mem_ap_wr32(tap, ARMV7M_DEMCR, val->uint32);
		break;

	case OID_CM3_CFSR:
		jtag_mem_ap_wr32(tap, ARMV7M_CFSR, val->uint32);
		break;

	default:
		return -1;
	}

	return 0;
}

/* initialize the ice driver */
int cm3ice_on_load(cm3ice_ctrl_t * ctrl, int mod_id)
{
	DCC_LOG1(LOG_TRACE, "mod_id=%d", mod_id);

	var_global_bulk_add(mod_id, cm3ice_var_tab, 
						sizeof(cm3ice_var_tab) / sizeof(var_entry_t));

	return 0;
}

int cm3ice_on_unload(cm3ice_ctrl_t * ctrl, int mod_id)
{
	DCC_LOG1(LOG_TRACE, "mod_id=%d", mod_id);

	var_global_mod_del_all(mod_id);

	return 0;
}

const struct module_def cm3ice_module = {
	.name = "cm3ice",
	.init = (module_init_t)cm3ice_on_load,
	.done = (module_done_t)cm3ice_on_unload,
	.var_get = (module_var_get_t)cm3ice_var_get,
	.var_set = (module_var_set_t)cm3ice_var_set
};

