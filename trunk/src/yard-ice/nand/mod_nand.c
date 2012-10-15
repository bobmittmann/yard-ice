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


#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nand.h"
#include "module.h"

#include <sys/dcclog.h>

/***************************************************************************
 *
 * NAND - Module operations
 *
 ***************************************************************************/

enum {
	ID_NONE = 0,
	ID_ECC_EN = 1,
	ID_BB_MARK_EN = 2,
	ID_BB_CHK_EN = 3
};

int nand_var_get(nand_dev_t * nand, int var_id, value_t * val)
{
	DCC_LOG1(LOG_TRACE, "var_id=%d", var_id);

	switch (var_id) {
	case ID_ECC_EN:
		val->uint32 = nand->ecc_en;
		break;
	case ID_BB_MARK_EN:
		val->uint32 = nand->bb_mark_en;
		break;
	case ID_BB_CHK_EN:
		val->uint32 = nand->bb_chk_en;
		break;
	default:
		return -1;
	}
	return 0;
}

int nand_var_set(nand_dev_t * nand, int var_id, const value_t * val)
{
	DCC_LOG1(LOG_TRACE, "var_id=%d", var_id);

	switch (var_id) {
	case ID_ECC_EN:
		nand->ecc_en = val->uint32 ? 1 : 0;
		break;
	case ID_BB_MARK_EN:
		nand->bb_mark_en = val->uint32 ? 1 : 0;
		break;
	case ID_BB_CHK_EN:
		nand->bb_chk_en = val->uint32 ? 1 : 0;
		break;
	default:
		return -1;
	}
	return 0;
}

int mod_nand_init(nand_dev_t * nand, int mod_id)
{
	DCC_LOG1(LOG_TRACE, "mod_id=%d", mod_id);

	var_global_add(mod_id, "ecc_en", TYPE_BOOL, ID_ECC_EN);
	var_global_add(mod_id, "bb_mark", TYPE_BOOL, ID_BB_MARK_EN);
	var_global_add(mod_id, "bb_chk", TYPE_BOOL, ID_BB_CHK_EN);

	return 0;
}

int mod_nand_done(nand_dev_t * nand, int mod_id)
{
	DCC_LOG1(LOG_TRACE, "mod_id=%d", mod_id);

	return JTAG_OK;
}

const struct module_def nand_module = {
	.name = "nand",
	.init = (module_init_t)mod_nand_init,
	.done = (module_done_t)mod_nand_done,
	.var_get = (module_var_get_t)nand_var_get,
	.var_set = (module_var_set_t)nand_var_set
};

extern nand_dev_t nand_dev[];

int mod_nand_start(void)
{
	return module_register(&nand_module, (void *)nand_dev);
}

