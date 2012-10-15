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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __ARM_PN_H__
#define __ARM_PN_H__

/* ARM JTAG IDCODE */
#define ARM_CORE_ID(PN) (((PN) >> 15) & 0x0001)
#define ARM_CAPABILITY(PN) (((PN) >> 12) & 0x0007)
#define ARM_FAMILY(PN) (((PN) >> 8) & 0x000f)
#define ARM_DEVICE(PN) ((PN) & 0x00ff)

/* ARM Capabilities */
#define ARM_S 0x01
#define ARM_E 0x04
#define ARM_E_S 0x05
#define ARM_EJ 0x06
#define ARM_EJ_S 0x07
#define ARM_T 0x08
#define ARM_TDMI 0x09
#define ARM_TDMI_S 0x0a

#ifdef __ASSEMBLER__


#else /* __ASSEMBLER__ */

#include <stdint.h>
#include <stdio.h>

struct arm_pn {
	uint8_t family;
	uint8_t device;
	uint8_t capability;
	uint8_t core_id;
};

typedef struct arm_pn arm_pn_t;


#ifdef __cplusplus
extern "C" {
#endif

void arm_pn_decode(uint32_t idcode, arm_pn_t * pn);

void arm_pn_show(FILE * f, arm_pn_t * pn);

const char * arm_capability_str(arm_pn_t * pn);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __ARM_PN_H__ */

