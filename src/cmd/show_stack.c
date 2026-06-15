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

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "hexdump.h"

int target_stack_show(FILE * f)
{
	struct debugger * dbg = &debugger;
	unsigned int addr;
	uint32_t buf[16];
	uint32_t * sp;
	int cnt;
	int ret;

	if ((ret = target_stack_refresh(buf, sizeof(buf))) < 0)
		return ret;

	addr = dbg->stack.base;
	cnt = dbg->stack.size;
	sp = (uint32_t *)buf;

	addr &= ~(sizeof(uint32_t) - 1);
	show_hex32(f, addr, sp, cnt);

	return 0;
}

