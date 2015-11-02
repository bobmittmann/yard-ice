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
#include "armice.h"
#include "nand.h"
#include "target/davinci.h"

#include <sys/dcclog.h>

#ifndef NAND_CHIP_MAX
#define NAND_CHIP_MAX 2
#endif

#ifndef NAND_DEV_MAX
#define NAND_DEV_MAX 2
#endif

nand_dev_t nand_dev[NAND_DEV_MAX];

static nand_chip_t nand_chip_tab[NAND_DEV_MAX * NAND_CHIP_MAX];
static uint8_t nand_dev_sel = 0;

nand_dev_t * nand_dev_get(unsigned int dev_id)
{
	if (dev_id >= NAND_DEV_MAX)
		return NULL;

	return &nand_dev[dev_id];
}

nand_chip_t * nand_chip_get(unsigned int dev_id, unsigned int chip_id)
{
	nand_dev_t * nand;

	if (dev_id >= NAND_DEV_MAX)
		return NULL;

	nand = &nand_dev[dev_id];

	if (chip_id >= nand->chip_cnt)
		return NULL;

	return &nand->chip[chip_id];
}

nand_dev_t *  nand_dev_select(unsigned int dev_id)
{
	if (dev_id >= NAND_DEV_MAX)
		return NULL;

	nand_dev_sel = dev_id;
	return &nand_dev[dev_id];
}

nand_chip_t * nand_chip_select(unsigned int dev_id, unsigned int chip_id)
{
	nand_dev_t * nand;

	if (dev_id >= NAND_DEV_MAX)
		return NULL;

	nand_dev_sel = dev_id;
	nand = &nand_dev[dev_id];

	if (chip_id >= nand->chip_cnt)
		return NULL;

	nand->chip_sel = chip_id;

	return &nand->chip[nand->chip_sel];
}

nand_dev_t * nand_dev_sel_get(void)
{
	return &nand_dev[nand_dev_sel];
}

nand_chip_t * nand_chip_sel_get(nand_dev_t * nand)
{
	return  &nand->chip[nand->chip_sel];
}

nand_dev_t *  nand_dev_register(const ice_drv_t * ice, unsigned int dev_id,
								uint32_t iobase, int bus_width,
								const nand_io_op_t * io, 
								 const nand_ecc_op_t * ecc)
{
	nand_dev_t * nand;
	int ret;

	if (dev_id >= NAND_DEV_MAX)
		return NULL;

	nand = &nand_dev[dev_id];

	/* NAND interface IO base addres */
	nand->iobase = iobase;
	/* NAND interface bus width */
	nand->bus_width = bus_width;
	nand->arg = (void *)ice;
	nand->io = io;
	nand->ecc = ecc;
	nand->is_open = 0;
	nand->ecc_en = 1;
	nand->bb_mark_en = 1;
	nand->bb_chk_en = 1;
	nand->chip_cnt = NAND_CHIP_MAX;
	nand->chip_sel = 0;
	nand->chip = &nand_chip_tab[dev_id * NAND_CHIP_MAX];

	DCC_LOG2(LOG_TRACE, "iobase=0x%08x bus_width=%d", 
			 iobase, bus_width);

	if ((ret = io->init(ice, nand, iobase, bus_width)) < 0) {
		DCC_LOG(LOG_WARNING, "io->init() fail!");
		return NULL;
	}

	return nand;
}


