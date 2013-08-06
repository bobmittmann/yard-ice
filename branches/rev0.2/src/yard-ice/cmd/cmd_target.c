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
 * @file target.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "eval.h"

#include "dbglog.h" 

static struct target_info * target_select_tool(FILE *f)
{
	struct target_info * target;
	unsigned int i;
	unsigned int x;
	char s[64];
	int n = 0;

	fprintf(f, "Target configuration utility\n\n");

	if ((target = get_target_info()) != NULL) {
		strcpy(s, target->name);
	}

	target = target_first();
	fprintf(f, "      | target:         |      arch name: | cpu family: "
			"| cpu vendor: |\n");
	fprintf(f, "  ----+-----------------+-----------------+-------------"
			"+-------------+\n");
	for (i = 0; target != NULL; i++) {
		if (strcmp(target->name, s) == 0) {
			fprintf(f, "-> ");
			n = i;
		} else {
			fprintf(f, "   ");
		}
		fprintf(f, "%2d | %-15s | %15s | %11s | %11s |\n", i, 
			target->name, target->arch->name, 
			target->arch->cpu->family, target->arch->cpu->vendor);
		target = target_next(target);
	}

	fprintf(f, "  ----+-----------------+-----------------+-------------"
			"+-------------+\n");
	fprintf(f, "Target(%d): ", n); 
	fgets(s, 4, f);

	x = strtoul(s, NULL, 0);

	if ((s[0] == '\0') || (s[0] == '\n') || (x == n) || (x > i)) {
		fprintf(f, "\nKeeping current configuration.\n");
		return get_target_info();
	}

	target = target_first();
	for (i = 0; (target != NULL); i++) {
		if (x == i) {
			fprintf(f, " New target: '%s'\n", target->name);
			setenv("TARGET", target->name, 1);
			break;
		}
		target = target_next(target);
	}

	return target;
}

int cmd_target(FILE *f, int argc, char ** argv)
{
	struct target_info * target;
	int force = 0;
	int probe = 0;
	int scan = 0;
	value_t val;
	int ret;
	int i;
	int n;

	argc--;
	argv++;

	if (argc == 0) {
		target = target_select_tool(f);
	} else {
		if ((target = target_lookup(*argv)) != NULL) {
			DCC_LOG(LOG_TRACE, "target_lookup() success.");
			argc--;
			argv++;
		} else {
			if ((n = eval_uint32(&val, argc, argv)) < 0) {
				fprintf(f, "usage: target <NAME | ID> <force>\n");
				return n;
			}

			argc -= n;
			argv += n;
			n = val.uint32;

			DCC_LOG(LOG_TRACE, "searching the target list...");

			target = target_first();
			for (i = 0; (i < n) && (target != NULL); i++)
				target = target_next(target);
		} 

		while (argc) {
			if ((strcmp(*argv, "force") == 0) || 
				(strcmp(*argv, "f") == 0)) {
				force = 1;
			} else {
				if ((strcmp(*argv, "probe") == 0) 
					|| (strcmp(*argv, "p") == 0)) {
					probe = 1;
				} else {
					if ((strcmp(*argv, "scan") == 0) 
						|| (strcmp(*argv, "s") == 0)) {
						scan = 1;
					} else {
						fprintf(f, "Invalid argument: %s...\n", *argv);
						return -1;
					}
				}
			}
			argc--;
			argv++;
		}

	}

	/* FIXME */
	(void)scan;

	if (target == NULL) {
		fprintf(f, "invalid target...\n");
		return -1;
	}

	fprintf(f, " - Target: '%s'\n", target->name);

	if (target_configure(f, target, force) < 0) {
		fprintf(f, "ERROR: target_configure()!\n");
		return -1;
	}

	if (probe) {
		if ((ret = target_probe(f)) < 0) {
			fprintf(f, " # probe ERROR!\n");
			return ret;
		} 
		if(ret == 0) {
			fprintf(f, " - probe Fail!\n");
		} else {
			fprintf(f, " - probe Ok.\n");
		}
	} 

	return 0;
}

