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
 * @file microjs-stdlib.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


/*****************************************************************************
 * MicroJS Runtime header file
 *****************************************************************************/

#ifndef __MICROJS_STDLIB_H__
#define __MICROJS_STDLIB_H__

#include <microjs-rt.h>

/* --------------------------------------------------------------------------
   Standard Library
   -------------------------------------------------------------------------- */

#define EXT_RAND    0
#define EXT_SQRT    1
#define EXT_LOG2    2
#define EXT_WRITE   3
#define EXT_TIME    4
#define EXT_SRAND   5
#define EXT_PRINT   6
#define EXT_PRINTF  7
#define EXT_MEMRD	8

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_STDLIB_H__ */

