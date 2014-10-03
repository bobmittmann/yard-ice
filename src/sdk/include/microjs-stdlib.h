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

#define EXT_LED          15
#define EXT_GROUP        16
#define EXT_MODULE       17
#define EXT_SENSOR       18

#define EXT_DEV_STATE    19
#define EXT_DEV_ALARM    20
#define EXT_DEV_TROUBLE  21
#define EXT_DEV_LEVEL    22

#define EXT_LED_ON       23
#define EXT_LED_FLASH    24

#define EXT_GRP_CLEAR    25
#define EXT_GRP_INSERT   26
#define EXT_GRP_REMOVE   27
#define EXT_GRP_BELONG   28



#define EXCEPT_BAD_ADDR                100
#define EXCEPT_INVALID_TROUBLE_CODE    101
#define EXCEPT_INVALID_ALARM_CODE      102
#define EXCEPT_INVALID_VALUE           106
#define EXCEPT_TOO_MANY_GROUPS         107
#define EXCEPT_INVALID_GROUP           108
#define EXCEPT_INVALID_DEVICE          109
#define EXCEPT_INVALID_LED             110
#define EXCEPT_INVALID_LEVEL           111

#define CLASS_DEV 0
#define CLASS_GRP 1
#define CLASS_LED 2

#ifdef __MICROJS_LIB_DEF__

#define CLASS_DEV 0

const struct ext_classtab microjs_class = {
	.ccnt = 3,
	.cdef = {
		[CLASS_DEV] = { .nm = "dev", 
			.first = EXT_DEV_STATE, .last = EXT_DEV_LEVEL },
		[CLASS_GRP] = { .nm = "grp", 
			.first = EXT_GRP_CLEAR, .last = EXT_GRP_BELONG },
		[CLASS_LED] = { .nm = "led", 
			.first = EXT_LED_ON, .last = EXT_LED_FLASH },
	}
};

const struct ext_libdef microjs_lib = {
	.name = "lib",
	.classtab = &microjs_class,
	.xcnt = 25,
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

		[EXT_GROUP] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "group", 
			.aos = { .cdef = CLASS_GRP, .size = 1, .offs = 0 } },

		[EXT_LED] = { .opt = O_ARRAY | O_OBJECT | O_SIZEOFFS, 
			.nm = "led", 
			.aos = { .cdef = CLASS_LED, .size = 1, .offs = 0 } },

		/* device class members */
		[EXT_DEV_STATE] = { .opt = O_INTEGER | O_MEMBER | O_READONLY, 
			.nm = "state" },
		[EXT_DEV_ALARM] = { .opt = O_INTEGER | O_MEMBER,  
			.nm = "alarm" },
		[EXT_DEV_TROUBLE] = { .opt = O_INTEGER | O_MEMBER,  
			.nm = "trouble" },
		[EXT_DEV_LEVEL] = { .opt = O_ARRAY | O_INTEGER | O_MEMBER,  
			.nm = "level" },

		[EXT_LED_ON] = { .opt = O_INTEGER | O_MEMBER, 
			.nm = "on" },
		[EXT_LED_FLASH] = { .opt = O_FUNCTION | O_MEMBER, 
			.nm = "flash" ,
			.f = { .argmin = 2, .argmax = 2, .ret = 0 } },

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

#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_STDLIB_H__ */

