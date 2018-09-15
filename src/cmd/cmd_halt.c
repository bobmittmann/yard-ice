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
 * @file cmd_halt.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "command.h"

int show_context(FILE * f, int which);
int show_stack(FILE * f);

int cmd_halt(FILE * f, int argc, char ** argv)
{
//	struct debugger * dbg = &debugger;
	int force = 0;
	int err;

	if (argc > 2) {
//		fprintf(f, msg_halt_usage);
		return -1;
	}

	if (argc > 1) {
		force = *argv[1] == 'f' ? 1 : 0;
	}

	if ((err = target_halt(force)) < 0) {
		fprintf(f, "#ERROR: target halt: %s\n", target_strerror(err));
		return err;
	}
#if ENABLE_HALT_SHOW_CONTEXT
	if ((err = target_show_context(f)) < 0) {
		fprintf(f, "#ERROR: target_show_context(): %s\n", target_strerror(err));
		return err;
	}
#endif
//	show_context(f, 0);
//	show_stack(f);

	return 0;
}

