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
 * @file target_db.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdlib.h>
#include <string.h>

#include "target.h"
#include "target/arm.h"
#include "target/a7e10x2.h"
#include "target/at91sam7.h"
#include "target/at91sam9.h"
#include "target/jtagtool3.h"

#include "dbglog.h"
#include <stdio.h>

struct target_info * target_lookup(const char * s)
{
	int i;

	for (i = 0; (target_db[i] != NULL); i++) {
		DCC_LOG1(LOG_MSG, "'%s'", target_db[i]->name);
		if (strcmp(s, target_db[i]->name) == 0) {
			return (struct target_info *)target_db[i];
		}
	}

	return NULL;
}

struct target_info * target_first(void)
{
	return (struct target_info *)target_db[0];
}

struct target_info * target_next(struct target_info * tp)
{
	int i;

	if (tp == NULL)
		return NULL;

	for (i = 0; tp != target_db[i]; i++);

	return (struct target_info *)target_db[i + 1];
}

int target_sym_lookup(const struct target_info * target, const char * name, 
					  struct target_sym ** sp)
{
	struct target_sym * sym;

	if (target == NULL)
		return -1;

	if ((sym = (struct target_sym *)target->arch->cpu->sym) != NULL) {
		while (sym->name[0] != '\0' ) {
			if (strcasecmp(sym->name, name) == 0) {
				if (sp != NULL)
					*sp = sym;
				return 0;
			}
			sym++;
		}
	}

	if ((sym = (struct target_sym *)target->arch->sym) != NULL) {
		while (sym->name[0] != '\0' ) {
			if (strcasecmp(sym->name, name) == 0) {
				if (sp != NULL)
					*sp = sym;
				return 0;
			}
			sym++;
		}
	}

	return -1;
}

