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
 * @file tag.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "val.h"

int val_tag_encode(value_t * val, const char * s)
{
	val->uint64 = 0;
	strncpy(val->tag, s, 8);
	
	return 0;
}

int val_tag_decode(const value_t * val, char * s)
{
	strncpy(s, val->tag, 8);
	s[8] = '\0';

	return 0;
}

const struct type_def type_def_tag = {
	.name = "tag",
	.encode = (val_encode_t)val_tag_encode,
	.decode = (val_decode_t)val_tag_decode
};

