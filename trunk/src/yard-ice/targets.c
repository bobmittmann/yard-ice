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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <string.h>

#include "target.h"
#include "target/arm.h"
#include "target/a7e10x2.h"
#include "target/at91sam7.h"
#include "target/at91sam9.h"
#include "target/jtagtool3.h"
#include "target/davinci.h"
#include "target/stellaris.h"
#include "target/stm32f.h"

#include "dbglog.h"
#include <stdio.h>

const struct target_sym void_sym[] = {
	{0, "", 0, 0}
};

const struct target_info * const target_db[] = {
	&target_null,
	&a7e10x2,
	&jtagtool3,
	&at91sam9260, 
	&at91sam7x256,
	&stm32f207ve,
/*
	&at91sam7s64,
	&a7e10x2_2m,
	&target_arm7le,
	&target_arm9le, 
	&ti_dm365,
	&ti_dm365hs,
	&stellaris_lm3s6965,
	&at91sam7s32,
	&at91sam7s128,
	&at91sam7s256,
	&at91sam7xc256,
	&target_arm7be, */
	NULL
};

