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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <hexdump.h>

#include <arpa/inet.h>

#include "config.h"
#include "target.h"
#include "debugger.h"

int show_asm(char * s, const struct ice_drv * drv, 
			 ice_context_t * ct, ice_insn_t * insn)
{
#if 0
	struct arm7context * arm7ct;
	struct arm7insn * arm7insn;
	int n;

	arm7ct = (struct arm7context *)ct;
	arm7insn = (struct arm7insn *)&arm7ct->insn;

	if (arm7insn->thumb) {
		if ((arm7insn->opc & 0xf800) == 0xf000) {
			n = sprintf(s, "%08x:\t%08x\t", arm7insn->pc & ~1, 
						  arm7insn->opc);
		} else {
			n = sprintf(s, "%08x:\t%04x\t\t", arm7insn->pc & ~1, 
						  arm7insn->opc);
		}
	} else {
		n = sprintf(s, "%08x:\t%08x\t", arm7insn->pc, arm7insn->opc);
	}

	return n + drv->insn_fmt(&s[n], ct, insn);
#endif
return 0;
}

