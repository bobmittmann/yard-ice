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
 * @file microjs-rt.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


/*****************************************************************************
 * MicroJS Runtime header file
 *****************************************************************************/

#ifndef __TEST_LIB_H__
#define __TEST_LIB_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <microjs.h>
#include <microjs-rt.h>

#define EXT_RAND    0
#define EXT_SQRT    1
#define EXT_LOG2    2
#define EXT_WRITE   3
#define EXT_TIME    4
#define EXT_SRAND   5
#define EXT_PRINT   6
#define EXT_PRINTF  7
#define EXT_MEMRD	8
#define EXT_SENS_STATE   9
#define EXT_SENS_ALARM   10
#define EXT_SENS_TROUBLE 11
#define EXT_MOD_STATE    12
#define EXT_MOD_ALARM    13
#define EXT_MOD_TROUBLE  14


#define EXCEPT_BAD_ADDR              100
#define EXCEPT_INVALID_TROUBLE_CODE  101
#define EXCEPT_INVALID_ALARM_CODE    102

#ifdef __TEST_LIB_DEF__

struct ext_libdef test_lib = {
	.name = "lib",
	.fncnt = 15,
	.fndef = {
		[EXT_RAND] = { .nm = "rand", .argmin = 0, .argmax = 0, .ret = 1 },
		[EXT_SRAND] = { .nm = "srand", .argmin = 1, .argmax = 1, .ret = 0 },
		[EXT_TIME] = { .nm = "time", .argmin = 0, .argmax = 0, .ret = 1 },
		[EXT_SQRT] = { .nm = "sqrt", .argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_LOG2] = { .nm = "log2", .argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_WRITE] = { .nm = "write", .argmin = 0, .argmax = 32, .ret = 0 },
		[EXT_PRINT] = { .nm = "print", .argmin = 0, .argmax = 32, .ret = 0 },
		[EXT_PRINTF] = { .nm = "printf", .argmin = 1, .argmax = 32, .ret = 0 },
		[EXT_MEMRD] = { .nm = "memrd", .argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_SENS_STATE] = { .nm = "sens_state", 
			.argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_SENS_ALARM] = { .nm = "sens_alarm", 
			.argmin = 2, .argmax = 2, .ret = 0 },
		[EXT_SENS_TROUBLE] = { .nm = "sens_trouble",
			.argmin = 2, .argmax = 2, .ret = 0 },
		[EXT_MOD_STATE] = { .nm = "mod_state",
			.argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_MOD_ALARM] = { .nm = "mod_alarm",
			.argmin = 2, .argmax = 2, .ret = 0 },
		[EXT_MOD_TROUBLE] = { .nm = "mod_trouble",
			.argmin = 2, .argmax = 2, .ret = 0 },
	}
};

#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __TEST_LIB_H__ */

