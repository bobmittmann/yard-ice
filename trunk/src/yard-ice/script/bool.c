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
#include "val.h"

int val_bool_encode(value_t * val, const char * s)
{
	char * endptr;
	unsigned long n;

	n = strtoul(s, &endptr, 0);
	
	if (endptr != s) {
		val->int32 = (n == 0) ? 0 : 1;
		return 0;
	}

	if ((strcasecmp(s, "true") == 0) || (strcasecmp(s, "yes") == 0)) {
		val->int32 = 1;
		return 0;
	}

	if ((strcasecmp(s, "false") == 0) || (strcasecmp(s, "no") == 0)) {
		val->int32 = 0;
		return 0;
	}

	return -1;
}

int val_bool_decode(const value_t * val, char * s)
{
	return sprintf(s, "%s", val->int32 ? "TRUE" : "FALSE");
}

struct type_def type_def_bool = {
	.name = "bool",
	.encode = (val_encode_t)val_bool_encode,
	.decode = (val_decode_t)val_bool_decode
};

