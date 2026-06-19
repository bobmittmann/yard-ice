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

#define TRACE_LEVEL TRACE_LVL_DBG

#include <trace.h>

int dict_lookup_linear(const struct dict_entry tab[], const char * tag)
{
	const struct dict_entry * lp;
	int i = 0;

	for (i = 0; ; i++) {
		lp = &tab[i];

		if (lp->key == NULL)
			return -1;

		if (strcmp(lp->key, tag) == 0) {
			break;
		}
	}

	return lp->val;
}

int str_bsearch(const char * lst[], int len, const char * key)
{
	int i = 0;
    int j = len - 1;

    /* Binary search */
    while (i <= j) {
        int k = i + ((j - i) / 2);
        int cmp = strcmp(lst[k], key);
        if (cmp == 0) {
			return k;
        } else if (cmp < 0) {
            i = k + 1;
        } else {
            j = k - 1;
        }
    }

	return -1; 
}

/* */
int dict_bsearch(const struct dict_entry lst[], int len, const char * key)
{
	int i = 0;
    int j = len - 1;

    /* Binary search */
    while (i <= j) {
        int k = i + ((j - i) / 2);
        int cmp = strcmp(lst[k].key, key);
        if (cmp == 0) {
			return lst[k].val;
        } else if (cmp < 0) {
            i = k + 1;
        } else {
            j = k - 1;
        }
    }

	return -1; 
}

