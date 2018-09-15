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
 * @file mod_jtag..c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "jtag.h"
#include "module.h"

#include <sys/dcclog.h>

/***************************************************************************
 *
 * JTAG - Module operations
 *
 ***************************************************************************/

const char * const jtag_tap_state_tab[] = {
	"DREXIT2",
	"DREXIT1",
	"DRSHIFT",
	"DRPAUSE",
	"IRSELECT",
	"DRUPDATE",
	"DRCAPTURE",
	"DRSELECT",
	"IREXIT2",
	"IREXIT1",
	"IRSHIFT",
	"IRPAUSE",
	"IDLE",
	"IRUPDATE",
	"IRCAPTURE",
	"RESET"
};

enum {
	ID_NONE,
	ID_JTAG_CLK
};

int jtag_var_get(void * jtag, int var_id, value_t * val)
{
	DCC_LOG1(LOG_TRACE, "var_id=%d", var_id);

	switch (var_id) {
	case ID_JTAG_CLK:
		val->uint32 = jtag_tck_freq_get();
		break;
	default:
		return -1;
	}
	return 0;
}

int jtag_var_set(void * jtag, int var_id, const value_t * val)
{
	DCC_LOG1(LOG_TRACE, "var_id=%d", var_id);

	switch (var_id) {
	case ID_JTAG_CLK:
		jtag_tck_freq_set(val->uint32);
		break;
	default:
		return -1;
	}
	return 0;
}

/* FIXME : jtag driver interface definition */
int jtag_drv_init(void);
int jtag_drv_done(void);

int mod_jtag_init(void * jtag, int mod_id)
{
	int ret;

	DCC_LOG(LOG_TRACE, "...");

	if ((ret = jtag_drv_init()) < 0) {
		DCC_LOG(LOG_WARNING, "jtag_drv_init() failed!");
		return ret;
	}

	jtag_tck_freq_set(100000);
	jtag_rtck_freq_set(100000);
	/* force 0 on TRST */
	jtag_trst(true);
	/* force 0 in TMS and TDI */
	jtag_run_test(1, JTAG_TAP_IDLE);

	var_global_add(mod_id, "speed", TYPE_UINT32, ID_JTAG_CLK);

	DCC_LOG(LOG_TRACE, "done.");

	return 0;
}

int mod_jtag_done(void * jtag, int mod_id)
{
	DCC_LOG1(LOG_TRACE, "mod_id=%d", mod_id);
	
	jtag_drv_done();

	return JTAG_OK;
}

const struct module_def jtag_module = {
	.name = "jtag",
	.init = (module_init_t)mod_jtag_init,
	.done = (module_done_t)mod_jtag_done,
	.var_get = (module_var_get_t)jtag_var_get,
	.var_set = (module_var_set_t)jtag_var_set
};

int jtag_start(void)
{
	return module_register(&jtag_module, NULL);
}

