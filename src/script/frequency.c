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
 * @file frequency.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "val.h"

int val_freq_encode(value_t * val, const char * s)
{
	char * endptr;

	val->uint32 = strtoul(s, &endptr, 0);
	
	if (endptr == s)
		return -1;

	return 0;
}

int val_freq_decode(const value_t * val, char * s)
{
	uint32_t f = val->uint32;

	if (f >= 1000000)
		return sprintf(s, "%d.%03d MHz", f / 1000000, (f % 1000000) / 1000);

	if (f >= 1000)
		return sprintf(s, "%d.%03d KHz", f / 1000, f % 1000);
	
	return sprintf(s, "%d Hz", f);
}

struct type_def type_def_freq = {
	.name = "frequency",
	.encode = (val_encode_t)val_freq_encode,
	.decode = (val_decode_t)val_freq_decode
};

