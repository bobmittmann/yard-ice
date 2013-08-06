/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file strstr.c
 * @brief YARD-ICE libc
 * @author Carlos Augusto Vieira e Vieira <carlos.vieira@boreste.com>
 */ 

#include <string.h>
#include <stdlib.h>

char * strstr(const char * haystack, const char * needle)
{
	char * n_begin;	/* where needle probably begins in haystack. */
	size_t n_len;	/* needle lengh. */

	if (!haystack || !needle)
		return NULL;

	if (!(*needle))
		return (char *) haystack;
	/* no non-empty needle can be found on empy haystack. */
	else if (!(*haystack))
		return NULL;

	n_len = strlen(needle);
	for (n_begin = (char *) haystack; *n_begin; ++n_begin) {
		/* search next probable instance of needle into haystack. */
		if ((n_begin = strchr(n_begin, (int) *needle)) == NULL)
			break;
		/* Let's test if the next n_len of haystack match with needle. */	
		if (strncmp(needle, n_begin, n_len) == 0)
			return n_begin;
	}
	return NULL;
}

