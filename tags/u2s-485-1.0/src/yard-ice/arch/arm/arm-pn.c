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
 * @file arm-pn.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>
#include <stdio.h>

#include "jtag.h"
#include "arm_pn.h" 

#define ARM_EXCEPT_ID_COUNT 12

static const struct {
	uint16_t pn;
	uint8_t family;
	uint8_t device;
	uint8_t capability;
} arm_except_id[ARM_EXCEPT_ID_COUNT] = {
	{ 0x0700, 7, 0x00, ARM_TDMI },
	{ 0xf0f0, 7, 0x00, ARM_TDMI },
	{ 0xf1f0, 7, 0x00, ARM_TDMI_S},
	{ 0x7700, 7, 0x00, ARM_EJ_S },
	{ 0x0900, 9, 0x00, ARM_TDMI},
	{ 0xf0f0, 9, 0x00, ARM_TDMI},
	{ 0x0720, 7, 0x20, ARM_T},
	{ 0xf0f0, 7, 0x20, ARM_T},
	{ 0x0920, 9, 0x20, ARM_T},
	{ 0x0922, 9, 0x22, ARM_T},
	{ 0x0940, 9, 0x40, ARM_T},
	{ 0xf0f0, 9, 0x40, ARM_T}
};

static int except_lookup(int pn)
{
	int i;

	for (i = 0; i < ARM_EXCEPT_ID_COUNT; i++) {
		if (pn == arm_except_id[i].pn) {
			return i;
		}
	}	

	return -1;
}

void arm_pn_decode(uint32_t idcode, arm_pn_t * pn)
{
	int n;
	int id;

	id = JTAG_IDCODE_PART_NUMBER(idcode);

	if ((n = except_lookup(id)) < 0) {
		pn->core_id = ARM_CORE_ID(id);
		pn->capability = ARM_CAPABILITY(id);
		pn->family = ARM_FAMILY(id);
		pn->device = ARM_DEVICE(id);
	} else {
		pn->core_id  = 0;
		pn->family = arm_except_id[n].family;
		pn->device = arm_except_id[n].device;
		pn->capability = arm_except_id[n].capability;
	}
}

static const char arm_capability_tab[][7] = {
	"",
	"-S",
	"?",
	"?",
	"E",
	"E-S",
	"EJ",
	"EJ-S",
	"T",
	"TDMI",
	"TDMI-S"
};

void arm_pn_show(FILE * f, arm_pn_t * pn)
{
	if (pn->core_id) {
		fprintf(f, "Not an ARM core core");
	} 

	if (pn->device == 0)
		fprintf(f, "ARM%d%s", pn->family, arm_capability_tab[pn->capability]);
	else
		fprintf(f, "ARM%d%02x%s", pn->family, pn->device, 
			   arm_capability_tab[pn->capability]);
}

const char * arm_capability_str(arm_pn_t * pn)
{
	return arm_capability_tab[pn->capability];
}

