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
 * @file pflock.c
 * @brief Real-time trace
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <sys/dcclog.h>

#include "profclk.h"

/* ---------------------------------------------------------------------------
 * Profiling clock
 * ---------------------------------------------------------------------------
 */

void profclk_init(void)
{
	/* Enable trace */
	CM3_DCB->demcr |= DCB_DEMCR_TRCENA;
	/* Enable cycle counter */
	CM3_DWT->ctrl |= DWT_CTRL_CYCCNTENA;

	DCC_LOG1(LOG_TRACE, "DWT_CTRL=0x%08x", CM3_DWT->ctrl);
	DCC_LOG1(LOG_TRACE, "DWT_CYCCNT=0x%08x", CM3_DWT->cyccnt);
}

