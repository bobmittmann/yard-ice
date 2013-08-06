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
 * @file stdint.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STDINT_H__
#define __STDINT_H__

typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long long int int64_t;

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;

typedef int intptr_t;
typedef unsigned int uintptr_t;

# define INT8_MIN  (-128)
# define INT16_MIN (-32767-1)
# define INT32_MIN (-2147483647-1)
# define INT64_MIN (-9223372036854775807LL-1)

# define INT8_MAX  (127)
# define INT16_MAX (32767)
# define INT32_MAX (2147483647)
# define INT64_MAX (9223372036854775807LL)

# define UINT8_MAX  (255)
# define UINT16_MAX (65535)
# define UINT32_MAX (4294967295U)
# define UINT64_MAX (18446744073709551615ULL)

#endif	/* __STDINT_H__ */

