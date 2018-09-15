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


#include "target.h"
#include "armice.h"
#include "nand.h"

#include <string.h>

#include <sys/dcclog.h>

static uint32_t nand_cache[128];
/* page */
static int32_t cache_pg = -1;
/* write pending */
static int8_t cache_wp = 0;

int nand_mem_write(ice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
				   const void * buf, ice_size_t len)
{
	nand_dev_t * nand;
	nand_chip_t * chip;
	uint8_t * cp = (uint8_t *)nand_cache;
	int pos;
	int ret;
	int pg;
	int end;
	int n;

	nand = nand_dev_sel_get();
	DCC_LOG1(LOG_TRACE, "chip=%d", nand->chip_sel);
	chip = &nand->chip[nand->chip_sel];

	pg = addr->offs / chip->data_size;
	end = (addr->offs + len) / chip->data_size;

	if ((pg != cache_pg) && (cache_wp)) {
		/* write back */
		cache_wp = 0;
		if ((ret = nand_page_write(nand, cache_pg, nand_cache)) < 0) {
			cache_pg = -1;
			return ret;
		}
	}

	pos = addr->offs % chip->data_size;
	n = chip->data_size - pos;
	n = (len < n) ? len : n;
	memcpy(&cp[pos], buf, n);

	if (end != pg) {
		/* write thrugh */
		cache_wp = 0;
		if ((ret = nand_page_write(nand, pg, nand_cache)) < 0) {
			cache_pg = -1;
			return ret;
		}
	} else
		cache_wp = 1;

	cache_pg = pg;

	return n;
}

int nand_mem_read(ice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
					 void * buf, ice_size_t len)
{
	nand_dev_t * nand;
	nand_chip_t * chip;
	uint8_t * cp = (uint8_t *)nand_cache;
	int pos;
	int ret;
	int pg;
	int n;

	nand = nand_dev_sel_get();
	DCC_LOG1(LOG_TRACE, "chip=%d", nand->chip_sel);
	chip = &nand->chip[nand->chip_sel];

	pg = addr->offs / chip->data_size;

	if (cache_wp) {
		/* write back */
		cache_wp = 0;
		if ((ret = nand_page_write(nand, cache_pg, nand_cache)) < 0) {
			cache_pg = -1;
			return ret;
		}
	}

	if (pg != cache_pg) {
		if ((ret = nand_page_read(nand, pg, nand_cache)) < 0) {
			cache_pg = -1;
			return ret;
		}
		cache_pg = pg;
	}

	pos = addr->offs % chip->data_size;
	n = chip->data_size - pos;
	n = (len < n) ? len : n;

	memcpy(buf, &cp[pos], n);

	return n;
}

int nand_mem_erase(ice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
				   ice_size_t len)
{
	nand_dev_t * nand;
	nand_chip_t * chip;
	int blk_size;;
	int blk;
	int ret;

	nand = nand_dev_sel_get();
	chip = &nand->chip[nand->chip_sel];

	DCC_LOG4(LOG_INFO, "chip=%d base=%08x offs=%d len=%d", 
			 nand->chip_sel, addr->base, addr->offs, len);

	if (cache_wp) {
		/* write back */
		cache_wp = 0;
		if ((ret = nand_page_write(nand, cache_pg, nand_cache)) < 0) {
			DCC_LOG(LOG_WARNING, "nand_page_write() failed!");
			cache_pg = -1;
			return ret;
		}
	}

	cache_pg = -1;

	blk_size = (chip->data_size * chip->pgs_per_blk);
	blk = (addr->offs / blk_size);

	if (addr->offs % blk_size) {
		DCC_LOG1(LOG_MSG, "offs(%d) is not block boundary.", addr->offs);
		return len;
	}

	if (nand_block_erase(nand, blk) < 0) {
		DCC_LOG(LOG_WARNING, "nand_block_erase() failed!");
		return -1;
	}

	return len;
}

const struct ice_mem_oper nand_mem_oper = {
	.read = nand_mem_read,
	.write = nand_mem_write, 
	.erase = nand_mem_erase
};

#if 0
nand_dev_t *  nand_mem_map(const ice_drv_t * ice, uint32_t iobase,
						   const nand_io_op_t * io, 
						   const nand_ecc_op_t * ecc, 
						   const nand_bb_op_t * bb)
{
	int ret;

	DCC_LOG1(LOG_TRACE, "iobase=0x%08x", iobase);

	/* Chip configuration */
	if ((ret = nand_chip_init(ice, &chip, io, iobase)) < 0) {
		return NULL;
	}

	if ((ret = nand_detect(ice, &chip, io)) < 0) {
		return NULL;
	}

	/* NAND operations */
	if ((ret = nand_open(&nand, ice, &chip, io, ecc, bb)) < 0) {
		return NULL;
	}

	return &nand;
}
#endif

