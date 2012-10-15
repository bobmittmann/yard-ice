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


#ifndef __ARM_FPB_H__
#define __ARM_FPB_H__

/*************************************************************************/
/* Flash Patch and Breakpoint (FPB)                                            */
/*************************************************************************/

/*

0xE0002000 FP_CTRL RW a FlashPatch Control Register, FP_CTRL.
0xE0002004 FP_REMAP RW UNKNOWN FlashPatch Remap register, FP_REMAP on page C1-908.
0xE0002008 - FP_COMP0- RW UNKNOWN a FlashPatch Comparator register, FP_COMPn on
0xE0002008+4n FP_COMPn page C1-909.
0xE0002FB4 FP_LSR b RO - See Table A-2 on page AppxA-915 in Appendix A ARMv7-M CoreSight Infrastructure IDs.
0xE0002FD0 - - RO - ID space, see Appendix A ARMv7-M CoreSight
0xE0002FFC Infrastructure IDs for more information.

*/


/* ... */

#define FP_CTRL_OFFS  0x00000000 /* FlashPatch Control Register */
#define FP_REMAP_OFFS 0x00000004 /* FlashPatch Remap register */
#define FP_COMP0_OFFS 0x00000008 /* FlashPatch Comparator register 0 */
#define FP_LSR_OFFS   0x00000fb4 /*  */

/* ... */

/* ----------------------------------------------------------------------- */
/* FP_CTRL FlashPatch Control Register */
/* ----------------------------------------------------------------------- */

#define FP_NUM_CODE(CTRL) ((((CTRL) >> 8) & 0x70) | (((CTRL) >> 4) & 0xf)) 
 /* The number of instruction address comparators. */

#define FP_NUM_LIT(CTRL) (((CTRL) >> 8) & 0x7) /* The number of literal 
	address comparators supported. If this field is zero, the implementation 
	does not support literal comparators. */

#define FP_KEY (1 << 1)
#define FP_ENABLE (1 << 0)
#define FP_DISABLE (0 << 0)

/* ----------------------------------------------------------------------- */
/* FP_REMAP FlashPatch Remap register */
/* ----------------------------------------------------------------------- */

#define FP_RMPSPT(REMAP) (((CTRL) >> 29) & 0x1) /* Indicates whether the FPB 
 unit supports flash patch remap:
	0 = Remapping not supported. The FPB only supports breakpoint functionality.
	1 = Hard-wired remap to SRAM region. */


/* ----------------------------------------------------------------------- */
/* FP_COMP FlashPatch Comparator register */
/* ----------------------------------------------------------------------- */

/* 
[31:30] REPLACE For an instruction address comparator:
Defines the behavior when the COMP address is matched:
	00 = Remap to remap address, see FlashPatch Remap register, 
		FP_REMAP on page C1-908 a.
	01 = Breakpoint on lower halfword, upper is unaffected
	10 = Breakpoint on upper halfword, lower is unaffected
	11 = Breakpoint on both lower and upper halfwords.

	The reset value of this field is UNKNOWN.
	For a literal address comparator:
	Field is UNK/SBZP.
[29] - Reserved.
[28:2] COMP Bits [28:2] of the address to compare with addresses from the Code memory region.

[0] ENABLE Enable bit for this comparator: 
	0 = Comparator disabled.
	1 = Comparator enabled.
	A Power-on reset clears this bit to 0. */

#define COMP_REMAP (0 << 30)
#define COMP_BP_LOW (1 << 30)
#define COMP_BP_HIGH (2 << 30)
#define COMP_BP_WORD (3 << 30)

#define COMP_ENABLE (1 << 0)

#ifdef __ASSEMBLER__

#else /* __ASSEMBLER__ */

#include "jtag_adi.h"

#ifdef __cplusplus
extern "C" {
#endif

struct arm_fpb {
	uint32_t fp_ctrl;
	uint32_t fp_remap;
	uint32_t fp_comp[142];
	uint32_t fp_res[861];
	uint32_t fp_lsr;
};

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __ARM_FPB_H__ */

