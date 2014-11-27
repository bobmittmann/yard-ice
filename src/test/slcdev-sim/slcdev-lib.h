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
#define EXT_TICKS           4
#define EXT_PRINT	        5
#define EXT_TIME            6

#define EXT_MODEL_NAME      7
#define EXT_TIMER           8
#define EXT_LED				9

#define EXT_THIS            10
#define EXT_MODULE          11
#define EXT_SENSOR          12
#define EXT_GROUP           13

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
#define EXT_DEV_LED         40
#define EXT_DEV_LEDNO       41

#define EXT_DEV_PW1         42
#define EXT_DEV_PW2         43
#define EXT_DEV_PW3         44
#define EXT_DEV_PW4         45
#define EXT_DEV_PW5         46
#define EXT_DEV_PCNT        47
#define EXT_DEV_GRP         48
#define EXT_DEV_PRINT       49
#define EXT_DEV_GRP_CLEAR   50


#define EXT_TMR_MS          51
#define EXT_TMR_SEC         52

#define EXT_TRIGGER         53
#define EXT_TRIG_ADDR       54
#define EXT_TRIG_MODULE     55
#define EXT_TRIG_SENSOR     56

#define EXT_S               57
#define EXT_M               58

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
#define EXCEPT_INVALID_BOOLEAN         111
#define EXCEPT_MISSING_ARGUMENT        113
#define EXCEPT_INVALID_TIMER           114

#define CLASS_DEV 0
#define CLASS_GRP 1
#define CLASS_LED 2
#define CLASS_TMR 3
#define CLASS_TRIG 4

#ifdef __SLCDEV_LIB_DEF__

const struct ext_classtab test_classtab = {
	.ccnt = 5,
	.cdef = {
		[CLASS_DEV] = { .nm = "dev", 
			.first = EXT_DEV_STATE, .last = EXT_DEV_GRP_CLEAR },
		[CLASS_GRP] = { .nm = "grp", 
			.first = EXT_GRP_CLEAR, .last = EXT_GRP_BELONG },
		[CLASS_LED] = { .nm = "led", 
			.first = EXT_LED_ON, .last = EXT_LED_FLASH },
		[CLASS_TMR] = { .nm = "tmr", 
			.first = EXT_TMR_MS, .last = EXT_TMR_SEC },
		[CLASS_TRIG] = { .nm = "trig", 
			.first = EXT_TRIG_ADDR, .last = EXT_TRIG_SENSOR},
	}
};

const struct ext_libdef slcdev_lib = {
	.name = "lib",
	.classtab = &test_classtab,
	.xcnt = 59,
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

		[EXT_TICKS] = { .opt = O_FUNCTION,  
			.nm = "ticks", 
			.f = { .argmin = 0, .argmax = 0, .ret = 1 } },

		[EXT_TIME] = { .opt = O_FUNCTION,  
			.nm = "time", 
			.f = { .argmin = 0, .argmax = 0, .ret = 1 } },

		[EXT_MODEL_NAME] = { .opt = O_FUNCTION,  
			.nm = "model_name", 
			.f = { .argmin = 1, .argmax = 1, .ret = 1 } },

		[EXT_THIS] = { .opt = O_OBJECT,
			.nm = "this", 
			.o = { .cdef = CLASS_DEV } },


		[EXT_SENSOR] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "sensor", 
			.aos = { .cdef = CLASS_DEV, .size = 1, .offs = 0 } },
		[EXT_S] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "s", 
			.aos = { .cdef = CLASS_DEV, .size = 1, .offs = 0 } },
		[EXT_MODULE] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "module", 
			.aos = { .cdef = CLASS_DEV, .size = 1, .offs = 160 } },
		[EXT_M] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "m", 
			.aos = { .cdef = CLASS_DEV, .size = 1, .offs = 160 } },

		[EXT_GROUP] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "group", 
			.aos = { .cdef = CLASS_GRP, .size = 1, .offs = 0 } },

		[EXT_TIMER] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "timer",
			.aos = { .cdef = CLASS_TMR, .size = 1, .offs = 0 } },
		[EXT_TMR_MS] = { .opt = O_INTEGER | O_MEMBER, 
			.nm = "ms" },
		[EXT_TMR_SEC] = { .opt = O_INTEGER | O_MEMBER, 
			.nm = "sec" },

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
		[EXT_DEV_LED] = {.opt = O_INTEGER | O_MEMBER, 
			.nm = "led" },
		[EXT_DEV_LEDNO] = {.opt = O_INTEGER | O_MEMBER, 
			.nm = "ledno" },
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
		[EXT_DEV_PCNT] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "pcnt" },
		[EXT_DEV_GRP] = { .opt = O_INTEGER | O_MEMBER | O_ARRAY,
			.nm = "grp" },
		[EXT_DEV_PRINT] = { .opt = O_FUNCTION | O_MEMBER,  
			.nm = "print", 
			.f = { .argmin = 1, .argmax = 1, .ret = 0 } },
		[EXT_DEV_GRP_CLEAR] = { .opt = O_FUNCTION | O_MEMBER,  
			.nm = "grp_clear", 
			.f = { .argmin = 1, .argmax = 1, .ret = 0 } },

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

		[EXT_TRIGGER] = { .opt = O_OBJECT | O_SINGLETON,
			.nm = "trigger", 
			.o = { .cdef = CLASS_TRIG, .inst = 0 } },
		[EXT_TRIG_ADDR] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "addr" },
		[EXT_TRIG_MODULE] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "module" },
		[EXT_TRIG_SENSOR] = { .opt = O_INTEGER | O_MEMBER,
			.nm = "sensor" },

		[EXT_LED] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "led", 
			.aos = { .cdef = CLASS_LED, .size = 1, .offs = 0 } },
		[EXT_LED_ON] = { .opt = O_INTEGER | O_MEMBER, 
			.nm = "on" },
		[EXT_LED_FLASH] = { .opt = O_FUNCTION | O_MEMBER, 
			.nm = "flash",
			.f = { .argmin = 2, .argmax = 2, .ret = 0 } },

	}
};

#else

extern const struct ext_libdef slcdev_lib;

#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __TEST_LIB_H__ */

