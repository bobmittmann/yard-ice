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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __LOOKUP_H__
#define __LOOKUP_H__

#include <stdint.h> 

/* lookup table entry */

struct lt_entry_int {
	const char * tag;
	int val;
};

typedef struct lt_entry_int lt_entry_int_t;

#ifdef __cplusplus
extern "C" {
#endif

int lookup_int_val(const lt_entry_int_t * tab, const char * tag, int * val);

#ifdef __cplusplus
}
#endif

#endif /* __LOOKUP_H__ */


