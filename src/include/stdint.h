/* 
 * File:	stdint.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __STDINT_H__
#define __STDINT_H__

/* Exact integral types.  */

/* Signed.  */
typedef signed char     int8_t;
typedef short int       int16_t;
typedef int             int32_t;
typedef long long int   int64_t;

/* Unsigned.  */
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long int  uint64_t;

/* Types for `void *' pointers.  */
typedef int                 intptr_t;
typedef unsigned int        uintptr_t;

/* Limits of integral types.  */

/* Minimum of signed integral types.  */
# define INT8_MIN  (-128)
# define INT16_MIN (-32767-1)
# define INT32_MIN (-2147483647-1)
# define INT64_MIN (-9223372036854775807LL-1)

/* Maximum of signed integral types.  */
# define INT8_MAX  (127)
# define INT16_MAX (32767)
# define INT32_MAX (2147483647)
# define INT64_MAX (9223372036854775807LL)

/* Maximum of unsigned integral types.  */
# define UINT8_MAX  (255)
# define UINT16_MAX (65535)
# define UINT32_MAX (4294967295U)
# define UINT64_MAX (18446744073709551615ULL)

#endif	/* __STDINT_H__ */

