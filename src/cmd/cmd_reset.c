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
 * @file cmd_reset.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "lookup.h"

#include <sys/dcclog.h>

static const lt_entry_int_t rst_mode[] = {
	{ "auto", RST_AUTO },
	{ "soft", RST_SOFT },
	{ "hard", RST_HARD },
	{ "core", RST_CORE },
	{ "dbg", RST_DBG },
	{ "sys", RST_SYS },
	{ "a", RST_AUTO },
	{ "s", RST_SOFT },
	{ "h", RST_HARD },
	{ "c", RST_CORE },
	{ "d", RST_DBG },
	{ "y", RST_SYS },
	{ NULL, 0 }
};

int cmd_reset(FILE * f, int argc, char ** argv)
{
	int err;
	int mode;
	int n;

	DCC_LOG(LOG_MSG, ".");

	if (argc > 2) {
//		printf(msg_reset_usage);
		return -1;
	}

	if (argc > 1) {
		if ((n = lookup_int_val(rst_mode, argv[1], &mode)) < 0) {
			printf("ERROR: invalid mode\n");
			return -1;
		}
	} else {
		mode = RST_AUTO;
		n = 0;
	}

	fprintf(f, "Target reset: %s\n", rst_mode[n].tag);

	if ((err = target_reset(f, mode)) < 0) {
		printf("ERROR: reset: %s.\n", target_strerror(err));
	}

	return err;
}

