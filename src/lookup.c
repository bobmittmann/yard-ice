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
#include <ctype.h>

#include "lookup.h"

#include <sys/dcclog.h>

int lookup_int_val(const lt_entry_int_t * tab, const char * tag, int * val)
{
	lt_entry_int_t * lp;
	int i = 0;

	for (i = 0; ; i++) {
		lp = (lt_entry_int_t *)&tab[i];

		if (lp->tag == NULL)
			return -1;

		if (strcmp(lp->tag, tag) == 0) {
			break;
		}
	}

	if (val != NULL)
		*val = lp->val;

	return i;
}

