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

#define EXT_PRINTF       0
#define EXT_RAND         1
#define EXT_SQRT         2
#define EXT_LOG2         3
#define EXT_MEMRD	     4
#define EXT_PRINT	     5

#define EXT_MODEL_NAME   9

#define EXT_SENS_STATE   10
#define EXT_SENS_ALARM   11
#define EXT_SENS_TROUBLE 12
#define EXT_SENS_LEVEL   13
#define EXT_SENS_PW1     14
#define EXT_SENS_PW2     15
#define EXT_SENS_PW3     16
#define EXT_SENS_PW4     17
#define EXT_SENS_PW5     18
#define EXT_SENS_EN      19
#define EXT_SENS_CFG     20
#define EXT_SENS_AP      21
#define EXT_SENS_TST     22
#define EXT_SENS_TBIAS   23
#define EXT_SENS_ILAT    24
#define EXT_SENS_IMODE   25
#define EXT_SENS_IRATE   26
#define EXT_SENS_IPRE    27
#define EXT_SENS_CLEAR   28
#define EXT_SENS_INSERT  29
#define EXT_SENS_REMOVE  30
#define EXT_SENS_BELONG  31
#define EXT_SENS_MODEL   32

#define EXT_MOD_STATE    40
#define EXT_MOD_ALARM    41
#define EXT_MOD_TROUBLE  42
#define EXT_MOD_LEVEL    43
#define EXT_MOD_PW1      44
#define EXT_MOD_PW2      45
#define EXT_MOD_PW3      46
#define EXT_MOD_PW4      47
#define EXT_MOD_PW5      48
#define EXT_MOD_EN       49
#define EXT_MOD_CFG      50
#define EXT_MOD_AP       51
#define EXT_MOD_TST      52
#define EXT_MOD_TBIAS    53
#define EXT_MOD_ILAT     54
#define EXT_MOD_IMODE    55
#define EXT_MOD_IRATE    56
#define EXT_MOD_IPRE     57
#define EXT_MOD_CLEAR    58
#define EXT_MOD_INSERT   59
#define EXT_MOD_REMOVE   60
#define EXT_MOD_BELONG   61
#define EXT_MOD_MODEL    62

#define EXCEPT_BAD_ADDR                100
#define EXCEPT_INVALID_TROUBLE_CODE    101
#define EXCEPT_INVALID_ALARM_CODE      102
#define EXCEPT_INVALID_LEVEL_VARIABLE  103
#define EXCEPT_INVALID_LEVEL_VALUE     104
#define EXCEPT_INVALID_PW_VALUE        105
#define EXCEPT_INVALID_VALUE           106
#define EXCEPT_TOO_MANY_GROUPS         107
#define EXCEPT_INVALID_GROUP           108

#ifdef __SLCDEV_LIB_DEF__

struct ext_libdef slcdev_lib = {
	.name = "lib",
	.fncnt = 63,
	.fndef = {
		[EXT_PRINTF] = { .nm = "printf", .argmin = 1, .argmax = 32, .ret = 0 },
		[EXT_PRINT] = { .nm = "print", .argmin = 0, .argmax = 32, .ret = 0 },
		[EXT_RAND] = { .nm = "rand", .argmin = 0, .argmax = 0, .ret = 1 },
		[EXT_SQRT] = { .nm = "sqrt", .argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_LOG2] = { .nm = "log2", .argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_MEMRD] = { .nm = "memrd", .argmin = 1, .argmax = 1, .ret = 1 },

		[EXT_MODEL_NAME] = { .nm = "model_name", 
			.argmin = 1, .argmax = 1, .ret = 1 },

		[EXT_SENS_STATE] = { .nm = "sens_state", 
			.argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_SENS_ALARM] = { .nm = "sens_alarm", 
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_TROUBLE] = { .nm = "sens_trouble",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_LEVEL] = { .nm = "sens_level",
			.argmin = 2, .argmax = 3, .ret = 1 },
		[EXT_SENS_PW1] = { .nm = "sens_pw1",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_PW2] = { .nm = "sens_pw2",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_PW3] = { .nm = "sens_pw3",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_PW4] = { .nm = "sens_pw4",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_PW5] = { .nm = "sens_pw5",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_EN] = { .nm = "sens_en",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_TST] = { .nm = "sens_tst",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_TBIAS] = { .nm = "sens_tbias",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_ILAT] = { .nm = "sens_ilat",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_IMODE] = { .nm = "sens_imode",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_IRATE] = { .nm = "sens_irate",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_IPRE] = { .nm = "sens_ipre",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_SENS_CFG] = { .nm = "sens_cfg",
			.argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_SENS_AP] = { .nm = "sens_ap",
			.argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_SENS_CLEAR] = { .nm = "sens_clear",
			.argmin = 1, .argmax = 1, .ret = 0 },
		[EXT_SENS_INSERT] = { .nm = "sens_insert",
			.argmin = 2, .argmax = 5, .ret = 0 },
		[EXT_SENS_REMOVE] = { .nm = "sens_remove",
			.argmin = 2, .argmax = 5, .ret = 0 },
		[EXT_SENS_BELONG] = { .nm = "sens_belong",
			.argmin = 2, .argmax = 5, .ret = 1 },
		[EXT_SENS_MODEL] = { .nm = "sens_model",
			.argmin = 1, .argmax = 1, .ret = 1 },

		[EXT_MOD_STATE] = { .nm = "mod_state",
			.argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_MOD_ALARM] = { .nm = "mod_alarm",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_TROUBLE] = { .nm = "mod_trouble",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_LEVEL] = { .nm = "mod_level",
			.argmin = 2, .argmax = 3, .ret = 1 },
		[EXT_MOD_PW1] = { .nm = "mod_pw1",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_PW2] = { .nm = "mod_pw2",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_PW3] = { .nm = "mod_pw3",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_PW4] = { .nm = "mod_pw4",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_PW5] = { .nm = "mod_pw5",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_EN] = { .nm = "mod_en",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_TST] = { .nm = "mod_tst",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_TBIAS] = { .nm = "mod_tbias",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_ILAT] = { .nm = "mod_ilat",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_IMODE] = { .nm = "mod_imode",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_IRATE] = { .nm = "mod_irate",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_IPRE] = { .nm = "mod_ipre",
			.argmin = 1, .argmax = 2, .ret = 1 },
		[EXT_MOD_CFG] = { .nm = "mod_cfg",
			.argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_MOD_AP] = { .nm = "mod_ap",
			.argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_MOD_CLEAR] = { .nm = "mod_clear",
			.argmin = 1, .argmax = 1, .ret = 0 },
		[EXT_MOD_INSERT] = { .nm = "mod_insert",
			.argmin = 2, .argmax = 5, .ret = 0 },
		[EXT_MOD_REMOVE] = { .nm = "mod_remove",
			.argmin = 2, .argmax = 5, .ret = 0 },
		[EXT_MOD_BELONG] = { .nm = "mod_belong",
			.argmin = 2, .argmax = 5, .ret = 1 },
		[EXT_MOD_MODEL] = { .nm = "mod_model",
			.argmin = 1, .argmax = 1, .ret = 1 },
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

