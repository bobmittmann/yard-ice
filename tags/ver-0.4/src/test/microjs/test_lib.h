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

#define EXT_RAND         0
#define EXT_SQRT         1
#define EXT_LOG2         2
#define EXT_WRITE        3
#define EXT_TIME         4
#define EXT_SRAND        5
#define EXT_PRINT        6
#define EXT_PRINTF       7
#define EXT_MEMRD	     8
#define EXT_SENS_STATE   9
#define EXT_SENS_ALARM   10
#define EXT_SENS_TROUBLE 11
#define EXT_MOD_STATE    12
#define EXT_MOD_ALARM    13
#define EXT_MOD_TROUBLE  14
#define EXT_MODULE       15
#define EXT_SENSOR       16
#define EXT_DEV_STATE    17
#define EXT_DEV_ALARM    18
#define EXT_DEV_TROUBLE  19

#define EXCEPT_BAD_ADDR              100
#define EXCEPT_INVALID_TROUBLE_CODE  101
#define EXCEPT_INVALID_ALARM_CODE    102

#define CLASS_DEV 0

#ifdef __TEST_LIB_DEF__

const struct ext_classtab test_classtab = {
	.ccnt = 1,
	.cdef = {
		[CLASS_DEV] = { .nm = "dev", 
			.fst = EXT_DEV_STATE, .lst = EXT_DEV_TROUBLE },
	}
};

const struct ext_libdef test_lib = {
	.name = "lib",
	.classtab = &test_classtab,
	.xcnt = 20,
	.xdef = {
		[EXT_RAND] = { .opt = O_FUNCTION, 
			.nm = "rand", 
			.f = { .argmin = 0, .argmax = 0, .ret = 1 } },
		[EXT_SRAND] = { .opt = O_FUNCTION,  
			.nm = "srand", 
			.f = { .argmin = 1, .argmax = 1, .ret = 0 } },
		[EXT_TIME] = { .opt = O_FUNCTION,  
			.nm = "time", 
			.f = { .argmin = 0, .argmax = 0, .ret = 1 } },
		[EXT_SQRT] = { .opt = O_FUNCTION,  
			.nm = "sqrt", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },
		[EXT_LOG2] = { .opt = O_FUNCTION,  
			.nm = "log2", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },
		[EXT_WRITE] = { .opt = O_FUNCTION,  
			.nm = "write", 
			.f = { .argmin = 0, .argmax = 32, .ret = 0 } },
		[EXT_PRINT] = { .opt = O_FUNCTION,  
			.nm = "print", 
			.f = { .argmin = 0, .argmax = 32, .ret = 0 } },
		[EXT_PRINTF] = { .opt = O_FUNCTION,  
			.nm = "printf", 
			.f = { .argmin = 1, .argmax = 32, .ret = 0 } },
		[EXT_MEMRD] = { .opt = O_FUNCTION,  
			.nm = "memrd", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },

		[EXT_SENS_STATE] = { .opt = O_FUNCTION,  
			.nm = "sens_state", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },
		[EXT_SENS_ALARM] = { .opt = O_FUNCTION, 
			.nm = "sens_alarm", 
			.f = { .argmin = 2, .argmax = 2, .ret = 0 } },
		[EXT_SENS_TROUBLE] = { .opt = O_FUNCTION,  
			.nm = "sens_trouble", 
			.f = { .argmin = 2, .argmax = 2, .ret = 0 } },

		[EXT_MOD_STATE] = { .opt = O_FUNCTION,  
			.nm = "mod_state", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },
		[EXT_MOD_ALARM] = { .opt = O_FUNCTION,  
			.nm = "mod_alarm", 
			.f = { .argmin = 2, .argmax = 2, .ret = 0 } },
		[EXT_MOD_TROUBLE] = { .opt = O_FUNCTION,  
			.nm = "mod_trouble", 
			.f = { .argmin = 2, .argmax = 2, .ret = 0 } },

		[EXT_SENSOR] = { .opt = O_ARRAY | O_OBJECT, 
			.nm = "sensor", 
			.aos = { .cdef = CLASS_DEV, .size = 1, .offs = 0 } },

		[EXT_MODULE] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "module", 
			.aos = { .cdef = CLASS_DEV, .size = 1, .offs = 160 } },

		/* device class members */
		[EXT_DEV_STATE] = { .opt = O_INTEGER | O_MEMBER | O_READONLY, 
			.nm = "state" },
		[EXT_DEV_ALARM] = { .opt = O_INTEGER | O_MEMBER,  
			.nm = "alarm" },
		[EXT_DEV_TROUBLE] = { .opt = O_INTEGER | O_MEMBER,  
			.nm = "trouble" },
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

