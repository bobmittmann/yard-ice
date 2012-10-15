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
#include <stdio.h>

#include "arm_opc.h"

#include <sys/dcclog.h>

const struct arm_opcode arm_opcodes[] =
{
	/* ARM instructions.  */
	{ 0xe1a00000, 0xffffffff, "nop\t\t\t(mov r0,r0)"},
	{ 0x012fff10, 0x0ffffff0, "bx%c\t%0-3r"},
	{ 0x00000090, 0x0fe000f0, "mul%c%20's\t%16-19r, %0-3r, %8-11r"},

	{ 0x00200090, 0x0fe000f0,
	 "mla%c%20's\t%16-19r, %0-3r, %8-11r, %12-15r"},

	{ 0x01000090, 0x0fb00ff0,
	 "swp%c%22'b\t%12-15r, %0-3r, [%16-19r]"},

	{ 0x00800090, 0x0fa000f0,
	 "%22?sumull%c%20's\t%12-15r, %16-19r, %0-3r, %8-11r"},

	{ 0x00a00090, 0x0fa000f0,
	 "%22?sumlal%c%20's\t%12-15r, %16-19r, %0-3r, %8-11r"},

	/* XScale instructions.  */
	{ 0x0e200010, 0x0fff0ff0, "mia%c\tacc0, %0-3r, %12-15r"},
	{ 0x0e280010, 0x0fff0ff0, "miaph%c\tacc0, %0-3r, %12-15r"},
	{ 0x0e2c0010, 0x0ffc0ff0,
	 "mia%17'T%17`B%16'T%16`B%c\tacc0, %0-3r, %12-15r"},

	{ 0x0c400000, 0x0ff00fff, "mar%c\tacc0, %12-15r, %16-19r"},
	{ 0x0c500000, 0x0ff00fff, "mra%c\t%12-15r, %16-19r, acc0"},
	{ 0xf450f000, 0xfc70f000, "pld\t%a"},

	/* V5 Instructions.  */
	{ 0xe1200070, 0xfff000f0, "bkpt\t0x%16-19X%12-15X%8-11X%0-3X"},
	{ 0xfa000000, 0xfe000000, "blx\t%B"},
	{ 0x012fff30, 0x0ffffff0, "blx%c\t%0-3r"},
	{ 0x016f0f10, 0x0fff0ff0, "clz%c\t%12-15r, %0-3r"},
	{ 0xfc100000, 0xfe100000, "ldc2%22'l\t%8-11d, cr%12-15d, %A"},
	{ 0xfc000000, 0xfe100000, "stc2%22'l\t%8-11d, cr%12-15d, %A"},

	{ 0xfe000000, 0xff000010,
	 "cdp2\t%8-11d, %20-23d, cr%12-15d, cr%16-19d, cr%0-3d, {%5-7d}"},

	{ 0xfe000010, 0xff100010,
	 "mcr2\t%8-11d, %21-23d, %12-15r, cr%16-19d, cr%0-3d, {%5-7d}"},

	{ 0xfe100010, 0xff100010,
	 "mrc2\t%8-11d, %21-23d, %12-15r, cr%16-19d, cr%0-3d, {%5-7d}"},

	/* V5E "El Segundo" Instructions.  */    
	{ 0x000000d0, 0x0e1000f0, "ldr%cd\t%12-15r, %s"},
	{ 0x000000f0, 0x0e1000f0, "str%cd\t%12-15r, %s"},

	{ 0x01000080, 0x0ff000f0,
	 "smlabb%c\t%16-19r, %0-3r, %8-11r, %12-15r"},

	{ 0x010000a0, 0x0ff000f0,
	 "smlatb%c\t%16-19r, %0-3r, %8-11r, %12-15r"},

	{ 0x010000c0, 0x0ff000f0,
	 "smlabt%c\t%16-19r, %0-3r, %8-11r, %12-15r"},

	{ 0x010000e0, 0x0ff000f0,
	 "smlatt%c\t%16-19r, %0-3r, %8-11r, %12-15r"},

	{ 0x01200080, 0x0ff000f0,
	 "smlawb%c\t%16-19r, %0-3r, %8-11r, %12-15r"},

	{ 0x012000c0, 0x0ff000f0,
	 "smlawt%c\t%16-19r, %0-3r, %8-11r, %12-15r"},

	{ 0x01400080, 0x0ff000f0,
	 "smlalbb%c\t%12-15r, %16-19r, %0-3r, %8-11r"},

	{ 0x014000a0, 0x0ff000f0,
	 "smlaltb%c\t%12-15r, %16-19r, %0-3r, %8-11r"},

	{ 0x014000c0, 0x0ff000f0,
	 "smlalbt%c\t%12-15r, %16-19r, %0-3r, %8-11r"},

	{ 0x014000e0, 0x0ff000f0,
	 "smlaltt%c\t%12-15r, %16-19r, %0-3r, %8-11r"},

	{ 0x01600080, 0x0ff0f0f0, "smulbb%c\t%16-19r, %0-3r, %8-11r"},
	{ 0x016000a0, 0x0ff0f0f0, "smultb%c\t%16-19r, %0-3r, %8-11r"},
	{ 0x016000c0, 0x0ff0f0f0, "smulbt%c\t%16-19r, %0-3r, %8-11r"},
	{ 0x016000e0, 0x0ff0f0f0, "smultt%c\t%16-19r, %0-3r, %8-11r"},

	{ 0x012000a0, 0x0ff0f0f0, "smulwb%c\t%16-19r, %0-3r, %8-11r"},
	{ 0x012000e0, 0x0ff0f0f0, "smulwt%c\t%16-19r, %0-3r, %8-11r"},

	{ 0x01000050, 0x0ff00ff0, "qadd%c\t%12-15r, %0-3r, %16-19r"},
	{ 0x01400050, 0x0ff00ff0, "qdadd%c\t%12-15r, %0-3r, %16-19r"},
	{ 0x01200050, 0x0ff00ff0, "qsub%c\t%12-15r, %0-3r, %16-19r"},
	{ 0x01600050, 0x0ff00ff0, "qdsub%c\t%12-15r, %0-3r, %16-19r"},

	{ 0x0c400000, 0x0ff00000,
	 "mcrr%c\t%8-11d, %4-7d, %12-15r, %16-19r, cr%0-3d"},

	{ 0x0c500000, 0x0ff00000,
	 "mrrc%c\t%8-11d, %4-7d, %12-15r, %16-19r, cr%0-3d"},

	/* ARM Instructions.  */
	{ 0x00000090, 0x0e100090, "str%c%6's%h\t%12-15r, %s"},
	{ 0x00100090, 0x0e100090, "ldr%c%6's%h\t%12-15r, %s"},
	{ 0x00000000, 0x0de00000, "and%c%20's\t%12-15r, %16-19r, %o"},
	{ 0x00200000, 0x0de00000, "eor%c%20's\t%12-15r, %16-19r, %o"},
	{ 0x00400000, 0x0de00000, "sub%c%20's\t%12-15r, %16-19r, %o"},
	{ 0x00600000, 0x0de00000, "rsb%c%20's\t%12-15r, %16-19r, %o"},
	{ 0x00800000, 0x0de00000, "add%c%20's\t%12-15r, %16-19r, %o"},
	{ 0x00a00000, 0x0de00000, "adc%c%20's\t%12-15r, %16-19r, %o"},
	{ 0x00c00000, 0x0de00000, "sbc%c%20's\t%12-15r, %16-19r, %o"},
	{ 0x00e00000, 0x0de00000, "rsc%c%20's\t%12-15r, %16-19r, %o"},
	{ 0x0120f000, 0x0db0f000, "msr%c\t%22?SCPSR%C, %o"},
	{ 0x010f0000, 0x0fbf0fff, "mrs%c\t%12-15r, %22?SCPSR"},
	{ 0x01000000, 0x0de00000, "tst%c%p\t%16-19r, %o"},
	{ 0x01200000, 0x0de00000, "teq%c%p\t%16-19r, %o"},
	{ 0x01400000, 0x0de00000, "cmp%c%p\t%16-19r, %o"},
	{ 0x01600000, 0x0de00000, "cmn%c%p\t%16-19r, %o"},
	{ 0x01800000, 0x0de00000, "orr%c%20's\t%12-15r, %16-19r, %o"},
	{ 0x01a00000, 0x0de00000, "mov%c%20's\t%12-15r, %o"},
	{ 0x01c00000, 0x0de00000, "bic%c%20's\t%12-15r, %16-19r, %o"},
	{ 0x01e00000, 0x0de00000, "mvn%c%20's\t%12-15r, %o"},
	{ 0x04000000, 0x0e100000, "str%c%22'b%t\t%12-15r, %a"},
	{ 0x06000000, 0x0e100ff0, "str%c%22'b%t\t%12-15r, %a"},
	
	{ 0x04000000, 0x0c100010, "str%c%22'b%t\t%12-15r, %a"},

	{ 0x06000010, 0x0e000010, "undefined"},
	{ 0x04100000, 0x0c100000, "ldr%c%22'b%t\t%12-15r, %a"},

	{ 0x08000000, 0x0e100000,
		"stm%c%23?id%24?ba\t%16-19r%21'!, %m%22'^"},

	{ 0x08100000, 0x0e100000,
		"ldm%c%23?id%24?ba\t%16-19r%21'!, %m%22'^"},

	{ 0x0a000000, 0x0e000000, "b%24'l%c\t%b"},
	{ 0x0f000000, 0x0f000000, "swi%c\t%0-23x"},

	/* Floating point coprocessor instructions */
	{ 0x0e000100, 0x0ff08f10, "adf%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0e100100, 0x0ff08f10, "muf%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0e200100, 0x0ff08f10, "suf%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0e300100, 0x0ff08f10, "rsf%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0e400100, 0x0ff08f10, "dvf%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0e500100, 0x0ff08f10, "rdf%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0e600100, 0x0ff08f10, "pow%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0e700100, 0x0ff08f10, "rpw%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0e800100, 0x0ff08f10, "rmf%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0e900100, 0x0ff08f10, "fml%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0ea00100, 0x0ff08f10, "fdv%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0eb00100, 0x0ff08f10, "frd%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0ec00100, 0x0ff08f10, "pol%c%P%R\t%12-14f, %16-18f, %0-3f"},
	{ 0x0e008100, 0x0ff08f10, "mvf%c%P%R\t%12-14f, %0-3f"},
	{ 0x0e108100, 0x0ff08f10, "mnf%c%P%R\t%12-14f, %0-3f"},
	{ 0x0e208100, 0x0ff08f10, "abs%c%P%R\t%12-14f, %0-3f"},
	{ 0x0e308100, 0x0ff08f10, "rnd%c%P%R\t%12-14f, %0-3f"},
	{ 0x0e408100, 0x0ff08f10, "sqt%c%P%R\t%12-14f, %0-3f"},
	{ 0x0e508100, 0x0ff08f10, "log%c%P%R\t%12-14f, %0-3f"},
	{ 0x0e608100, 0x0ff08f10, "lgn%c%P%R\t%12-14f, %0-3f"},
	{ 0x0e708100, 0x0ff08f10, "exp%c%P%R\t%12-14f, %0-3f"},
	{ 0x0e808100, 0x0ff08f10, "sin%c%P%R\t%12-14f, %0-3f"},
	{ 0x0e908100, 0x0ff08f10, "cos%c%P%R\t%12-14f, %0-3f"},
	{ 0x0ea08100, 0x0ff08f10, "tan%c%P%R\t%12-14f, %0-3f"},
	{ 0x0eb08100, 0x0ff08f10, "asn%c%P%R\t%12-14f, %0-3f"},
	{ 0x0ec08100, 0x0ff08f10, "acs%c%P%R\t%12-14f, %0-3f"},
	{ 0x0ed08100, 0x0ff08f10, "atn%c%P%R\t%12-14f, %0-3f"},
	{ 0x0ee08100, 0x0ff08f10, "urd%c%P%R\t%12-14f, %0-3f"},
	{ 0x0ef08100, 0x0ff08f10, "nrm%c%P%R\t%12-14f, %0-3f"},
	{ 0x0e000110, 0x0ff00f1f, "flt%c%P%R\t%16-18f, %12-15r"},
	{ 0x0e100110, 0x0fff0f98, "fix%c%R\t%12-15r, %0-2f"},
	{ 0x0e200110, 0x0fff0fff, "wfs%c\t%12-15r"},
	{ 0x0e300110, 0x0fff0fff, "rfs%c\t%12-15r"},
	{ 0x0e400110, 0x0fff0fff, "wfc%c\t%12-15r"},
	{ 0x0e500110, 0x0fff0fff, "rfc%c\t%12-15r"},
	{ 0x0e90f110, 0x0ff8fff0, "cmf%c\t%16-18f, %0-3f"},
	{ 0x0eb0f110, 0x0ff8fff0, "cnf%c\t%16-18f, %0-3f"},
	{ 0x0ed0f110, 0x0ff8fff0, "cmfe%c\t%16-18f, %0-3f"},
	{ 0x0ef0f110, 0x0ff8fff0, "cnfe%c\t%16-18f, %0-3f"},
	{ 0x0c000100, 0x0e100f00, "stf%c%Q\t%12-14f, %A"},
	{ 0x0c100100, 0x0e100f00, "ldf%c%Q\t%12-14f, %A"},
	{ 0x0c000200, 0x0e100f00, "sfm%c\t%12-14f, %F, %A"},
	{ 0x0c100200, 0x0e100f00, "lfm%c\t%12-14f, %F, %A"},

	/* Generic coprocessor instructions */
	{ 0x0e000000, 0x0f000010,
	 "cdp%c\t%8-11d, %20-23d, cr%12-15d, cr%16-19d, cr%0-3d, {%5-7d}"},

	{ 0x0e100010, 0x0f100010,
	 "mrc%c\t%8-11d, %21-23d, %12-15r, cr%16-19d, cr%0-3d, {%5-7d}"},

	{ 0x0e000010, 0x0f100010,
	 "mcr%c\t%8-11d, %21-23d, %12-15r, cr%16-19d, cr%0-3d, {%5-7d}"},

	{ 0x0c000000, 0x0e100000, "stc%c%22'l\t%8-11d, cr%12-15d, %A"},
	{ 0x0c100000, 0x0e100000, "ldc%c%22'l\t%8-11d, cr%12-15d, %A"},

	/* The rest.  */
	{ 0x00000000, 0x00000000, "undefined instruction %0-31x"},

	{ 0x00000000, 0x00000000, NULL}
};

#define BDISP(x) ((((x) & 0xffffff) ^ 0x800000) - 0x800000) /* 26 bit */

const struct thumb_opcode thumb_opcodes[] = {
	/* Thumb instructions.  */

	/* ARM V5 ISA extends Thumb.  */
	{ 0xbe00, 0xff00, "bkpt\t%0-7x"},
	{ 0x4780, 0xff87, "blx\t%3-6r"},	
	/* note: 4 bit register number.  */
	/* Note: this is BLX(2).  BLX(1) is done in arm-dis.c/print_insn_thumb()
	 as an extension of the special processing there for Thumb BL.
	 BL and BLX(1) involve 2 successive 16-bit instructions, which must
	 always appear together in the correct order.  So, the empty
	 string is put in this table, and the string interpreter takes <empty>
	 to mean it has a pair of BL-ish instructions.  */
	{ 0x46c0, 0xffff, "nop\t\t\t(mov r8, r8)"},
	/* Format 5 instructions do not update the PSR.  */
	{ 0x1c00, 0xffc0,
	 "mov\t%0-2r, %3-5r\t\t(add %0-2r, %3-5r, #%6-8d)"},
	/* Format 4.  */
	{ 0x4000, 0xffc0, "and\t%0-2r, %3-5r"},
	{ 0x4040, 0xffc0, "eor\t%0-2r, %3-5r"},
	{ 0x4080, 0xffc0, "lsl\t%0-2r, %3-5r"},
	{ 0x40c0, 0xffc0, "lsr\t%0-2r, %3-5r"},
	{ 0x4100, 0xffc0, "asr\t%0-2r, %3-5r"},
	{ 0x4140, 0xffc0, "adc\t%0-2r, %3-5r"},
	{ 0x4180, 0xffc0, "sbc\t%0-2r, %3-5r"},
	{ 0x41c0, 0xffc0, "ror\t%0-2r, %3-5r"},
	{ 0x4200, 0xffc0, "tst\t%0-2r, %3-5r"},
	{ 0x4240, 0xffc0, "neg\t%0-2r, %3-5r"},
	{ 0x4280, 0xffc0, "cmp\t%0-2r, %3-5r"},
	{ 0x42c0, 0xffc0, "cmn\t%0-2r, %3-5r"},
	{ 0x4300, 0xffc0, "orr\t%0-2r, %3-5r"},
	{ 0x4340, 0xffc0, "mul\t%0-2r, %3-5r"},
	{ 0x4380, 0xffc0, "bic\t%0-2r, %3-5r"},
	{ 0x43c0, 0xffc0, "mvn\t%0-2r, %3-5r"},
	/* format 13 */
	{ 0xb000, 0xff80, "add\tsp, #%0-6W"},
	{ 0xb080, 0xff80, "sub\tsp, #%0-6W"},
	/* format 5 */
	{ 0x4700, 0xff80, "bx\t%S"},
	{ 0x4400, 0xff00, "add\t%D, %S"},
	{ 0x4500, 0xff00, "cmp\t%D, %S"},
	{ 0x4600, 0xff00, "mov\t%D, %S"},
	/* format 14 */
	{ 0xb400, 0xfe00, "push\t%N"},
	{ 0xbc00, 0xfe00, "pop\t%O"},
	/* format 2 */
	{ 0x1800, 0xfe00, "add\t%0-2r, %3-5r, %6-8r"},
	{ 0x1a00, 0xfe00, "sub\t%0-2r, %3-5r, %6-8r"},
	{ 0x1c00, 0xfe00, "add\t%0-2r, %3-5r, #%6-8d"},
	{ 0x1e00, 0xfe00, "sub\t%0-2r, %3-5r, #%6-8d"},
	/* format 8 */
	{ 0x5200, 0xfe00, "strh\t%0-2r, [%3-5r, %6-8r]"},
	{ 0x5a00, 0xfe00, "ldrh\t%0-2r, [%3-5r, %6-8r]"},
	{ 0x5600, 0xf600, "ldrs%11?hb\t%0-2r, [%3-5r, %6-8r]"},
	/* format 7 */
	{ 0x5000, 0xfa00, "str%10'b\t%0-2r, [%3-5r, %6-8r]"},
	{ 0x5800, 0xfa00, "ldr%10'b\t%0-2r, [%3-5r, %6-8r]"},
	/* format 1 */
	{ 0x0000, 0xf800, "lsl\t%0-2r, %3-5r, #%6-10d"},
	{ 0x0800, 0xf800, "lsr\t%0-2r, %3-5r, #%6-10d"},
	{ 0x1000, 0xf800, "asr\t%0-2r, %3-5r, #%6-10d"},
	/* format 3 */
	{ 0x2000, 0xf800, "mov\t%8-10r, #%0-7d"},
	{ 0x2800, 0xf800, "cmp\t%8-10r, #%0-7d"},
	{ 0x3000, 0xf800, "add\t%8-10r, #%0-7d"},
	{ 0x3800, 0xf800, "sub\t%8-10r, #%0-7d"},
	/* format 6 */
	{ 0x4800, 0xf800, "ldr\t%8-10r, [pc, #%0-7W]\t(%0-7a)"},  
	/* TODO: Disassemble PC relative "LDR rD,=<symbolic>" */
	/* format 9 */
	{ 0x6000, 0xf800, "str\t%0-2r, [%3-5r, #%6-10W]"},
	{ 0x6800, 0xf800, "ldr\t%0-2r, [%3-5r, #%6-10W]"},
	{ 0x7000, 0xf800, "strb\t%0-2r, [%3-5r, #%6-10d]"},
	{ 0x7800, 0xf800, "ldrb\t%0-2r, [%3-5r, #%6-10d]"},
	/* format 10 */
	{ 0x8000, 0xf800, "strh\t%0-2r, [%3-5r, #%6-10H]"},
	{ 0x8800, 0xf800, "ldrh\t%0-2r, [%3-5r, #%6-10H]"},
	/* format 11 */
	{ 0x9000, 0xf800, "str\t%8-10r, [sp, #%0-7W]"},
	{ 0x9800, 0xf800, "ldr\t%8-10r, [sp, #%0-7W]"},
	/* format 12 */
	{ 0xa000, 0xf800, "add\t%8-10r, pc, #%0-7W\t(adr %8-10r,%0-7a)"},
	{ 0xa800, 0xf800, "add\t%8-10r, sp, #%0-7W"},
	/* format 15 */
	{ 0xc000, 0xf800, "stmia\t%8-10r!,%M"},
	{ 0xc800, 0xf800, "ldmia\t%8-10r!,%M"},
	/* format 18 */
	{ 0xe000, 0xf800, "b\t%0-10B"},
	{ 0xe800, 0xf800, "undefined"},
	/* format 19 */
	{ 0xf000, 0xf800, ""}, 
	/* special processing required in disassembler */
	{ 0xf800, 0xf800, "second half of BL instruction %0-15x"},
	/* format 16 */
	{ 0xd000, 0xff00, "beq\t%0-7B"},
	{ 0xd100, 0xff00, "bne\t%0-7B"},
	{ 0xd200, 0xff00, "bcs\t%0-7B"},
	{ 0xd300, 0xff00, "bcc\t%0-7B"},
	{ 0xd400, 0xff00, "bmi\t%0-7B"},
	{ 0xd500, 0xff00, "bpl\t%0-7B"},
	{ 0xd600, 0xff00, "bvs\t%0-7B"},
	{ 0xd700, 0xff00, "bvc\t%0-7B"},
	{ 0xd800, 0xff00, "bhi\t%0-7B"},
	{ 0xd900, 0xff00, "bls\t%0-7B"},
	{ 0xda00, 0xff00, "bge\t%0-7B"},
	{ 0xdb00, 0xff00, "blt\t%0-7B"},
	{ 0xdc00, 0xff00, "bgt\t%0-7B"},
	{ 0xdd00, 0xff00, "ble\t%0-7B"},
	/* format 17 */
	{ 0xde00, 0xff00, "bal\t%0-7B"},
	{ 0xdf00, 0xff00, "swi\t%0-7d"},
	/* format 9 */
	{ 0x6000, 0xf800, "str\t%0-2r, [%3-5r, #%6-10W]"},
	{ 0x6800, 0xf800, "ldr\t%0-2r, [%3-5r, #%6-10W]"},
	{ 0x7000, 0xf800, "strb\t%0-2r, [%3-5r, #%6-10d]"},
	{ 0x7800, 0xf800, "ldrb\t%0-2r, [%3-5r, #%6-10d]"},
	/* the rest */
	{ 0x0000, 0x0000, "undefined instruction %0-15x"},

	{ 0x0000, 0x0000, NULL}
};


#define BDISP23(x) ((((((x) & 0x07ff) << 11) | (((x) & 0x07ff0000) >> 16)) \
                     ^ 0x200000) - 0x200000) /* 23bit */

#ifndef streq
#define streq(a,b)	(strcmp ((a), (b)) == 0)
#endif

#ifndef strneq
#define strneq(a,b,n)	(strncmp ((a), (b), (n)) == 0)
#endif

#ifndef NUM_ELEM
#define NUM_ELEM(a)     (sizeof (a) / sizeof (a)[0])
#endif

static const char * const arm_conditional[] =
	{ "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
	"hi", "ls", "ge", "lt", "gt", "le", "", "nv"
};

const char * const arm_regnames[18] = {
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", 
	"r8", "r9", "sl", "fp", "ip", "sp", "lr", "pc", "cpsr", "spsr" };

static const char * const arm_fp_const[] =
	{ "0.0", "1.0", "2.0", "3.0", "4.0", "5.0", "0.5", "10.0" };

static const char * const arm_shift[] = { "lsl", "lsr", "asr", "ror" };

static int fmt_addr(char * s, unsigned int addr)
{
	return sprintf(s, "%08x", (unsigned int)(addr));
}

const char * arm_reg_name(int r)
{
	return arm_regnames[r];
}

/* find a register by name */
int arm_lookup_reg(char * name)
{
	int r;

	for (r = 0; r < 18; r++) {
		if (strcmp(name, arm_regnames[r]) == 0)
			return r;
	}

	return -1;
} 

static int arm_decode_shift(char * s, unsigned int opc)
{
	int n;

	n = sprintf(s, "%s", arm_regnames[opc & 0xf]);
	s += n;

	if ((opc & 0xff0) != 0) {
		if ((opc & 0x10) == 0) {
			int amount = (opc & 0xf80) >> 7;
			int shift = (opc & 0x60) >> 5;

			if (amount == 0) {
				if (shift == 3) {
					n += sprintf(s, ", rrx");
					return n;
				}

				amount = 32;
			}

			n += sprintf(s, ", %s #%d", arm_shift[shift], amount);
		} else
			n += sprintf(s, ", %s %s", arm_shift[(opc & 0x60) >> 5],
				 arm_regnames[(opc & 0xf00) >> 8]);
	}

	return n;
}

/* Print one instruction from PC.
   Return the size of the instruction (always 4 on ARM). */
int arm_fmt_insn(char * s, unsigned int pc, unsigned int opc)
{
	struct arm_opcode * insn;
	char * cp = s;

	for (insn = (struct arm_opcode *)arm_opcodes; insn->assembler; insn++) {
		if ((opc & insn->mask) == insn->value) {
			char *c;

			DCC_LOG1(LOG_MSG, "match: %s", insn->assembler);

			for (c = (char *)insn->assembler; *c; c++) {
				if (*c == '%') {
					switch (*++c) {
					case '%':
						*cp++ = '%';
						break;

					case 'a':
						if (((opc & 0x000f0000) == 0x000f0000)
							&& ((opc & 0x02000000) == 0)) {
							int offset = opc & 0xfff;

							cp += sprintf(cp, "[pc");

							if (opc & 0x01000000) {
								if ((opc & 0x00800000) == 0)
									offset = -offset;

								/* pre-indexed */
								cp += sprintf(cp, ", #%d]", offset);

								offset += pc + 8;

								/* Cope with the possibility of write-back
								   being used.  Probably a very dangerous thing
								   for the programmer to do, but who are we to
								   argue ?  */
								if (opc & 0x00200000)
									*cp++ = '!';
							} else {
								/* Post indexed.  */
								cp += sprintf(cp, "], #%d", offset);

								offset = pc + 8;	/* ie ignore the offset.  */
							}

							cp += sprintf(cp, "\t; ");
							cp += fmt_addr(cp, offset);
							
						} else {
							cp += sprintf(cp, "[%s",
								 arm_regnames[(opc >> 16) & 0xf]);
							if ((opc & 0x01000000) != 0) {
								if ((opc & 0x02000000) == 0) {
									int offset = opc & 0xfff;

									if (offset)
										cp += sprintf(cp, ", %s#%d",
											 (((opc & 0x00800000) == 0)
											  ? "-" : ""), offset);
								} else {
									cp += sprintf(cp, ", %s",
										 (((opc & 0x00800000) == 0)
										  ? "-" : ""));
									cp += arm_decode_shift(cp, opc);
								}

								cp += sprintf(cp, "]%s",
									 ((opc & 0x00200000) != 0) ? "!" : "");
							} else {
								if ((opc & 0x02000000) == 0) {
									int offset = opc & 0xfff;

									if (offset)
										cp += sprintf(cp, "], %s#%d",
											 (((opc & 0x00800000) == 0)
											  ? "-" : ""), offset);
									else
										*cp++ = ']';
								} else {
									cp += sprintf(cp, "], %s",
										 (((opc & 0x00800000) == 0)
										  ? "-" : ""));
									cp += arm_decode_shift(cp, opc);
								}
							}
						}
						break;

					case 's':
						if ((opc & 0x004f0000) == 0x004f0000) {
							/* PC relative with immediate offset.  */
							int offset =
								((opc & 0xf00) >> 4) | (opc & 0xf);

							if ((opc & 0x00800000) == 0)
								offset = -offset;

							cp += sprintf(cp, "[pc, #%d]\t; ", offset);

							cp += fmt_addr(cp, offset + pc + 8);
						} else {
							cp += sprintf(cp, "[%s",
								 arm_regnames[(opc >> 16) & 0xf]);
							if ((opc & 0x01000000) != 0) {
								/* Pre-indexed.  */
								if ((opc & 0x00400000) == 0x00400000) {
									/* Immediate.  */
									int offset =
										((opc & 0xf00) >> 4) | (opc &
																  0xf);
									if (offset)
										cp += sprintf(cp, ", %s#%d",
											 (((opc & 0x00800000) == 0)
											  ? "-" : ""), offset);
								} else {
									/* Register.  */
									cp += sprintf(cp, ", %s%s",
										 (((opc & 0x00800000) == 0)
										  ? "-" : ""),
										 arm_regnames[opc & 0xf]);
								}

								cp += sprintf(cp, "]%s",
									 ((opc & 0x00200000) != 0) ? "!" : "");
							} else {
								/* Post-indexed.  */
								if ((opc & 0x00400000) == 0x00400000) {
									/* Immediate.  */
									int offset =
										((opc & 0xf00) >> 4) | (opc &
																  0xf);
									if (offset)
										cp += sprintf(cp, "], %s#%d",
											 (((opc & 0x00800000) == 0)
											  ? "-" : ""), offset);
									else
										*cp++ = ']';
								} else {
									/* Register.  */
									cp += sprintf(cp, "], %s%s",
										 (((opc & 0x00800000) == 0)
										  ? "-" : ""),
										 arm_regnames[opc & 0xf]);
								}
							}
						}
						break;

					case 'b':
						cp += fmt_addr(cp, BDISP(opc) * 4 + pc + 8);
						break;

					case 'c':
						cp += sprintf(cp, "%s",
							 arm_conditional[(opc >> 28) & 0xf]);
						break;

					case 'm':
					{
						int started = 0;
						int reg;

						*cp++ = '{';
						for (reg = 0; reg < 16; reg++)
							if ((opc & (1 << reg)) != 0) {
								if (started) {
									*cp++ = ',';
									*cp++ = ' ';
								}
								started = 1;
								cp += sprintf(cp, "%s", arm_regnames[reg]);
							}
						*cp++ = '}';
					}
						break;

					case 'o':
						if ((opc & 0x02000000) != 0) {
							int rotate = (opc & 0xf00) >> 7;
							int immed = (opc & 0xff);

							immed = (((immed << (32 - rotate))
									  | (immed >> rotate)) & 0xffffffff);
							cp += sprintf(cp, "#%d\t; 0x%x", immed, immed);
						} else
							cp += arm_decode_shift(cp, opc);
						break;

					case 'p':
						if ((opc & 0x0000f000) == 0x0000f000)
							*cp++ = 'p';
						break;

					case 't':
						if ((opc & 0x01200000) == 0x00200000)
							*cp++ = 't';
						break;

					case 'h':
						if ((opc & 0x00000020) == 0x00000020)
							*cp++ = 'h';
						else
							*cp++ = 'b';
						break;

					case 'A':
						cp += sprintf(cp, "[%s",
							 arm_regnames[(opc >> 16) & 0xf]);
						if ((opc & 0x01000000) != 0) {
							int offset = opc & 0xff;

							if (offset)
								cp += sprintf(cp, ", %s#%d]%s",
									 ((opc & 0x00800000) == 0 ? "-" : ""),
									 offset * 4,
									 ((opc & 0x00200000) != 0 ? "!" : ""));
							else
								*cp++ = ']';
						} else {
							int offset = opc & 0xff;

							if (offset)
								cp += sprintf(cp, "], %s#%d",
									 ((opc & 0x00800000) == 0 ? "-" : ""),
									 offset * 4);
							else
								*cp++ = ']';
						}
						break;

					case 'B':
						/* Print ARM V5 BLX(1) address: pc+25 bits.  */
					{
						unsigned int address;
						unsigned int offset = 0;

						if (opc & 0x00800000)
							/* Is signed, hi bits should be ones.  */
							offset = (-1) ^ 0x00ffffff;

						/* Offset is (SignExtend(offset field)<<2).  */
						offset += opc & 0x00ffffff;
						offset <<= 2;
						address = offset + pc + 8;

						if (opc & 0x01000000)
							/* H bit allows addressing to 2-byte boundaries.  */
							address += 2;

						cp += fmt_addr(cp, address);
					}
						break;

					case 'C':
						*cp++ = '_';
						if (opc & 0x80000)
							*cp++ = 'f';
						if (opc & 0x40000)
							*cp++ = 's';
						if (opc & 0x20000)
							*cp++ = 'x';
						if (opc & 0x10000)
							*cp++ = 'c';
						break;

					case 'F':
						switch (opc & 0x00408000) {
						case 0:
							*cp++ = '4';
							break;
						case 0x8000:
							*cp++ = '1';
							break;
						case 0x00400000:
							*cp++ = '2';
							break;
						default:
							*cp++ = '3';
						}
						break;

					case 'P':
						switch (opc & 0x00080080) {
						case 0:
							*cp++ = 's';
							break;
						case 0x80:
							*cp++ = 'd';
							break;
						case 0x00080000:
							*cp++ = 'e';
							break;
						default:
							cp += sprintf(cp, "<illegal precision>");
							break;
						}
						break;
					case 'Q':
						switch (opc & 0x00408000) {
						case 0:
							*cp++ = 's';
							break;
						case 0x8000:
							*cp++ = 'd';
							break;
						case 0x00400000:
							*cp++ = 'e';
							break;
						default:
							*cp++ = 'p';
							break;
						}
						break;
					case 'R':
						switch (opc & 0x60) {
						case 0:
							break;
						case 0x20:
							*cp++ = 'p';
							break;
						case 0x40:
							*cp++ = 'm';
							break;
						default:
							*cp++ = 'z';
							break;
						}
						break;

					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					{
						int bitstart = *c++ - '0';
						int bitend = 0;

						while (*c >= '0' && *c <= '9')
							bitstart = (bitstart * 10) + *c++ - '0';

						switch (*c) {
						case '-':
							c++;

							while (*c >= '0' && *c <= '9')
								bitend = (bitend * 10) + *c++ - '0';

							if (!bitend)
								return 0;

							switch (*c) {
							case 'r':
							{
								unsigned int reg;

								reg = opc >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;
								cp += sprintf(cp, "%s", arm_regnames[reg]);

								DCC_LOG1(LOG_MSG, "regs = %s", 
										 arm_regnames[reg]);
							}
								break;
							case 'd':
							{
								unsigned int reg;

								reg = opc >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;

								 cp += sprintf(cp, "%d", reg);
							}
								break;
							case 'x':
							{
								unsigned int reg;

								reg = opc >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;

								cp += sprintf(cp, "0x%08x", reg);

								/* Some SWI instructions have special
								   meanings.  */
								if ((opc & 0x0fffffff) == 0x0FF00000)
									 cp += sprintf(cp, "\t; IMB");
								else 
									if ((opc & 0x0fffffff) == 0x0FF00001)
										 cp += sprintf(cp, "\t; IMBRange");
							}
								break;
							case 'X':
							{
								unsigned int reg;

								reg = opc >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;

								 cp += sprintf(cp, "%01x", reg & 0xf);
							}
								break;
							case 'f':
							{
								unsigned int reg;

								reg = opc >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;

								if (reg > 7)
									 cp += sprintf(cp, "#%s", 
												   arm_fp_const[reg & 7]);
								else
									 cp += sprintf(cp, "f%d", reg);
							}
								break;
							default:
								DCC_LOG(LOG_MSG, "unmatch 3");
								return 0;
							}
							break;

						case '`':
							c++;
							if ((opc & (1 << bitstart)) == 0) { 
								*cp++ = *c;
							}
							break;
						case '\'':
							c++;
							if ((opc & (1 << bitstart)) != 0) { 
								*cp++ = *c;
							}
							break;
						case '?':
							++c;
							if ((opc & (1 << bitstart)) != 0) { 
								*cp++ = *c++;
							}
							else { 
								*cp++ = *++c;
							}
							break;
						default:
							DCC_LOG(LOG_MSG, "unmatch 2");
							return 0;
						}
						break;

					default:
						DCC_LOG(LOG_MSG, "unmatch 1");
						return 0;
					}
					}
				} else { 
					*cp++ = *c;
				}
			}
			*cp = '\0';
			return cp - s;
		}
	}
	return 0;
}

/* Print one instruction from PC 
   Return the size of the instruction. */
int thumb_fmt_insn(char * s, unsigned int pc, unsigned int opc)
{
	struct thumb_opcode *insn;
	char * cp = s;

	for (insn = (struct thumb_opcode *)thumb_opcodes; insn->assembler; insn++) {
		if ((opc & insn->mask) == insn->value) {
			char * c;
	
			c = (char *)insn->assembler;

			/* Special processing for Thumb 2 instruction BL sequence:  */
			if (!*c) {	/* Check for empty (not NULL) assembler string.  */
				unsigned int offset;

				offset = BDISP23(opc);

				if ((opc & 0x10000000) == 0) {
					cp += sprintf(cp, "blx\t");

					/* The spec says that bit 1 of the branch's destination
					   address comes from bit 1 of the instruction's
					   address and not from the offset in the instruction.  */
					if (offset & 0x1) {
						/* func (stream, "*malformed!* "); */
						offset &= ~0x1;
					}

					offset |= ((pc & 0x2) >> 1);
				} else
					cp += sprintf(cp, "bl\t");

				cp += fmt_addr(cp, offset * 2 + pc + 4);
				return cp - s;
			} else {

				opc &= 0xffff;

				for (; *c; c++) {
					if (*c == '%') {
						int domaskpc = 0;
						int domasklr = 0;

						switch (*++c) {
						case '%':
							*cp++ = '%';
							break;

						case 'S':
						{
							unsigned int reg;

							reg = (opc >> 3) & 0x7;
							if (opc & (1 << 6))
								reg += 8;

							cp += sprintf(cp, "%s", arm_regnames[reg]);
						}
							break;

						case 'D':
						{
							unsigned int reg;

							reg = opc & 0x7;
							if (opc & (1 << 7))
								reg += 8;

							cp += sprintf(cp, "%s", arm_regnames[reg]);
						}
							break;

						case 'T':
							cp += sprintf(cp, "%s",
								 arm_conditional[(opc >> 8) & 0xf]);
							break;

						case 'N':
							if (opc & (1 << 8))
								domasklr = 1;
							/* Fall through.  */
						case 'O':
							if (*c == 'O' && (opc & (1 << 8)))
								domaskpc = 1;
							/* Fall through.  */
						case 'M':
						{
							int started = 0;
							int reg;

							*cp++ = '{';

							/* It would be nice if we could spot
							   ranges, and generate the rS-rE format: */
							for (reg = 0; (reg < 8); reg++)
								if ((opc & (1 << reg)) != 0) {
									if (started) {
										*cp++ = ',';
										*cp++ = ' ';
									}
									started = 1;
									cp += sprintf(cp, "%s", arm_regnames[reg]);
								}

							if (domasklr) {
								if (started) {
									*cp++ = ',';
									*cp++ = ' ';
								}
								started = 1;
								cp += sprintf(cp, arm_regnames[14] /* "lr" */ );
							}

							if (domaskpc) {
								if (started) {
									*cp++ = ',';
									*cp++ = ' ';
								}
								cp += sprintf(cp, arm_regnames[15] /* "pc" */ );
							}

							*cp++ = '}';
						}
							break;


						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
						{
							int bitstart = *c++ - '0';
							int bitend = 0;

							while (*c >= '0' && *c <= '9')
								bitstart = (bitstart * 10) + *c++ - '0';

							switch (*c) {
							case '-':
							{
								unsigned int reg;

								c++;
								while (*c >= '0' && *c <= '9')
									bitend = (bitend * 10) + *c++ - '0';
								if (!bitend)
									return 0;
								reg = opc >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;
								switch (*c) {
								case 'r':
									cp += sprintf(cp, "%s", arm_regnames[reg]);
									break;

								case 'd':
									cp += sprintf(cp, "%d", reg);
									break;

								case 'H':
									cp += sprintf(cp, "%d", reg << 1);
									break;

								case 'W':
									cp += sprintf(cp, "%d", reg << 2);
									break;

								case 'a':
									/* PC-relative address -- the bottom two
									   bits of the address are dropped
									   before the calculation.  */
									cp += fmt_addr(cp, ((pc + 4) & ~3) + 
												  (reg << 2));
									break;

								case 'x':
									cp += sprintf(cp, "0x%04x", reg);
									break;

								case 'I':
									reg =
										((reg ^ (1 << bitend)) -
										 (1 << bitend));
									cp += sprintf(cp, "%d", reg);
									break;

								case 'B':
									reg = ((reg ^ (1 << bitend)) -
										 (1 << bitend));
									cp += fmt_addr(cp, reg * 2 + pc + 4);
									break;

								default:
									return 0;
								}
							}
								break;

							case '\'':
								c++;
								if ((opc & (1 << bitstart)) != 0) { 
									*cp++ = *c; 
								}
								break;

							case '?':
								++c;
								if ((opc & (1 << bitstart)) != 0) {
									*cp++ = *c++; 
								}
								else { 
									*cp++ = *++c; 
								}
								break;

							default:
								return 0;
							}
						}
							break;

						default:
							return 0;
						}
					} else { 
						*cp++ = *c;
					}
				}
			}
			*cp = '\0';
			return cp - s;
		}
	}

	/* No match.  */
	return 0;
}

