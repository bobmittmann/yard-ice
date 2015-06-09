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
 * @file cm3-udelay.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/delay.h>
#include <stdint.h>
#include <arch/cortex-m3.h>

static unsigned int cm3_get_ticks(void)
{
	return 0xffffffff - (CM3_SYSTICK->cvr << 8);
}

void cm3_udelay_calibrate(void)
{
	struct cm3_systick * systick = CM3_SYSTICK;
	uint32_t ticks1ms;
	uint32_t rvr;
	uint32_t csr;

	cm3_cpsid_i();
	rvr = systick->rvr;
	csr = systick->csr;

	systick->rvr = 0x00ffffff;
	systick->csr = SYSTICK_CSR_ENABLE;
	ticks1ms = cm3_systick_load_1ms << 8;

	udelay_calibrate(ticks1ms, cm3_get_ticks);

	systick->rvr = rvr;
	systick->csr = csr;

	cm3_cpsie_i();
}

