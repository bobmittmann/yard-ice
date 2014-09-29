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

#define EXT_PRINTF          0
#define EXT_RAND            1
#define EXT_SQRT            2
#define EXT_LOG2            3
#define EXT_MEMRD	        4
#define EXT_PRINT	        5

#define EXT_MODEL_NAME      6
#define EXT_MODULE          10
#define EXT_SENSOR          11
#define EXT_GROUP           12
#define EXT_LED				13

#define EXT_LED_ON			14
#define EXT_LED_FLASH		15
#define EXT_GRP_CLEAR    	16
#define EXT_GRP_INSERT      17
#define EXT_GRP_REMOVE      18
#define EXT_GRP_BELONG      19

#define EXT_DEV_STATE       20
#define EXT_DEV_MODEL       21
#define EXT_DEV_ADDR        22
#define EXT_DEV_IS_MODULE   23
#define EXT_DEV_AP          24
#define EXT_DEV_EN          25
#define EXT_DEV_CFG         26
#define EXT_DEV_TST         27
#define EXT_DEV_TBIAS       28
#define EXT_DEV_ILAT        29
#define EXT_DEV_IMODE       30
#define EXT_DEV_IRATE       31
#define EXT_DEV_IPRE        32
#define EXT_DEV_ALARM       33
#define EXT_DEV_TROUBLE     34
#define EXT_DEV_LEVEL       35
#define EXT_DEV_OUT1        36
#define EXT_DEV_OUT2        37
#define EXT_DEV_OUT3        38
#define EXT_DEV_OUT5        39
#define EXT_DEV_PW1         40
#define EXT_DEV_PW2         41
#define EXT_DEV_PW3         42
#define EXT_DEV_PW4         43
#define EXT_DEV_PW5         44
#define EXT_DEV_GRP_CLEAR   45

#define EXCEPT_BAD_ADDR                100
#define EXCEPT_INVALID_TROUBLE_CODE    101
#define EXCEPT_INVALID_ALARM_CODE      102
#define EXCEPT_INVALID_LEVEL_VARIABLE  103
#define EXCEPT_INVALID_LEVEL_VALUE     104
#define EXCEPT_INVALID_PW_VALUE        105
#define EXCEPT_INVALID_VALUE           106
#define EXCEPT_TOO_MANY_GROUPS         107
#define EXCEPT_INVALID_GROUP           108
#define EXCEPT_INVALID_DEVICE          109
#define EXCEPT_INVALID_LED             110
#define EXCEPT_MISSING_ARGUMENT        113

#define CLASS_DEV 0
#define CLASS_GRP 1
#define CLASS_LED 2

#ifdef __SLCDEV_LIB_DEF__

const struct ext_classtab test_classtab = {
	.ccnt = 3,
	.cdef = {
		[CLASS_DEV] = { .nm = "dev", 
			.fst = EXT_DEV_STATE, .lst = EXT_DEV_GRP_CLEAR },
		[CLASS_GRP] = { .nm = "grp", 
			.fst = EXT_GRP_CLEAR, .lst = EXT_GRP_BELONG },
		[CLASS_LED] = { .nm = "led", 
			.fst = EXT_LED_ON, .lst = EXT_LED_FLASH },
	}
};

struct ext_libdef slcdev_lib = {
	.name = "lib",
	.classtab = &test_classtab,
	.xcnt = 46,
	.xdef = {
		[EXT_PRINTF] = { .opt = O_FUNCTION,  
			.nm = "printf", 
			.f = { .argmin = 1, .argmax = 32, .ret = 0 } },
		[EXT_PRINT] = { .opt = O_FUNCTION,  
			.nm = "print", 
			.f = { .argmin = 0, .argmax = 32, .ret = 0 } },
		[EXT_RAND] = { .opt = O_FUNCTION, 
			.nm = "rand", 
			.f = { .argmin = 0, .argmax = 0, .ret = 1 } },
		[EXT_SQRT] = { .opt = O_FUNCTION,  
			.nm = "sqrt", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },
		[EXT_LOG2] = { .opt = O_FUNCTION,  
			.nm = "log2", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },
		[EXT_MEMRD] = { .opt = O_FUNCTION,  
			.nm = "memrd", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },

		[EXT_MODEL_NAME] = { .opt = O_FUNCTION,  
			.nm = "model_name", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },

		[EXT_SENSOR] = { .opt = O_ARRAY | O_OBJECT, 
			.nm = "sensor", 
			.aos = { .cdef = CLASS_DEV, .size = 1, .offs = 0 } },

		[EXT_MODULE] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "module", 
			.aos = { .cdef = CLASS_DEV, .size = 1, .offs = 160 } },

		[EXT_GROUP] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "group", 
			.aos = { .cdef = CLASS_GRP, .size = 1, .offs = 0 } },

		[EXT_LED] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "led", 
			.aos = { .cdef = CLASS_LED, .size = 1, .offs = 0 } },

		[EXT_LED_ON] = { .opt = O_INTEGER | O_MEMBER, 
			.nm = "on" },

		[EXT_LED_FLASH] = { .opt = O_FUNCTION | O_MEMBER, 
			.nm = "flash" ,
			.f = { .argmin = 2, .argmax = 2, .ret = 0 } },

		/* device class members */
		[EXT_DEV_STATE] = { .opt = O_INTEGER | O_MEMBER | O_READONLY, 
			.nm = "state" },
		[EXT_DEV_MODEL] = { .opt = O_INTEGER | O_MEMBER | O_READONLY, 
			.nm = "model" },
		[EXT_DEV_ADDR] = { .opt = O_INTEGER | O_MEMBER | O_READONLY, 
			.nm = "addr" },
		[EXT_DEV_IS_MODULE] = { .opt = O_INTEGER | O_MEMBER | O_READONLY, 
			.nm = "is_module" },
		[EXT_DEV_AP] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "ap" },
		[EXT_DEV_EN] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "en" },
		[EXT_DEV_CFG] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "cfg" },
		[EXT_DEV_TST] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "tst" },
		[EXT_DEV_TBIAS] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "tbias" },
		[EXT_DEV_ILAT] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "ilat" },
		[EXT_DEV_IMODE] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "imode" },
		[EXT_DEV_IRATE] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "irate" },
		[EXT_DEV_IPRE] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "ipre" },
		[EXT_DEV_ALARM] = { .opt = O_INTEGER | O_MEMBER,  
			.nm = "alarm" },
		[EXT_DEV_TROUBLE] = { .opt = O_INTEGER | O_MEMBER,  
			.nm = "trouble" },
		[EXT_DEV_LEVEL] = { .opt = O_INTEGER | O_MEMBER | O_ARRAY,
			.nm = "level" },
		[EXT_DEV_OUT1] = {.opt = O_INTEGER | O_MEMBER, 
			.nm = "out1" },
		[EXT_DEV_OUT2] = {.opt = O_INTEGER | O_MEMBER, 
			.nm = "out2" },
		[EXT_DEV_OUT3] = {.opt = O_INTEGER | O_MEMBER, 
			.nm = "out3" },
		[EXT_DEV_OUT5] = {.opt = O_INTEGER | O_MEMBER, 
			.nm = "out5" },
		[EXT_DEV_PW1] = {.opt = O_INTEGER | O_MEMBER, 
			.nm = "pw1" },
		[EXT_DEV_PW2] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "pw2" },
		[EXT_DEV_PW3] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "pw3" },
		[EXT_DEV_PW4] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "pw4" },
		[EXT_DEV_PW5] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "pw5" },
		[EXT_DEV_GRP_CLEAR] = { .opt = O_FUNCTION | O_MEMBER,  
			.nm = "grp_clear", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },

		[EXT_GRP_CLEAR] = { .opt = O_FUNCTION | O_MEMBER,
			.nm = "clear",
			.f = { .argmin = 1, .argmax = 1, .ret = 0 } },
		[EXT_GRP_INSERT] = { .opt = O_FUNCTION | O_MEMBER,
			.nm = "insert",
			.f = { .argmin = 2, .argmax = 2, .ret = 0 } },
		[EXT_GRP_REMOVE] = { .opt = O_FUNCTION | O_MEMBER,
			.nm = "remove",
			.f = { .argmin = 2, .argmax = 2, .ret = 0 } },
		[EXT_GRP_BELONG] = {  .opt = O_FUNCTION | O_MEMBER,
			.nm = "belong",
			.f = { .argmin = 2, .argmax = 2, .ret = 1 } },
	}
};

#else

extern struct ext_libdef slcdev_lib;

#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __TEST_LIB_H__ */

